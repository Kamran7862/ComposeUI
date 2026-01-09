#ifndef FLUSHCALLBACK_H
#define FLUSHCALLBACK_H

#include <lvgl.h>
#include "display/Display.h"

/**
 * @brief LVGL flush callback to transfer rendered pixel data to the physical display.
 *
*  When LVGL completes rendering a screen region, it calls this
 * callback to transfer the pixel buffer to the display hardware.
 *
 * The function retrieves the display instance from lvgl's user data, then delegates the
 * pixel transfer operation to the display's hardware-specific flushCallback method.
 * The display driver handles the actual SPI/parallel communication with the display.
 *
 * @tparam Driver The display driver type (HX8357D, ILI9341, etc.).
 * @param display Pointer to the lvgl display object containing configuration and user data.
 * @param area Pointer to the rectangular area struct defining the screen region to update.
 * @param px_map Pixel buffer containing RGB565 color data for the specified area.
 */
template<typename Driver>
void my_flush_cb(lv_display_t* display, const lv_area_t* area, uint8_t* px_map) {
  auto* myDisplay = static_cast<Display<Driver>*>(lv_display_get_user_data(display)); // Retrieve the screen instance from lvgl's user data.
  myDisplay->flushCallback(display, area, px_map, lv_display_flush_ready); // Delegate the pixel transfer to the display's flushCallback method.
}

#endif