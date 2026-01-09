#ifndef DISPLAY_H
#define DISPLAY_H

/**
 * @file Display.h
 * @brief Hardware abstraction layer for lvgl displays.
 * 
 * The Display template wraps hardware-specific driver structs to provide a compile-time
 * interface for lvgl display management. It handles buffer allocation, driver
 * initialization, and pixel transfer operations.
 * 
 * To add a new display driver:
 * 1. Create a driver struct (e.g., TFT_NewDisplay.h) with required static constants:
 *    - id: Controller ID
 *    - horizontalRes: Pixel width
 *    - verticalRes: Pixel height
 *    - orientation: Screen rotation (0-3)
 *    - fillScreenColor: Initial background color
 * 2. Implement required methods:
 *    - Constructor accepting hardware pin configuration
 *    - init(): Initialize hardware and set orientation
 *    - flushCallback(): Transfer pixels from LVGL buffer to hardware
 *    - debugDraw(): Optional validation function
 * 3. Instantiate with Display<NewDriver> and pass constructor arguments
 */

#include <lvgl.h>

template<typename Driver>
class Display {
private:
  Driver driver; // The driver struct being wrapped.

  static constexpr uint16_t id = Driver::id; // Controller ID.
  static constexpr uint16_t horizontalRes = Driver::horizontalRes; // The displays pixel width.
  static constexpr uint16_t verticalRes = Driver::verticalRes; // The displays pixel height.
  static constexpr uint8_t orientation = Driver::orientation; // The number representation of the displays orientation.
  static constexpr uint16_t fillScreenColor = Driver::fillScreenColor; // The initilization color of the display.
  static constexpr uint16_t bufferLength = ((uint32_t(horizontalRes) * uint32_t(verticalRes)) / 10); // Buffer size for partial rendering (1/10th of the screen).

  uint16_t buffer[bufferLength];  // A non-static buffer (per-instance, fixed size).

public:

  /**
   * @brief Construct display with driver-specific arguments.
   * 
   * Uses template forwarding because different display drivers require
   * different constructor arguments (e.g., pin configurations, SPI settings).
   */
  template<typename... Args>
  Display(Args&&... args) 
    : driver(std::forward<Args>(args)...) {};

  // Get the display controller ID from driver configuration.
  static constexpr uint16_t getID() { return id; }

  // Get the display horizontal resolution from driver configuration.
  static constexpr uint16_t getHorizontalRes() { return horizontalRes; }

  // Get the display vertical resolution from driver configuration.
  static constexpr uint16_t getVerticalRes() { return verticalRes; }

  // Get the buffer length in pixels (1/10th of total screen area).
  static constexpr uint16_t getBufferLength() { return bufferLength; }

  // Get the buffer size in bytes for LVGL configuration.
  size_t getBufferSizeBytes() const { 
    return sizeof(this->buffer); 
  }

  // Get pointer to the pixel buffer for LVGL rendering.
  void* getBuffer() {
    return static_cast<void*>(this->buffer);
  }

  // Calculate total screen area in pixels.
  constexpr uint32_t getArea() const {
    return (uint32_t(this->horizontalRes) * uint32_t(this->verticalRes));
  }

  /**
   * @brief Initialize the display hardware and configure default state.
   * 
   * Starts a display driver, applies the configured orientation,
   * and fills the screen with the initial background color.
   */
  void init() {
    driver.init();
  }

  /**
   * @brief LVGL flush callback that transfers rendered pixels to display hardware.
   * 
   * Called by lvgl when a screen region needs to be updated. Receives pixel data from
   * lvgl's rendering buffer (px_map) and writes it to the display controller via SPI.
   * 
   * @param display The lvgl display object being flushed.
   * @param area The rectangular screen region to update (coordinates are inclusive).
   * @param px_map A pointer to pixel data in RGB565 format (passed as uint8_t* for API generality).
   * @param flushCompleteCallback Callback to signal lvgl that the flush operation is complete.
   */
  void flushCallback(lv_display_t* display, const lv_area_t* area,  uint8_t* px_map, void (*flushCompleteCallback)(lv_display_t*)) {
    driver.flushCallback(display, area, px_map, flushCompleteCallback);
  }

  /**
   * @brief Creates simple colored rectangles at known coordinates to validate the display's alignment.
   * 
   * Will produce a red pixel at the origin, green bottom left, blue bottom right.
   *
   * @param isPortrait If true, swaps width/height for portrait orientation testing.
   */
  void debugDraw(bool isPortrait = false) {
    driver.debugDraw(isPortrait);
  }  
};

#endif