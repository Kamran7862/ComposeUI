#include <stdint.h> // primatives
#include "utils/Math.h"
#include "ui/widgets/Graticules.h"
#include "utils/WidgetAttributes.h"

/**
 * @brief Helper function; calculates the pixel step size for time and voltage divisions.
 *
 * Creates graticule lines.
 * 
 */
void Graticules::setStepSize() {
  int16_t timeLength = (this->privateBoundary.x2 - this->privateBoundary.x1) + 1; // Boundary is inclusive: pixel span is (x2 - x1 + 1).
  int16_t voltageLength = (this->privateBoundary.y2 - this->privateBoundary.y1) + 1;

  this->timeStepSize = timeLength / this->timeDivisions; // For N divisions, number of lines is N - 1.
  this->voltageStepSize = voltageLength / this->voltageDivisions;
}

/**
 * @brief Helper function; calculates and sets the coordinates for each graticule line.
 *
 * Determines the position of time and voltage graticules based on
 * boundaries and calculated step sizes.
 */
void Graticules::setLineSegments() {
  int8_t i;

  // Time graticules (vertical lines)
  for (i = 0; i < this->timeDivisions; i++) {
    int16_t x = this->privateBoundary.x1 + (i * this->timeStepSize);

    this->Time[i].x1 = x; 
    this->Time[i].y1 = this->privateBoundary.y1; 
    this->Time[i].x2 = x; 
    this->Time[i].y2 = this->privateBoundary.y2;
  }

  // Voltage graticules (horizontal lines)
  for (i = 0; i < this->voltageDivisions; i++) {
  int16_t y = this->privateBoundary.y1 + (i * this->voltageStepSize); 
  
  this->Voltage[i].x1 = this->privateBoundary.x1; 
  this->Voltage[i].y1 = y; 
  this->Voltage[i].x2 = this->privateBoundary.x2; 
  this->Voltage[i].y2 = y;
  }
}

/**
 * @brief Configures graticule member variables.
 * 
 * Sets up boundary coordinates, colors, line thickness, division counts,
 * and origin positioning. Automatically calculates step sizes and line
 * segment coordinates based on provided parameters.
 */
void Graticules::configure (
                            const Utils::Widget::Boundary* const boundary,
                            Utils::Widget::Color originColor,
                            int8_t originThickness, 
                            Utils::Widget::Color gridlineColor, 
                            int8_t gridlineThickness,
                            int8_t timeDivisions,
                            int8_t voltageDivisions,
                            int8_t xOriginIndex,
                            int8_t yOriginIndex) {

  this->privateBoundary.x1 = boundary->x1;
  this->privateBoundary.x2 = boundary->x2;
  this->privateBoundary.y1 = boundary->y1;
  this->privateBoundary.y2 = boundary->y2;

  this->originColor = originColor;
  this->originThickness = originThickness;
  this->gridlineColor = gridlineColor;
  this->gridlineThickness = gridlineThickness;
  this->timeDivisions = timeDivisions;
  this->voltageDivisions = voltageDivisions;
  this->xOriginIndex = xOriginIndex;
  this->yOriginIndex = yOriginIndex;

  setStepSize();
  setLineSegments();
}

/**
 * @brief Returns the color of the specified graticule type.
 *
 * @return RGB 565 pixel value as an unsigned 16 bit integer.
 */
uint16_t Graticules::getColor(Graticules::Attribute::LineType type) {
  
  switch(type) {
    case Graticules::Attribute::LineType::ORIGIN: 
      return Utils::Math::enum565(this->originColor);
      break;

    case Graticules::Attribute::LineType::GRIDLINE:
      return Utils::Math::enum565(this->gridlineColor);
      break;

    default:
      return 0;
      break;
  }
}

/**
 * @brief Returns the line thickness of the specified graticule type.
 *
 * @return Thickness size as an 8 bit integer.
 */
int8_t Graticules::getThickness(Graticules::Attribute::LineType type) {
  
  switch(type) {
    case Graticules::Attribute::LineType::ORIGIN: 
      return this->originThickness;
      break;

    case Graticules::Attribute::LineType::GRIDLINE: 
      return this->gridlineThickness;
      break;

    default: 
      return 0;
      break;
  }
}

/**
 * @brief Returns the pixel step size for the specified axis type.
 *
 * @return Step size in pixels as a 16 bit integer.
 */
int16_t Graticules::getStepSize(Graticules::Attribute::AxisType type) {
  
  switch (type) {
    case Graticules::Attribute::AxisType::TIME: 
      return this->timeStepSize;
      break;

    case Graticules::Attribute::AxisType::VOLTAGE: 
      return this->voltageStepSize;
      break;

    default: 
      return 0;
      break;
  }
}

/**
 * @brief Returns the number of time/voltage divisions.
 * 
 * This method returns the number of divisions (graticules) for the given axis type 
 * (e.g., TIME or VOLTAGE).
 * 
 * @return Number of divisions as an 8 bit integer.
 */
int8_t Graticules::getDivisions(Graticules::Attribute::AxisType type) {
  
  switch(type) {
    case Graticules::Attribute::AxisType::TIME: 
      return this->timeDivisions;
      break;

    case Graticules::Attribute::AxisType::VOLTAGE: 
      return this->voltageDivisions;
      break;

    default: 
      return 0;
      break;
  }
}

/**
 * @brief Returns the index of the axis centerline for the specified axis type.
 *
 * @return Axis index an 8 bit integer.
 */
int8_t Graticules::getAxisIndex(Graticules::Attribute::AxisType type) {
  
  switch (type) {
    case Graticules::Attribute::AxisType::TIME: 
      return this->xOriginIndex;
      break;

    case Graticules::Attribute::AxisType::VOLTAGE: 
      return this->yOriginIndex;
      break;

    default: 
      return 0;
      break;
  }
}

/**
 * @brief Returns the graticule coordinates.
 *
 * If the graticuleIndex or AxisType are invalid, returns GraticulesLineSegment{0, 0, 0, 0}.
 * 
 */
GraticulesLineSegment Graticules::getLineSegment(int8_t segmentIndex, Graticules::Attribute::AxisType type) {
  
  if (!isValidIndex(segmentIndex, type)) {
      return GraticulesLineSegment{0, 0, 0, 0};
  }

  GraticulesLineSegment segment;
  switch (type) {

    case Graticules::Attribute::AxisType::TIME: 
      segment.x1 = Time[segmentIndex].x1;
      segment.y1 = Time[segmentIndex].y1;
      segment.x2 = Time[segmentIndex].x2;
      segment.y2 = Time[segmentIndex].y2;
      return segment;
      break;

    case Graticules::Attribute::AxisType::VOLTAGE: 
      segment.x1 = Voltage[segmentIndex].x1;
      segment.y1 = Voltage[segmentIndex].y1;
      segment.x2 = Voltage[segmentIndex].x2;
      segment.y2 = Voltage[segmentIndex].y2;
      return segment;
      break;

    default: 
      return GraticulesLineSegment{0, 0, 0, 0};
      break;
  }
}

/**
 * @brief Returns the graticules boundary.
 */
GraticulesBoundary Graticules::getBoundary() {
  GraticulesBoundary boundary;
  boundary.x1 = this->privateBoundary.x1;
  boundary.y1 = this->privateBoundary.y1;
  boundary.x2 = this->privateBoundary.x2;
  boundary.y2 = this->privateBoundary.y2;
  return boundary;
}

/**
 * @brief Returns a single coordinate value from the graticule.
 * 
 * @param graticuleIndex Index of the graticule.
 * @param type The axis type.
 * @param coordinate Which coordinate to return (x1, y1, x2, y2).
 * @return Coordinate value as an 16 bit integer; returns 0 if not exits.
 */
int16_t Graticules::getCoordinate(int8_t segmentIndex, Graticules::Attribute::AxisType type, Graticules::Attribute::Coordinate coordinate) {
  GraticulesLineSegment segment = getLineSegment(segmentIndex, type);

  switch(coordinate) {
    case Graticules::Attribute::Coordinate::X1: 
      return segment.x1;
      break;

    case Graticules::Attribute::Coordinate::Y1: 
      return segment.y1;
      break;

    case Graticules::Attribute::Coordinate::X2: 
      return segment.x2;
      break;

    case Graticules::Attribute::Coordinate::Y2: 
      return segment.y2;
      break;

    default: 
      return 0;
      break;
  }
}

/**
 * @brief Validates that a line segment exists at the index of an axis type.
 * @param graticuleIndex Index of the line segment.
 * @param type The axis type.
 * @return True if valid, false otherwise.
 */
bool Graticules::isValidIndex(int8_t segmentIndex, Graticules::Attribute::AxisType type) {
  uint8_t graticuleCount = (type == Graticules::Attribute::AxisType::TIME) ? this->timeDivisions : this->voltageDivisions;
  return (segmentIndex >= 0 && segmentIndex < graticuleCount);
}