#ifndef TFT_HX8357D_H
#define TFT_HX8357D_H

/**
 * @file TFT_HX8357D.h
 * @brief Hardware driver configuration for the Adafruit HX8357D TFT display controller.
 * 
 * This driver wraps the Adafruit_HX8357 library and provides configuration for display
 * orientation and rendering. The rotation parameter controls the screen orientation:
 * - 0 = Portrait
 * - 1 = Landscape
 * - 2 = Portrait (flipped)
 * - 3 = Landscape (flipped)
 * 
 * IMPORTANT: When changing between portrait (0/2) and landscape (1/3) modes, you must
 * swap the horizontalRes and verticalRes values. For example:
 * - Portrait:  horizontalRes=320, verticalRes=480
 * - Landscape: horizontalRes=480, verticalRes=320
 * 
 * The display's physical dimensions remain constant, but the logical coordinate system
 * rotates.
 */

#include <lvgl.h>
#include <stdint.h> // primatives
#include <Adafruit_HX8357.h>

struct TFT_HX8357D {

  static constexpr uint16_t id = 0x8357; // The controller ID for the HX8357D.
  static constexpr uint16_t horizontalRes = 480; // The HX8357D's pixel width.
  static constexpr uint16_t verticalRes = 320; // The HX8357D's pixel height.
  static constexpr uint8_t orientation = 1; // Portrait:0, Landscape:1, Portrait(flipped):2, Landscape(flipped):3. Default: 0.
  static constexpr uint16_t fillScreenColor = 0x0000; // Color: Black, Format: RGB565.

  Adafruit_HX8357 driver;

  TFT_HX8357D(uint8_t chipSelectPin, uint8_t commandDataPin, uint8_t resetPin)
    : driver(chipSelectPin, commandDataPin, resetPin) {}

  /**
   * @brief Initialize the display hardware and configure default state.
   * 
   * Starts the Adafruit HX8357 driver, applies the configured orientation,
   * and fills the screen with the initial background color.
   */
  void init() {
    driver.begin();
    driver.setRotation(this->orientation);
    driver.fillScreen(this->fillScreenColor);
  }

  /**
   * @brief LVGL flush callback that transfers rendered pixels to display hardware.
   * 
   * Called by lvgl when a screen region needs to be updated. Receives pixel data from
   * lvgl's rendering buffer (px_map) and writes it to the display controller via SPI.
   * 
   * @param display The lvgl display object being flushed.
   * @param area The rectangular screen region to update (coordinates are inclusive).
   * @param px_map Pointer to pixel data in RGB565 format (passed as uint8_t* for API generality).
   * @param flushCompleteCallback Callback to signal lvgl that the flush operation is complete.
   */
  void flushCallback(lv_display_t* display, const lv_area_t* area,  uint8_t* px_map, void (*flushCompleteCallback)(lv_display_t*)) {
    uint16_t* pixelBuffer = reinterpret_cast<uint16_t*>(px_map); // Reads the 8 bit px_map [0x00][0xF8] as one 16-bit value.

    const int32_t width = area->x2 - area->x1 + 1; // Calculate the width and height of the screen area to be updated (+1 for inclusive coordinates).
    const int32_t height = area->y2 - area->y1 + 1;
    const int32_t displayArea = width * height; // The area that the display will write too.

    this->driver.startWrite(); // Perform chip-select and start an SPI transaction.
    this->driver.setAddrWindow(area->x1, area->y1, width, height); // The rectangle that will be written to graphics RAM.
    this->driver.writePixels(pixelBuffer, displayArea); // Write pixel data from the buffer to the display at the correct location.
    this->driver.endWrite(); // Perform a chip de-select.
    
    flushCompleteCallback(display); // After all pixels are written, call the registered callback to signal completion
  }

  /**
   * @brief Creates simple colored rectangles at known coordinates to validate the display's alignment.
   * 
   * Will produce a red pixel at the origin, green bottom left, blue bottom right.
   *
   * @param isPortrait If true, swaps width/height for portrait orientation testing.
   */
  void debugDraw(bool isPortrait) {
    struct Area { uint16_t x1, y1, x2, y2; }; // Area struct mimics lvgl's area struct.
    
    const uint16_t screenWidth = isPortrait ? verticalRes : horizontalRes;
    const uint16_t screenHeight = isPortrait ? horizontalRes : verticalRes;
    
    // Red pixel block at top-left (0, 0).
    {
      Area area = {0, 0, 9, 9}; // 10x10 block (0-9 inclusive = 10 pixels).
      uint16_t width = area.x2 - area.x1 + 1; // Calculate the width and height of the display area to be updated (+1 for inclusive coordinates).
      uint16_t height = area.y2 - area.y1 + 1;
      
      uint16_t pixels[100]; // 10x10 = 100 pixels.
      for (int i = 0; i < 100; i++) pixels[i] = 0xF800; // Red
      
      this->driver.startWrite();
      this->driver.setAddrWindow(area.x1, area.y1, width, height);
      this->driver.writePixels(pixels, width * height);
      this->driver.endWrite();
    }

    // Green pixel block at bottom-left.
    {
      Area area = {0, uint16_t(screenHeight - 9), 9, uint16_t(screenHeight - 1)};
      uint16_t width = area.x2 - area.x1 + 1;
      uint16_t height = area.y2 - area.y1 + 1;
      
      uint16_t pixels[100]; // 10x10 = 100 pixels
      for (int i = 0; i < 100; i++) pixels[i] = 0x07E0; // Green
      
      this->driver.startWrite();
      this->driver.setAddrWindow(area.x1, area.y1, width, height);
      this->driver.writePixels(pixels, width * height);
      this->driver.endWrite();
    }

    // Blue pixel block at bottom-right.
    {
      Area area = {uint16_t(screenWidth - 9), uint16_t(screenHeight - 9), uint16_t(screenWidth - 1), uint16_t(screenHeight - 1)};
      uint16_t width = area.x2 - area.x1 + 1;
      uint16_t height = area.y2 - area.y1 + 1;
      
      uint16_t pixels[100]; // 10x10 = 100 pixels
      for (int i = 0; i < 100; i++) pixels[i] = 0x001F; // Blue
      
      this->driver.startWrite();
      this->driver.setAddrWindow(area.x1, area.y1, width, height);
      this->driver.writePixels(pixels, width * height);
      this->driver.endWrite();
    }
  }
};

#endif