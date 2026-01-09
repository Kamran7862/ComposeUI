#ifndef SCREEN_H
#define SCREEN_H

#include <lvgl.h> 
#include <Arduino.h>
#include "utils/Math.h"
#include "utils/LVGL.h"
#include "display/Display.h"
#include "ui/pool/WidgetPool.h"
#include "display/FlushCallback.h"
#include "utils/WidgetAttributes.h"
#include "ui/registry/WidgetRegistry.h"

/**
 * @enum ScreenState
 * @brief States of the screen's state machine.
 */
enum class ScreenState {
  UNINITIALIZED, // The screen is not initialized or services not set.
  SERVICES_SET, // The screen's services (WidgetRegistry, WidgetPool) have been set. 
  ERROR_SERVICES, // There is a problem with the screen's services.
  WIDGETS_REGISTERED, // The widget geometry calculations completed.
  ERROR_REGISTRATION, // There was an error calculating widget geometry.
  ATTRIBUTES_SET, // LVGL objects have been created and attributes have been applied.
  ERROR_ATTRIBUTES, // An error occurred while applying widget attributes.
  COMPLETE // All widgets successfully rendered to display.
};

/**
 * @class Screen
 * @brief Central coordinator for lvgl display management and widget orchestration
 * 
 * The Screen class is the primary interface between hardware display drivers
 * and the lvgl graphics library. It manages widget lifecycle, and coordinates between subsystems.
 * 
 * @tparam Driver The hardware display driver type (e.g., TFT_HX8357D)
 */
template<typename Driver>
class Screen {

private:

  ScreenState state = ScreenState::UNINITIALIZED; // The state of the screen.

  Display<Driver>& display; // A reference to a display struct.
  WidgetRegistry* widgetRegistry = nullptr; // A pointer to the singleton WidgetRegistry service.
  WidgetPool* widgetPool = nullptr; // A pointer to the singleton WidgetPool service.

public:

  /**
   * @brief Initialize an lvgl display with a hardware display driver.
   * 
   * @param display Reference to a configured Display<Driver> wrapper containing hardware driver
   */
  Screen(Display<Driver>& display)
    : display(display)
  {
    lv_init(); // Initialize lvgl.
    lv_tick_set_cb(millis); // Configure lvgl timing with arduino millisecond counter.

    // Delete any existing lvgl display to prevent conflicts.
    lv_display_t* activeDisplay = lv_display_get_default(); // Get the active display pointer. 
    if (activeDisplay) {
        lv_display_delete(activeDisplay);
    }

    // Create and configure an lvgl display.
    lv_display_t* lvglDisplay = lv_display_create(display.getHorizontalRes(), display.getVerticalRes()); // Create the display.
    lv_display_set_user_data(lvglDisplay, &display); // Register the display<driver> objects address.
    lv_display_set_buffers(lvglDisplay, display.getBuffer(), nullptr, display.getBufferSizeBytes(), LV_DISPLAY_RENDER_MODE_PARTIAL); // Configure lvgl screen buffer for writing to the display.
    lv_display_set_flush_cb(lvglDisplay, my_flush_cb<Driver>); // Register the flushcallback<Driver> function for the specific display.
    lv_display_set_rotation(lvglDisplay, LV_DISPLAY_ROTATION_0); // Set the lvgl rotation to match the Display<Driver>.
    lv_display_set_default(lvglDisplay); // Set the current lvgl display as the primary display.
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0); // Set the screen background black.
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0); // Alter the background opacity to cover previously drawn layers.
  }

  /**
   * @brief Configure the screen's services.
   * 
   * @param widgetRegistry Pointer to WidgetRegistry containing attribute definitions.
   * @param widgetPool Pointer to WidgetPool containing widget instances.
   */
  void setServices(WidgetRegistry* widgetRegistry, WidgetPool* widgetPool) {
    this->widgetRegistry = widgetRegistry;
    this->widgetPool = widgetPool;

    // Validate both services are available before proceeding.
    if (this->widgetRegistry && this->widgetPool) {
      this->state = ScreenState::SERVICES_SET;
    } else {
      this->state = ScreenState::ERROR_SERVICES;
    }
  }

  // Determines the widget's geometry from registry attributes.
  void setRegistry() {
    namespace Json = Utils::LVGL::Debug::Json;
    bool errorRegistration = false;

    Json::reset();
    Json::objectStart();
    Json::action("Screen", "Display", "Retrieving display information");
    Json::status("Success");
    Json::displayInfo(display.getHorizontalRes(), display.getVerticalRes());
    Serial.println();

    // Process each widget for geometry calculation.
    for (auto& entry : *widgetRegistry) {
      Utils::Widget::Attributes* attributes = entry.value;

      if (!attributes) {
        Json::status("Error");
        Json::field("message", "Attributes are nullptr", false);
        Json::objectEnd();
        errorRegistration = true;
        break;
      }

      // Calculate absolute dimensions using area-based scaling.
      int16_t width;
      int16_t height;
      Json::action("Display", "Registry", "Determining geometry");
      switch (attributes->geometry.mode) {

        case Utils::Widget::SizingMode::ABSOLUTE:
          width = attributes->geometry.width;
          height = attributes->geometry.height;
          break;

        case Utils::Widget::SizingMode::AREA_PERCENT:
          width = Utils::Math::areaScaling(attributes->geometry.percentSize, display.getHorizontalRes());
          height = Utils::Math::areaScaling(attributes->geometry.percentSize, display.getVerticalRes());
          attributes->geometry.width = width;
          attributes->geometry.height = height;
          break;

        case Utils::Widget::SizingMode::DIMENSION_PERCENT:
          width = Utils::Math::dimensionScaling(attributes->geometry.percentSize, display.getHorizontalRes());
          height = Utils::Math::dimensionScaling(attributes->geometry.percentSize, display.getVerticalRes());
          attributes->geometry.width = width;
          attributes->geometry.height = height;
          break;

        default:
          width = attributes->geometry.width;
          height = attributes->geometry.height;
          break;
      }

      Json::status("Success");
      Json::objectStart("widget");
      Json::field("name", attributes->name);

      // Configure the boundary areas for custom widgets.
      if (attributes->isCustom) {
        attributes->geometry.boundary = Utils::Math::convertArea(width, height);
        Json::geometry(width, height, true);
        Json::boundary(attributes->geometry.boundary.x1, attributes->geometry.boundary.y1, attributes->geometry.boundary.x2, attributes->geometry.boundary.y2);
        Json::objectEnd(); // close geometry
      } else {
        Json::geometry(width, height, false);
        Json::field("boundary", "not configured", false);
        Json::objectEnd(); // close geometry
      }
      Json::objectEnd(); // close widget
    }

    Json::objectEnd();
    Serial.println();

    if (errorRegistration) {
      Json::reset();
      Json::objectStart();
      Json::field("error", "Failed to apply geometry", false);
      Json::objectEnd();
      this->state = ScreenState::ERROR_REGISTRATION;
    } else {
      this->state = ScreenState::WIDGETS_REGISTERED;
    }
  }

  /**
   * @brief Create/configure lvgl widget objects from registry attributes.
   * 
   * Iterates through all widget pool instances, creates LVGL objects,
   * and applies styling from the widget registry.
   */
  void setWidgets() {
    namespace Json = Utils::LVGL::Debug::Json;
    bool errorAttributes = false;

    // Grab a widget instance from the pool.
    for (auto& entry : *widgetPool) {
      Utils::Widget::Type type = entry.key;
      Widget* widget = entry.value;

      Json::reset();
      Json::objectStart();
      Json::action("Pool", "LVGL", "Creating lvgl object and associating a pointer");

      // Create the lvgl object/lvgl pointer.
      lv_obj_t* thisWidget = Utils::LVGL::createWidget(type, lv_scr_act());
      if (!thisWidget) {
        Json::error("Failed to create LVGL object");
        errorAttributes = true;
        break;
      }
      
      Json::status("Success");
      widget->setLVGL(thisWidget); // Set the lvgl object pointer in the base class.

      Json::action("Registry", "LVGL", "Setting widget attributes");
      Json::status("Success");
      Json::objectStart("widget");

      // If the widget is a custom type it will have a custom draw callback.
      auto callback = widget->getCallback();
      if (callback) {
        Json::field("type", "Custom");
        lv_obj_set_user_data(thisWidget, widget);
        lv_obj_add_event_cb(thisWidget, callback, LV_EVENT_DRAW_MAIN, widget);
      } else {
        Json::field("type", "Built-in");
      }

      // Use the type from the widget in the pool to retrieve the attributes in the registry.
      Utils::Widget::Attributes* attributes = this->widgetRegistry->getAttribute(type);
      if (!attributes) {
        Json::field("error", "Failed to retrieve attributes", false);
        Json::objectEnd(); // close widget
        Json::objectEnd(); // close root
        errorAttributes = true;
        break;
      }

      Json::field("name", attributes->name);

      // Layout spacing configuration.
      if (attributes->spacing.padding != 0 || attributes->spacing.margin != 0) {
        lv_obj_set_style_pad_all(thisWidget, attributes->spacing.padding, attributes->part.region);
        lv_obj_set_style_margin_all(thisWidget, attributes->spacing.margin, attributes->part.region);
        Json::field("padding", attributes->spacing.padding);
        Json::field("margin", attributes->spacing.margin);
      } else {
        Json::field("spacing", "ignored");
      }

      // Position and alignment setup.
      lv_obj_set_align(thisWidget, attributes->position.alignment);
      lv_obj_set_x(thisWidget, attributes->position.offsetX);
      lv_obj_set_y(thisWidget, attributes->position.offsetY);
      Json::field("position_offset_x", attributes->position.offsetX);
      Json::field("position_offset_y", attributes->position.offsetY);

      // Background appearance styling.
      if (attributes->background.backgroundColor != Utils::Widget::Color::DEFAULT || attributes->background.backgroundOpacity != 0) {
        uint16_t backgroundColor = Utils::Math::enum565(attributes->background.backgroundColor);
        lv_obj_set_style_bg_color(thisWidget, Utils::Math::convert565(backgroundColor), attributes->part.region);
        lv_obj_set_style_bg_opa(thisWidget, attributes->background.backgroundOpacity, attributes->part.region);
        Json::fieldHex("background_color", backgroundColor);
        Json::field("background_opacity", attributes->background.backgroundOpacity);
      } else {
        Json::field("background_appearance", "ignored");
      }

      // Border styling configuration.
      if (attributes->border.borderWidth != 0 || attributes->border.borderOpacity != 0 || attributes->border.borderColor != Utils::Widget::Color::DEFAULT) {
        uint16_t borderColor = Utils::Math::enum565(attributes->border.borderColor);
        lv_obj_set_style_border_width(thisWidget, attributes->border.borderWidth, attributes->part.region);
        lv_obj_set_style_border_color(thisWidget, Utils::Math::convert565(borderColor), attributes->part.region);
        lv_obj_set_style_border_opa(thisWidget, attributes->border.borderOpacity, attributes->part.region);
        lv_obj_set_style_border_side(thisWidget, attributes->border.borderSide, attributes->part.region);
        Json::field("border_width", attributes->border.borderWidth);
        Json::fieldHex("border_color", borderColor);
        Json::field("border_opacity", attributes->border.borderOpacity);
        Json::field("border_side", attributes->border.borderSide);
      } else {
        Json::field("border", "ignored");
      }

      // Outline effect styling.
      if (attributes->outline.outlineWidth != 0 || attributes->outline.outlineOpacity != 0 || attributes->outline.outlineColor != Utils::Widget::Color::DEFAULT) {
        uint16_t outlineColor = Utils::Math::enum565(attributes->outline.outlineColor);
        lv_obj_set_style_outline_width(thisWidget, attributes->outline.outlineWidth, attributes->part.region);
        lv_obj_set_style_outline_color(thisWidget, Utils::Math::convert565(outlineColor), attributes->part.region);
        lv_obj_set_style_outline_opa(thisWidget, attributes->outline.outlineOpacity, attributes->part.region);
        Json::field("outline_width", attributes->outline.outlineWidth);
        Json::fieldHex("outline_color", outlineColor);
        Json::field("outline_opacity", attributes->outline.outlineOpacity);
      } else {
        Json::field("outline", "ignored");
      }

      // The widget dimensions from the calculated geometry.
      lv_obj_set_width(thisWidget, attributes->geometry.width);
      lv_obj_set_height(thisWidget, attributes->geometry.height);
      Json::field("widget_width", attributes->geometry.width);
      Json::field("widget_height", attributes->geometry.height);

      // Typography and text rendering.
      if (attributes->text.font || attributes->text.textColor != Utils::Widget::Color::DEFAULT || attributes->text.letterSpacing != 0 || attributes->text.lineSpacing != 0) {
        if (attributes->text.font) {
          lv_obj_set_style_text_font(thisWidget, attributes->text.font, attributes->part.region);
        }
        uint16_t textColor = Utils::Math::enum565(attributes->text.textColor);
        lv_obj_set_style_text_color(thisWidget, Utils::Math::convert565(textColor), attributes->part.region);
        lv_obj_set_style_text_opa(thisWidget, attributes->text.textOpacity, attributes->part.region);
        lv_obj_set_style_text_letter_space(thisWidget, attributes->text.letterSpacing, attributes->part.region);
        lv_obj_set_style_text_line_space(thisWidget, attributes->text.lineSpacing, attributes->part.region);
        lv_obj_set_style_text_align(thisWidget, attributes->text.textAlign, attributes->part.region);
        lv_obj_set_style_text_decor(thisWidget, attributes->text.textDecor, attributes->part.region);
        Json::fieldHex("text_color", textColor);
        Json::field("text_opacity", attributes->text.textOpacity);
        Json::field("letter_spacing", attributes->text.letterSpacing);
        Json::field("line_spacing", attributes->text.lineSpacing);
      } else {
        Json::field("typography", "ignored");
      }

      // Label-specific configuration.
      if (attributes->label.text) {
        lv_label_set_text(thisWidget, attributes->label.text);
        lv_label_set_long_mode(thisWidget, attributes->label.longMode);
        if (attributes->label.recolor) {
          lv_label_set_recolor(thisWidget, true);
        }
        Json::field("label_text", attributes->label.text);
        Json::fieldBool("label_recolor", attributes->label.recolor);
      } else {
        Json::field("label_text", "ignored");
      }

      // Interactive behavior flags.
      if (attributes->behavior.clickable) {
        lv_obj_add_flag(thisWidget, LV_OBJ_FLAG_CLICKABLE);
      }
      if (attributes->behavior.scrollable) {
        lv_obj_add_flag(thisWidget, LV_OBJ_FLAG_SCROLLABLE);
      }
      if (attributes->behavior.focusable) {
        lv_obj_add_flag(thisWidget, LV_OBJ_FLAG_CLICK_FOCUSABLE);
      }
      Json::fieldBool("clickable", attributes->behavior.clickable);
      Json::fieldBool("scrollable", attributes->behavior.scrollable);
      Json::fieldBool("focusable", attributes->behavior.focusable, false);

      Json::objectEnd(); // close widget
      Json::objectEnd(); // close root
      Serial.println();
    }

    // Update the screen's state.
    if (errorAttributes) {
      Json::reset();
      Json::objectStart();
      Json::field("error", "Failed to apply attributes", false);
      Json::objectEnd();
      this->state = ScreenState::ERROR_ATTRIBUTES;
    } else {
      this->state = ScreenState::ATTRIBUTES_SET;
    }
  }

  /**
   * @brief Force widget rendering and screen refresh.
   * 
   * Triggers lvgl layout updates and invalidates all widgets to schedule
   * redraw operations.
   */
  void drawWidgets() {
    // Force rendering for each widget in the pool.
    for (auto& entry : *widgetPool) {
      Widget* widget = entry.value; // Get widget instance from pool.
      lv_obj_t* thisWidget = widget->getLVGL(); // Get associated LVGL object.
      lv_obj_update_layout(lv_scr_act());
      lv_obj_invalidate(thisWidget);
    }

    // Update state to indicate rendering is complete.
    this->state = ScreenState::COMPLETE;
  }

  /**
   * @brief Print general lvgl screen information using simple format.
   */
  void printLVGL() {
    namespace Debug = Utils::LVGL::Debug;
    
    // Get lvgl's active screen.
    const lv_obj_t* activeScreen = lv_scr_act();
    if (!activeScreen) {
      Serial.println("Active screen not set (lv_scr_act() == NULL)");
      return;
    }

    // Print the coordinate space of the active screen (x1, y1, x2, y2).
    lv_area_t screenArea; 
    lv_obj_get_coords(activeScreen, &screenArea);
    Debug::printArea("Screen Coordinates", screenArea);
    
    // Derive/Print the screens dimensions.
    const int16_t screenWidth = screenArea.x2 - screenArea.x1 + 1;
    const int16_t screenHeight = screenArea.y2 - screenArea.y1 + 1;
    Serial.print("Display Dimensions (W x H): ");
    Serial.print(screenWidth); Serial.print(" x "); Serial.println(screenHeight);

    // Print the screens's styling.
    const lv_opa_t transparency = lv_obj_get_style_bg_opa(activeScreen, LV_PART_MAIN); 
    const lv_opa_t borderTransparency = lv_obj_get_style_border_opa(activeScreen, LV_PART_MAIN);
    Serial.print("Transparency (bg_opa): "); Serial.println((int)transparency);
    Serial.print("Border Transparency (border_opa): "); Serial.println((int)borderTransparency);
    Serial.print("Color Depth (LV_COLOR_DEPTH): "); Serial.println(LV_COLOR_DEPTH);
    
    // Make sure that the screen's origin is (0, 0); where lvgl draws from.
    if (screenArea.x1 != 0 || screenArea.y1 != 0) {
      Serial.println("WARNING: The Screen's origin is not (0,0)");
    }
  }

  // Print LVGL widget information using JSON format.
  void printWidgets() {
    namespace Json = Utils::LVGL::Debug::Json;
    
    Json::reset();
    Json::objectStart();
    Json::action("Pool", "LVGL", "Reading widget information");

    for (auto& entry : *widgetPool) {
      Utils::Widget::Type type = entry.key;
      Widget* widget = entry.value;
      lv_obj_t* thisWidget = widget->getLVGL(); // Get the lvgl object stored in the widget.
      Utils::Widget::Attributes* attributes = this->widgetRegistry->getAttribute(type); // Get the widgets attributes using its type from the pool.

      Json::objectStart("widget");
      Json::field("name", attributes->name);
      Json::widgetCoords(thisWidget);
      Json::objectEnd(); // close widget
    }
    
    Json::objectEnd(); // close root
    Serial.println();
  }

  /**
   * @brief Creates simple colored rectangles at known coordinates to validate lvgl's alignment.
   * 
   * Will produce a red pixel at the origin, green bottom left, blue bottom right.
   *
   * @param isPortrait If true, swaps width/height for portrait orientation testing.
   */
  void debugDraw(bool isPortrait = false) {
    // Use display dimensions - getHorizontalRes/getVerticalRes are post-rotation values
    const int32_t screenWidth = isPortrait ? display.getVerticalRes() : display.getHorizontalRes();
    const int32_t screenHeight = isPortrait ? display.getHorizontalRes() : display.getVerticalRes();
    constexpr int16_t pixelBlockSize = 10;

    // Red pixel block at top-left (0, 0)
    lv_obj_t* redBlock = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(redBlock);
    lv_obj_set_size(redBlock, pixelBlockSize, pixelBlockSize);
    lv_obj_set_pos(redBlock, 0, 0);
    lv_obj_set_style_bg_color(redBlock, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_bg_opa(redBlock, LV_OPA_COVER, 0);

    // Green pixel block at bottom-left
    lv_obj_t* greenBlock = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(greenBlock);
    lv_obj_set_size(greenBlock, pixelBlockSize, pixelBlockSize);
    lv_obj_set_pos(greenBlock, 0, screenHeight - pixelBlockSize);
    lv_obj_set_style_bg_color(greenBlock, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_bg_opa(greenBlock, LV_OPA_COVER, 0);

    // Blue pixel block at bottom-right
    lv_obj_t* blueBlock = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(blueBlock);
    lv_obj_set_size(blueBlock, pixelBlockSize, pixelBlockSize);
    lv_obj_set_pos(blueBlock, screenWidth - pixelBlockSize, screenHeight - pixelBlockSize);
    lv_obj_set_style_bg_color(blueBlock, lv_color_hex(0x0000FF), 0);
    lv_obj_set_style_bg_opa(blueBlock, LV_OPA_COVER, 0);
  }

  // Get the current screen state for monitoring workflow
  ScreenState& getState() {
    return this->state;
  }
};

#endif