#ifndef WIDGETATTRIBUTES_H
#define WIDGETATTRIBUTES_H

/**
 * @file WidgetAttributes.h
 * @brief The widget attribute system for lvgl-based UI components.
 * 
 * This file defines an attribute system for describing widget properties.
 * This system supports both built-in widgets and custom widget implementations.
 * 
 */

#include <lvgl.h>
#include <stdint.h> // primatives

namespace Utils {
  namespace Widget {

    /**
     * @enum Type
     * @brief Defines the widget type classification.
     * 
     */
    enum class Type {
      DEFAULT, // Fallback type for unspecified widgets.
      LABEL, // An lvgl label widget.
      GRAPH // Custom oscilloscope/graph widget with grid lines and origin markers.
    };

    /**
     * @enum Role
     * @brief Describes the functional purpose of the widget in the UI.
     * 
     */
    enum class Role {
      DECORATIVE, // Pure visual element, no user interaction.
      INFORMATIVE, // Displays data to user (labels, graphs, indicators).
      FUNCTIONAL, // Interactive control element (buttons, sliders).
      FEEDBACK, // Provides response to user actions (progress bars).
      BRANDING, // Logo, company info, visual identity elements.
      BACKGROUND, // Base layer element, typically non-interactive.
      PREVIEW, // Shows preview or thumbnail content.
      DEFAULT // Fallback role for unspecified widgets.
    };

    /**
     * @enum Color
     * @brief RGB565 color palette for widget styling.
     * 
     * Format: RRRRR GGGGGG BBBBB (5-6-5 bits)
     */
    enum class Color {
      DEFAULT = 0x0000, // Black (0,0,0) - fallback color.
      BLACK = 0x0000, // Black (0,0,0).
      WHITE = 0xFFFF, // White (255,255,255).
      RED = 0xF800, // Red (255, 0, 0).
      GREEN = 0x07E0, // Green (0, 255, 0).
      BLUE = 0x001F, // Blue (0, 0, 255).
      LIGHT_GREY = 0x2104 // Light grey (33, 16, 33).
    };

    /**
     * @struct Spacing
     * @brief Controls whitespace around widget content.
     * 
     */
    struct Spacing {
      int16_t margin = 0; // External spacing (pixels) - distance from other widgets.
      int32_t padding = 0; // Internal spacing (pixels) - distance from content to border.
    };

    /**
     * @struct Position
     * @brief Defines widget placement and alignment on screen.
     * 
     */
    struct Position {
      int16_t offsetX = 0; // Horizontal offset (pixels) from alignment point.
      int16_t offsetY = 0; // Vertical offset (pixels) from alignment point. 
      lv_align_t alignment = LV_ALIGN_DEFAULT; // LVGL alignment preset (center, top-left, etc.).
    };

    /**
     * @struct Background
     * @brief Controls widget background appearance.
     * 
     */
    struct Background {
      Color backgroundColor = Color::DEFAULT; // Background fill color (RGB565).
      uint8_t backgroundOpacity = 0; // Alpha value (0=transparent, 255=opaque).
    };

    /**
     * @struct Border
     * @brief Configures widget border styling.
     * 
     */
    struct Border {
      int8_t borderWidth = 0; // Border thickness in pixels.
      Color borderColor = Color::DEFAULT; // Border color (RGB565).
      uint8_t borderOpacity = 0; // Alpha value (0=transparent, 255=opaque).
      lv_border_side_t borderSide = LV_BORDER_SIDE_FULL; // Which sides to draw border on.
    };

    /**
     * @struct Outline
     * @brief Creates external outline effect around widget.
     * 
     */
    struct Outline {
      int8_t outlineWidth = 0; // Outline thickness in pixels.
      Color outlineColor = Color::DEFAULT; // Outline color (RGB565).
      uint8_t outlineOpacity = 0; // Alpha value (0=transparent, 255=opaque).
    };

    /**
     * @struct Boundary
     * @brief Rectangular region in absolute screen coordinates.
     * 
     * Defines a custom widget's model.
     */
    struct Boundary {
      int16_t x1 = 0;
      int16_t y1 = 0;
      int16_t x2 = 0;
      int16_t y2 = 0;
    };

    /**
     * @enum SizingMode
     * @brief Describes the method of determining how to scale widget geometry.
     * 
     */
    enum class SizingMode {
      ABSOLUTE, // Use width/height directly (geometry).
      AREA_PERCENT, // Use areaScaling (current - sqrt based).
      DIMENSION_PERCENT // Use dimensionScaling (linear %).
    };

    /**
     * @struct Size
     * @brief Controls widget dimensions and custom geometry.
     * 
     */
    struct Geometry {
      SizingMode mode = SizingMode::ABSOLUTE;
      int16_t width = 0; // Final or input width.
      int16_t height = 0; // Final or input height.
      int16_t percentSize = 0; // Percentage of screen area (0-100) for custom widgets.
      Boundary boundary; // Widget's rendering area in screen coordinates.
    };

    /**
     * @struct Text
     * @brief Typography and text rendering properties.
     * 
     */
    struct Text {
      const lv_font_t* font = nullptr; // Pointer to LVGL font (nullptr = default font).
      Color textColor = Color::DEFAULT; // Text color (RGB565).
      uint8_t textOpacity = 255; // Text opacity (0=transparent, 255=opaque).
      int8_t letterSpacing = 0; // Horizontal spacing between characters (pixels).
      int8_t lineSpacing = 0; // Vertical spacing between text lines (pixels).
      lv_text_align_t textAlign = LV_TEXT_ALIGN_AUTO; // Text alignment within widget bounds.
      lv_text_decor_t textDecor = LV_TEXT_DECOR_NONE; // Text decoration (underline, strikethrough).
    };

    /**
     * @struct Label
     * @brief Text display widget configuration.
     * 
     */
    struct Label {
      const char* text = nullptr; // Text content to display (null-terminated string).
      lv_label_long_mode_t longMode = LV_LABEL_LONG_WRAP; // Behavior when text is too long (wrap, scroll, etc.).
      bool recolor = false; // Enable color codes in text (e.g. FF0000 red text).
    };

    /**
     * @struct Part
     * @brief LVGL widget part selector for targeted styling.
     */
    struct Part {
      lv_part_t region = LV_PART_MAIN; // Widget part to style (main, indicator, knob, scrollbar, etc.).
    };

    /**
     * @struct Behavior
     * @brief Defines widget interaction capabilities.
     * 
     */
    struct Behavior {
      bool clickable = false; // Responds to touch/click events.
      bool scrollable = false; //  Can be scrolled if content overflows.
      bool focusable = false; // Can receive keyboard/encoder focus.
    };

    /**
     * @struct Graph
     * @brief Specialized configuration for oscilloscope/graph widgets
     * 
     * Defines a 4-quadrant Cartesian coordinate system with customizable
     * grid lines and origin markers. Used for displaying waveforms and measurements.
     */
    struct Graph {
      Color originColor = Color::DEFAULT; // Color of X/Y axis origin lines.
      int8_t originThickness = 0; // Thickness of origin axis lines (pixels).
      Color gridlineColor = Color::DEFAULT; // Color of background grid lines.
      int8_t gridlineThickness = 0; // Thickness of grid lines (pixels).
      int8_t xDivisionQty = 0; // Number of vertical grid divisions.
      int8_t yDivisionQty = 0; // Number of horizontal grid divisions.
      int8_t xOriginIndex = 0; // Which vertical line serves as X origin (0-based).
      int8_t yOriginIndex = 0; // Which horizontal line serves as Y origin (0-based).
    };

    /**
     * @struct Attributes
     * @brief Complete widget configuration container.
     * 
     * Aggregates all widget properties into a single structure.
     * 
     * Usage:
     * - Create instance with appropriate type/role
     * - Configure visual properties (spacing, position, colors)
     * - Set behavioral flags (clickable, draggable, etc.)
     */
    struct Attributes {
      // Metadata
      bool isCustom = false; // true=custom widget, false=LVGL built-in.
      Type type = Type::DEFAULT; // Widget type classification.
      Role role = Role::DEFAULT; // Functional role in UI.
      const char* name = ""; // Text content to display (null-terminated string).

      // Layout & appearance.
      Spacing spacing;
      Position position;
      Background background;
      Border border;
      Outline outline;
      Geometry geometry;
      Text text;
      Label label;
      Part part;
      Behavior behavior;

      /**
       * @union Data
       * @brief Type-specific widget configuration data
       * 
       * Contains specialized settings for different widget types.
       * Only one member is active at a time based on the 'type' field.
       */
      union Data {
        Graph graph; // Configuration for oscilloscope/graph widgets.
        Data() {} // Default constructor for union.
      } data;
      
      Attributes() {} // Default constructor initializes all fields to defaults.
    };
  }
}

#endif