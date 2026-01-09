#include <lvgl.h>
#include "utils/LVGL.h"
#include "utils/Math.h"
#include "ui/widgets/Graticules.h"
#include "render/GraticulesRenderer.h"

// Helper function to initialize line descriptor based on line type.
static void initializeLineDescriptor(lv_draw_line_dsc_t& lineDescriptor, Graticules* graticules, bool isOrigin) {
  lv_draw_line_dsc_init(&lineDescriptor); // Initialize with default values; best practice, see definition of the type.

  if (isOrigin) {
      lineDescriptor.color = Utils::Math::convert565(graticules->getColor(Graticules::Attribute::LineType::ORIGIN));
      lineDescriptor.width = graticules->getThickness(Graticules::Attribute::LineType::ORIGIN);
  } else {
      lineDescriptor.color = Utils::Math::convert565(graticules->getColor(Graticules::Attribute::LineType::GRIDLINE));
      lineDescriptor.width = graticules->getThickness(Graticules::Attribute::LineType::GRIDLINE);
  }
}

// Helper function to set line segment coordinates.
static void setLineSegmentCoordinates(lv_draw_line_dsc_t& lineDescriptor, const GraticulesLineSegment& lineSegment) {
  lineDescriptor.p1.x = lineSegment.x1;
  lineDescriptor.p1.y = lineSegment.y1;
  lineDescriptor.p2.x = lineSegment.x2;
  lineDescriptor.p2.y = lineSegment.y2;
}

// Helper function to draw the graticules object.
static void drawGraticule(Graticules::Attribute::AxisType axisType, Graticules* graticules, lv_layer_t* layer) {
  const uint8_t divisions = graticules->getDivisions(axisType);
  const int8_t axisIndex = graticules->getAxisIndex(axisType);
  const char* axisName = (axisType == Graticules::Attribute::AxisType::TIME) ? "TIME SEGMENT" : "VOLTAGE SEGMENT";
  static uint8_t printCount = 0;
  lv_draw_line_dsc_t lineDescriptor;

  // Loop over the graticules and draw it.
  for (uint8_t i = 0; i < divisions; i++) {
      GraticulesLineSegment lineSegment = graticules->getLineSegment(i, axisType);
      bool isOrigin = (i == axisIndex);

      // Prevent multiple debugs from printing as the layers of lvgl run.
      if (printCount <= 1) {
          Utils::LVGL::Debug::printLine(axisName, lineSegment.x1, lineSegment.y1, lineSegment.x2, lineSegment.y2);
      }
    
      // Initialize the line descriptor; origin lines get different styling.
      initializeLineDescriptor(lineDescriptor, graticules, isOrigin);

      // Set line segment.
      setLineSegmentCoordinates(lineDescriptor, lineSegment);

      // Draw the line.
      lv_draw_line(layer, &lineDescriptor);
  }
  printCount++;
}

/**
 * @brief LVGL draw event callback for graticules.
 * 
 * This function is the main entry point for graticule rendering within the
 * LVGL event system. When the graticules widget requires redrawing (due to screen
 * updates, overlays, or refresh cycles), LVGL triggers this callback.
 * 
 * @param event LVGL event structure containing target widget, layer context, and user data.
 * 
 */
void graticule_draw_event_cb(lv_event_t* event) {
  lv_layer_t* layer = lv_event_get_layer(event); // Get the LVGL drawing layer context for rendering.
  lv_obj_t* widget = static_cast<lv_obj_t*>(lv_event_get_target(event)); // Extract the lvgl widget object that triggered this draw event.
  Graticules* graticules = static_cast<Graticules*>(lv_obj_get_user_data(widget)); // Retrieve the Graticules instance stored in the widget's user data.
  
  // Prevent multiple debugs from printing as the layers of lvgl run.
  static bool hasPrinted = false;
  if (!hasPrinted) {
    hasPrinted = true;
    Utils::LVGL::Debug::printClippingArea(widget);
  }

  drawGraticule(Graticules::Attribute::AxisType::TIME, graticules, layer); // Vertical lines.
  drawGraticule(Graticules::Attribute::AxisType::VOLTAGE, graticules, layer); // Horizontal lines.
}