#ifndef GRATICULESRENDERER_H
#define GRATICULESRENDERER_H

/**
 * @file GraticulesRenderer.h
 * @brief Rendering utilities for oscilloscope graticules.
 * 
 * Provides functions for drawing grid lines and handling LVGL draw events
 * for oscilloscope-style graticules.
 */

#include <lvgl.h>

void graticule_draw_event_cb(lv_event_t* event);

#endif