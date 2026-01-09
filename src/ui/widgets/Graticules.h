#ifndef GRATICULES_H
#define GRATICULES_H

#include <stdint.h>
#include "utils/Constants.h"
#include "utils/WidgetAttributes.h"
#include "ui/widgets/Widget.h"

/**
 * @struct GraticulesLineSegment
 * @brief Public value-type representation of a grid line segment.
 *
 * This struct is the *external* geometry format returned to callers and
 * consumed by lvgl.
 *
 * Internally, Graticules maintains its own private LineSegment type that
 * enforces class invariants. GraticulesLineSegment exists so callers can receive a
 * safe copy of the geometry without exposing internal state.
 */
struct GraticulesLineSegment {
  int16_t x1 = 0; // Line coordinate x1.
  int16_t y1 = 0; // Line coordinate y1.
  int16_t x2 = 0; // Line coordinate x2.
  int16_t y2 = 0; // Line coordinate y2.
};

/**
 * @struct GraticulesBoundry
 * @brief Public value-type representation of the widget's bounding region.
 *
 * This is the external POD form returned by Graticules. It allows callers to
 * inspect the computed bounds without exposing or modifying the class's
 * internal invariant-controlled Boundry type.
 */
struct GraticulesBoundary {
  int16_t x1 = 0; // Lower bounds.
  int16_t y1 = 0; // Lower bounds.
  int16_t x2 = 0; // Upper bounds.
  int16_t y2 = 0; // Upper bounds.
};

/**
 * @class Graticules
 * @brief Computes and exposes an oscilloscope grid (graticules).
 *
 * This class calculates line segments for the TIME (vertical) and
 * VOLTAGE (horizontal) axes based on a WidgetAttributes::Boundry and division counts.
 * It is a data/geometry provider only; it does not render.
 */
class Graticules : public Widget{
private:

  /**
   * @struct PrivateLineSegment
   * @brief Internal invariant-controlled line segment representation.
   */
  struct PrivateLineSegment {
    int16_t x1 = 0; // Line coordinate x1.
    int16_t y1 = 0; // Line coordinate y1.
    int16_t x2 = 0; // Line coordinate x2.
    int16_t y2 = 0; // Line coordinate y2.
  };

  /**
   * @struct PrivateBoundry
   * @brief Internal invariant-controlled bounding region.
   */
  struct PrivateBoundary {
    int16_t x1 = 0; // Lower bounds.
    int16_t y1 = 0; // Lower bounds.
    int16_t x2 = 0; // Upper bounds.
    int16_t y2 = 0; // Upper bounds.
  };

  PrivateBoundary privateBoundary;

  Utils::Widget::Color originColor = Utils::Widget::Color::DEFAULT; // A RGB565 color for the x/y axis lines.
  int8_t originThickness = 0; // The thickness of the x/y axis lines (px).

  Utils::Widget::Color gridlineColor = Utils::Widget::Color::DEFAULT; // A RGB565 color for grid lines.
  int8_t gridlineThickness = 0; // The thickness of the grid lines.

  int8_t timeDivisions = 0; // Total TIME divisions (spaces). Usually N lines = timeDivisions - 1.
  int8_t voltageDivisions = 0; // Total VOLTAGE divisions (spaces). Usually N lines = voltageDivisions - 1.

  int8_t xOriginIndex = 0; // Index of the vertical center line (TIME axis).
  int8_t yOriginIndex = 0; // Index of the horizontal center line (VOLTAGE axis).

  int16_t timeStepSize; // Horizontal step, in pixels, between vertical (TIME) lines.
  int16_t voltageStepSize; // Vertical step, in pixels, between horizontal (VOLTAGE) lines.

  PrivateLineSegment Time[Utils::ArrayConstants::MAX_GRATICULES]; // Computed vertical line segments.
  PrivateLineSegment Voltage[Utils::ArrayConstants::MAX_GRATICULES]; // Computed horizontal line segments.

  void setStepSize();
  void setLineSegments();

public:

  Graticules() = default;
  ~Graticules() = default;

 /**
  * @struct Attribute
  * @brief Public symbolic identifiers for querying graticule geometry.
  */
  struct Attribute {
    enum class AxisType {TIME, VOLTAGE};
    enum class LineType {ORIGIN, GRIDLINE};
    enum class Coordinate {X1, X2, Y1, Y2};
  };

  void configure(
                  const Utils::Widget::Boundary* const boundary, 
                  Utils::Widget::Color originColor,
                  int8_t originThickness, 
                  Utils::Widget::Color gridlineColor, 
                  int8_t gridlineThickness,
                  int8_t timeDivisions,
                  int8_t voltageDivisions,
                  int8_t xOriginIndex,
                  int8_t yOriginIndex
                );
  
  uint16_t getColor(Graticules::Attribute::LineType type);
  int8_t getThickness(Graticules::Attribute::LineType type);
  int16_t getStepSize(Graticules::Attribute::AxisType type);
  int8_t getDivisions(Graticules::Attribute::AxisType type);
  int8_t getAxisIndex(Graticules::Attribute::AxisType type);

  GraticulesLineSegment getLineSegment(int8_t segmentIndex, Graticules::Attribute::AxisType type);
  GraticulesBoundary getBoundary();
  int16_t getCoordinate(int8_t segmentIndex, Graticules::Attribute::AxisType type, Graticules::Attribute::Coordinate coordinate);
  bool isValidIndex(int8_t segmentIndex, Graticules::Attribute::AxisType type);
};

#endif