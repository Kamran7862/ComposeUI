#include <Arduino.h>
#include "ui/lvgl/Screen.h"
#include "display/Display.h"
#include "ui/pool/WidgetPool.h"
#include "display/TFT_HX8357D.h"
#include "ui/builder/WidgetBuilder.h"
#include "ui/registry/WidgetRegistry.h"
#include "ui/instances/WidgetInstances.h"
#include "ui/definitions/WidgetDefinitions.h"

/**
 * @file main.cpp
 * @brief System orchestrator for UI initialization and runtime.
 * 
 * Coordinates the complete system lifecycle from hardware initialization through widget
 * rendering. Manages the interaction between display drivers, LVGL screen management,
 * widget registry/pool services, and the builder orchestrator. Provides state machine
 * workflows for systematic setup and debugging capabilities for hardware and rendering
 * validation.
 *
 */

// Define display specific pin numbers for initilization.
#define LCD_CS 10 // LCD Chip select (C/S)
#define LCD_DC 9 // LCD Data/Command (D/C)
#define LCD_RST 8 // LCD Reset

// Helper function to configure/run the screen.
template<typename Driver>
bool setScreen(Screen<Driver>& screen, WidgetRegistry* widgetRegistry, WidgetPool* widgetPool) {
  bool orchestrating = true;
  
  Serial.println("--Screen Configuration Information--");

  while (orchestrating) {
      ScreenState screenState = screen.getState();
      
      switch (screenState) {
          case ScreenState::UNINITIALIZED:
            Serial.println("Setting services...");
            screen.setServices(widgetRegistry, widgetPool);
            break;
          
          case ScreenState::SERVICES_SET:
            Serial.println("Success: Services set.");
            Serial.println();
            screen.setRegistry();
            break;
          
          case ScreenState::ERROR_SERVICES:
            Serial.println("Error: Sevices. Exiting configuration...");
            Serial.println();
            orchestrating = false;
            return false;
            break;  // defensive
              
          case ScreenState::WIDGETS_REGISTERED:
            Serial.println("Success: Widget dimensions registered.");
            Serial.println("Creating widgets in lvgl...");
            Serial.println();
            screen.setWidgets();
            break;

          case ScreenState::ERROR_REGISTRATION:
            Serial.println("Error: Registration. Exiting configuration...");
            Serial.println();
            orchestrating = false;
            return false;
            break;            
          
          case ScreenState::ATTRIBUTES_SET:
            Serial.println("Success: Screen setup complete!");
            Serial.println();
            orchestrating = false;
            return true;
            break;
            
          case ScreenState::ERROR_ATTRIBUTES:
            Serial.println("Error: Attributes. Exiting configuration...");
            Serial.println();
            orchestrating = false;
            return false;
            break;
              
          default:
            Serial.println("Error: Unknown error occured.");
            Serial.println();
            orchestrating = false;
            return false;
            break;
      }
  }
  return false;  // defensive
}

// Helper function to configure/run the builder.
bool setBuilder(WidgetBuilder& builder, WidgetRegistry* widgetRegistry, WidgetPool* widgetPool) {
  bool orchestrating = true;

  Serial.println("--Builder Configuration Information--");
  
  while (orchestrating) {
      BuilderState builderState = builder.getState();
      
      switch (builderState) {
        case BuilderState::UNINITIALIZED:
          Serial.println("Setting services...");
          builder.setServices(widgetRegistry, widgetPool);
          break;

        case BuilderState::SERVICES_SET:
          Serial.println("Success: Services set.");
          Serial.println("Building widgets...");
          builder.setWidgets();
          break;

        case BuilderState::ERROR_SERVICES:
          Serial.println("Error: Sevices. Exiting configuration...");
          Serial.println();
          orchestrating = false;
          return false;
          break;  // defensive
            
        case BuilderState::COMPLETE:
          Serial.println("Success: Added widgets to the pool!");
          Serial.println();
          orchestrating = false;
          return true;
          break;

        case BuilderState::ERROR_BUILDING:
          Serial.println("Error: Building. Exiting configuration...");
          Serial.println();
          orchestrating = false;
          return false;
          break;
            
        default:
          Serial.println("Error: Unknown error occured.");
          Serial.println();
          orchestrating = false;
          return false;
          break;
      }
  }
  return false; // defensive
}

void setup() {

  // 1. Start the serial monitor.
  Serial.begin(9600);

  // 2. Add widgets to the object pool.
  WidgetPool& widgetPool = WidgetPool::getInstance();
  UI::Instances::Widgets::addWidgets(widgetPool);

  // 3. Add widget attributes to the registry.
  WidgetRegistry& widgetRegistry = WidgetRegistry::getInstance();
  UI::Definitions::Widgets::registerWidgets(widgetRegistry);

  /**
   * 4. Initialize the display driver.
   * 
   * Wraps the TFT_HX8357D hardware driver and allocates the pixel buffer for LVGL.
   * The init() call configures SPI communication, sets orientation, and fills the
   * screen with the initial background color.
   * 
   * Debug: display.debugDraw(bool) tests hardware pixel writing independently of LVGL.
   * Draws colored blocks in screen corners to validate orientation and SPI communication.
   * Cannot be used simultaneously with screen.debugDraw() or normal rendering.
   */  
  static Display<TFT_HX8357D> display(LCD_CS, LCD_DC, LCD_RST);
  display.init(); // Initialize the display hardware.
  // display.debugDraw();
  
  /**
   * 5. Initialize the LVGL screen coordinator.
   * 
   * Configures LVGL display system with the hardware driver and manages the complete
   * widget lifecycle. Coordinates between WidgetRegistry (attributes) and WidgetPool
   * (instances) to render widgets to the display.
   * 
   * Debug: screen.debugDraw() tests LVGL rendering pipeline by drawing test blocks
   * through LVGL's event system. Cannot be used with display.debugDraw() simultaneously.
   */  
  static Screen<TFT_HX8357D> screen(display);
  // screen.debugDraw();

  /**
   * 6. Initialize the widget builder.
   * 
   * The WidgetBuilder orchestrates widget configuration through a state
   * machine workflow. It applies attributes from the registry,
   * and prepares them for rendering.
   * 
   * Note: When debugging display or screen, comment out everything from here down.
   * comment out through loop() when testing the display.
   */  
  WidgetBuilder& builder = WidgetBuilder::getInstance();

  // 7. Configure the registry, pool & builder.
  if (!setScreen(screen, &widgetRegistry, &widgetPool)) {
    Serial.println("-----------System Status------------");
    Serial.println("Error: Screen setup failed!");
    Serial.println();
    Serial.println("------LVGL Screen Information------");
    screen.printLVGL();
    return;
  }

  if (!setBuilder(builder, &widgetRegistry, &widgetPool)) {
    Serial.println("-----------System Status------------");
    Serial.println("Error: Builder setup failed!");
    Serial.println();
    Serial.println("------LVGL Screen Information------");
    screen.printLVGL();
    return;
  }

  // 8. Draw widgets to the display.
  screen.drawWidgets();
  Serial.println("-----------System Status------------");
  Serial.println("Configured: Setup complete - display initialized!");
  Serial.println();
  Serial.println("------LVGL Screen Information------");
  screen.printLVGL();
  Serial.println();
  Serial.println("------LVGL Object Rendering Information-------");
  screen.printWidgets();
}

/**
 * @brief Main event loop for LVGL processing.
 * 
 * Handles LVGL timer events, screen refreshes, input processing, and animation updates.
 * Must be called repeatedly to keep the UI responsive. The 5ms delay prevents excessive
 * CPU usage while maintaining smooth rendering.
 * 
 * Note: Comment out when debugging display or screen to prevent interference with
 * validation functions (display.debugDraw() or screen.debugDraw()).
 */
void loop() {
    lv_timer_handler(); // Process pending lvgl tasks (refresh, inputs, timers).
    delay(5); // Delay refreshes to prevent excessive cpu usage.
}