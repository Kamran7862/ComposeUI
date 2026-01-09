#ifndef LVGL_UTILS_H
#define LVGL_UTILS_H

/**
 * @file LVGL.h
 * @brief LVGL utility functions.
 * 
 * This file provides a collection of functions that assist
 * with lvgl type operations.
 * 
 */

#include <lvgl.h>
#include <cstring> // for color checking
#include <stdint.h> // primitives
#include <Arduino.h> // for printing
#include "utils/WidgetAttributes.h" // for Widget::Type enum

namespace Utils {
  namespace LVGL {

    /**
     * @brief Create the appropriate LVGL widget object based on type.
     * 
     * Returns the correct LVGL widget for built-in types (label, button, etc.)
     * or a base lv_obj for custom/unknown types.
     * 
     * @param type The widget type from Utils::Widget::Type enum.
     * @param parent The parent LVGL object.
     * @return Pointer to the created LVGL object.
     */
    inline lv_obj_t* createWidget(Utils::Widget::Type type, lv_obj_t* parent) {
      switch (type) {
        case Utils::Widget::Type::LABEL:
          return lv_label_create(parent);
        default:
          return lv_obj_create(parent);
      }
    }

    // Debugging utilities for LVGL draw events and rendering operations.
    namespace Debug {

      /**
       * @brief Print an lvgl area.
       * 
       * @param label Text to identify the area.
       * @param area The lvgl area structure.
       */
      inline void printArea(const char* label, const lv_area_t& area) {
        Serial.print(label); Serial.print(": (");
        Serial.print(area.x1); Serial.print(","); Serial.print(area.y1);
        Serial.print(") -> (");
        Serial.print(area.x2); Serial.print(","); Serial.print(area.y2);
        Serial.println(")");
      }

      /**
       * @brief Print a widget's clipping/bounding area.
       * 
       * @param widget The lvgl widget object.
       */
      inline void printClippingArea(lv_obj_t* widget) {
        lv_area_t clip;
        lv_obj_get_coords(widget, &clip);
        printArea("CUSTOM WIDGET CLIPPING AREA", clip);
      }

      /**
       * @brief Print a line segment with a label.
       * 
       * @param label Text to identify the line segment.
       * @param x1 Starting X coordinate.
       * @param y1 Starting Y coordinate.
       * @param x2 Ending X coordinate.
       * @param y2 Ending Y coordinate.
       */
      inline void printLine(const char* label, int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
        Serial.print(label); Serial.print(": (");
        Serial.print(x1); Serial.print(","); Serial.print(y1);
        Serial.print(") -> (");
        Serial.print(x2); Serial.print(","); Serial.print(y2);
        Serial.println(")");
      }

      /**
       * @brief Print a point with a label.
       * 
       * @param label Text to identify the point.
       * @param x X coordinate.
       * @param y Y coordinate.
       */
      inline void printPoint(const char* label, int16_t x, int16_t y) {
        Serial.print(label); Serial.print(": (");
        Serial.print(x); Serial.print(","); Serial.print(y);
        Serial.println(")");
      }

      /**
       * @brief Prints the widget dimensions.
       * 
       * @param widget The lvgl widget.
       */
      inline void printDimensions(lv_obj_t* widget) {
        Serial.print("Dimensions: ");
        Serial.print(lv_obj_get_width(widget)); Serial.print("x"); Serial.println(lv_obj_get_height(widget));
      }

      /**
       * @brief Print lvgl display configuration.
       * 
       * Reports what lvgl believes the display dimensions are.
       */
      inline void printDisplayInfo() {
        lv_display_t* disp = lv_display_get_default();
        if (!disp) {
          Serial.println("ERROR: No default LVGL display configured");
          return;
        }
        
        int32_t width = lv_display_get_horizontal_resolution(disp);
        int32_t height = lv_display_get_vertical_resolution(disp);
        
        Serial.println("------LVGL Display Info------");
        Serial.print("Resolution: "); Serial.print(width); Serial.print("x"); Serial.println(height);
      }

      /**
       * @brief Prints widget coordinates.
       * 
       * Reports both the requested position and the actual computed coordinates
       * after lvgl's layout processing.
       * 
       * @param label Text to identify the widget.
       * @param widget The lvgl widget.
       */
      inline void printWidgetCoords(const char* label, lv_obj_t* widget) {
        lv_area_t coords;
        lv_obj_get_coords(widget, &coords);
        
        Serial.print(label); Serial.print(" coords: (");
        Serial.print(coords.x1); Serial.print(","); Serial.print(coords.y1);
        Serial.print(") -> (");
        Serial.print(coords.x2); Serial.print(","); Serial.print(coords.y2);
        Serial.println(")");
      }

      /**
       * @brief Print flush callback area for debugging partial rendering.
       * 
       * Call this from your flush callback to trace which screen regions
       * lvgl is sending for rendering.
       * 
       * @param flushNumber The sequential flush call number.
       * @param area The area being flushed.
       */
      inline void printFlushArea(int flushNumber, const lv_area_t* area) {
        int16_t width = area->x2 - area->x1 + 1;
        int16_t height = area->y2 - area->y1 + 1;
        int32_t pixels = width * height;
        
        Serial.print("Flush #"); Serial.print(flushNumber);
        Serial.print(" area: ("); Serial.print(area->x1); Serial.print(","); Serial.print(area->y1);
        Serial.print(") -> ("); Serial.print(area->x2); Serial.print(","); Serial.print(area->y2);
        Serial.print(") size: "); Serial.print(width); Serial.print("x"); Serial.print(height);
        Serial.print(" = "); Serial.print(pixels); Serial.println(" px");
      }

      /**
       * @brief Print pixel buffer values at specific positions.
       * 
       * Inspects a 16-bit pixel buffer.
       * 
       * @param pixelBuffer A pointer to a RGB565 pixel buffer.
       * @param bufferWidth Width of the buffer in pixels (for row calculations).
       * @param x X position in the buffer.
       * @param y Y position in the buffer (relative to buffer start, not screen).
       * @param label Optional label for the pixel.
       */
      inline void printPixelValue(const uint16_t* pixelBuffer, int16_t bufferWidth, int16_t x, int16_t y, const char* label = nullptr) {
        int32_t index = y * bufferWidth + x;
        uint16_t value = pixelBuffer[index];
        
        if (label) {
          Serial.print(label); Serial.print(" ");
        }
        Serial.print("px["); Serial.print(index); Serial.print("] (");
        Serial.print(x); Serial.print(","); Serial.print(y);
        Serial.print("): 0x"); Serial.println(value, HEX);
      }

      /**
       * @brief Print expected vs actual color comparison.
       * 
       * Compares a pixel value against RGB565 values.
       * 
       * @param value The actual RGB565 pixel value.
       * @param expectedName Name of expected color ("red", "green", "blue", etc.).
       */
      inline void printColorCheck(uint16_t value, const char* expectedName) {
        Serial.print("Color check: 0x"); Serial.print(value, HEX);
        Serial.print(" (expected "); Serial.print(expectedName);
        Serial.print(": ");
        
        // Print expected RGB565 values for common colors
        if (strcmp(expectedName, "red") == 0) {
          Serial.print("0xF800");
          Serial.print(value == 0xF800 ? " ✓" : " ✗");
        } else if (strcmp(expectedName, "green") == 0) {
          Serial.print("0x07E0");
          Serial.print(value == 0x07E0 ? " ✓" : " ✗");
        } else if (strcmp(expectedName, "blue") == 0) {
          Serial.print("0x001F");
          Serial.print(value == 0x001F ? " ✓" : " ✗");
        } else if (strcmp(expectedName, "white") == 0) {
          Serial.print("0xFFFF");
          Serial.print(value == 0xFFFF ? " ✓" : " ✗");
        } else if (strcmp(expectedName, "black") == 0) {
          Serial.print("0x0000");
          Serial.print(value == 0x0000 ? " ✓" : " ✗");
        } else {
          Serial.print("unknown");
        }
        Serial.println(")");
      }

      /**
       * @brief Print an lvgl flush callback diagnostic.
       * 
       * Combines area info, pixel inspection, and color validation.
       * 
       * @param flushNumber Sequential flush number (use a static counter in callback 'i++').
       * @param area The flush area.
       * @param pixelBuffer The pixel data buffer (cast uint8_t* to uint16_t*).
       * @param inspectCorners If true, prints pixel values at buffer corners.
       */
      inline void printFlushDiagnostic(int flushNumber, const lv_area_t* area, const uint16_t* pixelBuffer, bool inspectCorners = false) {
        printFlushArea(flushNumber, area);
        
        if (inspectCorners && pixelBuffer) {
          int16_t width = area->x2 - area->x1 + 1;
          int16_t height = area->y2 - area->y1 + 1;
          
          Serial.println("  Corner pixels:");
          printPixelValue(pixelBuffer, width, 0, 0, "    TL");
          printPixelValue(pixelBuffer, width, width - 1, 0, "    TR");
          printPixelValue(pixelBuffer, width, 0, height - 1, "    BL");
          printPixelValue(pixelBuffer, width, width - 1, height - 1, "    BR");
        }
      }

      // Pseudo-JSON formatting for readable structured logging.
      namespace Json {
        
        // Current indentation level for nested JSON structures.
        inline uint8_t& indentLevel() {
          static uint8_t level = 0;
          return level;
        }

        // Print indentation spaces based on current level.
        inline void printIndent() {
          for (uint8_t i = 0; i < indentLevel(); i++) {
            Serial.print("  ");
          }
        }

        // Start a JSON object with opening brace.
        inline void objectStart() {
          printIndent();
          Serial.println("{");
          indentLevel()++;
        }

        // End a JSON object with closing brace.
        inline void objectEnd() {
          indentLevel()--;
          printIndent();
          Serial.println("}");
        }

        // Start a named nested object.
        inline void objectStart(const char* key) {
          printIndent();
          Serial.print("\""); Serial.print(key); Serial.println("\": {");
          indentLevel()++;
        }

        // Print a string field.
        inline void field(const char* key, const char* value, bool comma = true) {
          printIndent();
          Serial.print("\""); Serial.print(key); Serial.print("\": \""); Serial.print(value); Serial.print("\"");
          if (comma) Serial.println(","); else Serial.println();
        }

        // Print an integer field.
        inline void field(const char* key, int32_t value, bool comma = true) {
          printIndent();
          Serial.print("\""); Serial.print(key); Serial.print("\": "); Serial.print(value);
          if (comma) Serial.println(","); else Serial.println();
        }

        // Print a boolean field.
        inline void fieldBool(const char* key, bool value, bool comma = true) {
          printIndent();
          Serial.print("\""); Serial.print(key); Serial.print("\": "); Serial.print(value ? "true" : "false");
          if (comma) Serial.println(","); else Serial.println();
        }

        // Print a hex value field.
        inline void fieldHex(const char* key, uint16_t value, bool comma = true) {
          printIndent();
          Serial.print("\""); Serial.print(key); Serial.print("\": \"0x"); Serial.print(value, HEX); Serial.print("\"");
          if (comma) Serial.println(","); else Serial.println();
        }

        // Print an action header (source → target with action description).
        inline void action(const char* source, const char* target, const char* actionText) {
          field("source", source);
          field("target", target);
          field("action", actionText);
        }

        // Print status field.
        inline void status(const char* statusText, bool comma = true) {
          field("status", statusText, comma);
        }

        // Print error message and close object.
        inline void error(const char* message) {
          status("Error");
          field("message", message, false);
          objectEnd();
        }

        // Print geometry object with width, height.
        inline void geometry(int16_t width, int16_t height, bool hasBoundary = false) {
          objectStart("geometry");
          field("width", width);
          field("height", height, hasBoundary);
        }

        // Print boundary coordinates.
        inline void boundary(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
          objectStart("boundary");
          field("x1", static_cast<int32_t>(x1));
          field("y1", static_cast<int32_t>(y1));
          field("x2", static_cast<int32_t>(x2));
          field("y2", static_cast<int32_t>(y2), false);
          objectEnd();
        }

        // Print display dimensions.
        inline void displayInfo(int16_t width, int16_t height) {
          objectStart("display");
          field("width", width);
          field("height", height, false);
          objectEnd();
        }

        // Print widget coordinates.
        inline void widgetCoords(lv_obj_t* widget) {
          objectStart("coords");
          field("width", lv_obj_get_width(widget));
          field("height", lv_obj_get_height(widget));
          field("x", lv_obj_get_x(widget));
          field("y", lv_obj_get_y(widget), false);
          objectEnd();
        }

        // Reset indent level (call at start of new log session).
        inline void reset() {
          indentLevel() = 0;
        }
      } // namespace Json
    } // namespace Debug
  } // namespace LVGL 
} // namespace Utils

#endif
