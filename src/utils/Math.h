#ifndef MATH_H
#define MATH_H

/**
 * @file Math.h
 * @brief Mathematical utility functions for display calculations.
 * 
 * This file provides math operations for the UI system;
 * includes color space conversions, geometric scaling, and coordinate transformations.
 * 
 */

#include <math.h>
#include <lvgl.h>
#include "utils/WidgetAttributes.h"

namespace Utils {
    namespace Math {

        /**
         * @brief Converts a 16-bit RGB565 color value into a lv_color_t structure.
         * 
         * This function takes a 16-bit RGB565 color value and expands it into an lv_color_t structure,
         * which represents a color in lvgl's internal format.
         * 
         * @param color A 16-bit RGB565 color value (5 bits red, 6 bits green, 5 bits blue).
         * @return lv_color_t The expanded color in LVGL's internal format.
         */
        inline lv_color_t convert565(uint16_t color) {
            lv_color_t c;

            uint8_t red = (color >> 11) & 0x1F; // Extract and compress the 5-bit red component from the RGB565 value.
            uint8_t green = (color >> 5) & 0x3F;
            uint8_t blue = color & 0x1F;

            // where 8_bit = (compressed_565 * 255 (8 bit)) / x (565 max size)
            // where 8_bit = compressed_565 * 2^x / 2^x
            c.red = (red << 3) | (red >> 2); // Scale the 5-bit red value to 8 bits (0-255) and assign it to the lv_color_t structure.
            c.green = (green << 2) | (green >> 4);
            c.blue = (blue << 3) | (blue >> 2);

            return c;
        }

        /**
         * @brief Converts a Utils::Widget::Color to a 16-bit RGB565 value.
         * 
         * The RGB565 format is a compact representation of a color using 16 bits:
         * - 5 bits for red (0-31)
         * - 6 bits for green (0-63)
         * - 5 bits for blue (0-31)
         * 
         * This function maps a Utils::Widget::Color to this format.
         * 
         * @param color The color to convert, represented as a Utils::Widget::Color.
         * @return uint16_t The corresponding 16-bit RGB565 value.
         */
        inline uint16_t enum565(Utils::Widget::Color color) {
            return static_cast<uint16_t>(color);
        }

        /**
         * @brief Create a Boundary.
         *
        * Constructs a Boundary spanning from (0,0) to (width - 1, height - 1).
         *
         * @param width  The rectangle width in whatever coordinate space the caller uses.
         * @param height The rectangle height in whatever coordinate space the caller uses.
         */
        inline Utils::Widget::Boundary convertArea(int16_t width, int16_t height) {
            Utils::Widget::Boundary boundary;
            boundary.x1 = 0;
            boundary.y1 = 0;
            boundary.x2 = width - 1;
            boundary.y2 = height - 1;

            return boundary;
        }

        /**
         * @brief Linearly scales a dimension by a percentage.
         *
         * This interprets the percentage as a direct scaling factor for width or height.
         * For example, 60% produces a dimension that is 0.6 × the original.
         *
         * @param percent  The percentage (0–100) to scale by.
         * @param dimension The original dimension to be scaled.
         */
        inline int16_t dimensionScaling(int16_t percent, int16_t dimension) {
            float scale = static_cast<float>(percent) / 100.0f; 
            return scale * dimension;
        }

        /**
         * @brief Scales a dimension such that the resulting area matches the percentage.
         *
         * This interprets the percentage as a desired area ratio. The dimension is scaled
         * by sqrt(percent/100), makes sure that width × height becomes the specified
         * fraction of the original area. For example, 60% area yields a dimension
         * scaled by sqrt(0.6) ≈ 0.7746.
         *
         * @param percent  The target area percentage (0–100 or higher).
         * @param dimension The original dimension to be scaled.
         */
        inline int16_t areaScaling(int16_t percent, int16_t dimension) {
            float scale = static_cast<float>(percent) / 100.0f; 
            float scalingFactor = std::sqrt(scale); 
            return scalingFactor * dimension;
        }
    }
}

#endif