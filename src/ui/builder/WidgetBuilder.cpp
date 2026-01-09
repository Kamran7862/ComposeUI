#include "ui/widgets/Widget.h"
#include "ui/pool/WidgetPool.h"
#include "ui/widgets/Graticules.h"
#include "utils/WidgetAttributes.h"
#include "ui/builder/WidgetBuilder.h"
#include "ui/registry/WidgetRegistry.h"

/**
 * @brief Get the singleton instance of WidgetBuilder.
 * 
 * The instance is created on first access and persists for the application lifetime.
 * 
 * @return Reference to the singleton WidgetBuilder instance.
 */
WidgetBuilder& WidgetBuilder::getInstance() {
  static WidgetBuilder instance;
  return instance;
}

/**
 * @brief Set the registry and pool services required for widget building.
 * 
 * Configures the builder with references to the WidgetRegistry (attribute definitions)
 * and WidgetPool (widget instances). Both services must be valid for successful
 * widget building operations.
 * 
 * @param widgetRegistry Pointer to the WidgetRegistry containing attribute definitions.
 * @param widgetPool Pointer to the WidgetPool containing widget instances.
 */
void WidgetBuilder::setServices(WidgetRegistry* widgetRegistry, WidgetPool* widgetPool) {
  this->widgetRegistry = widgetRegistry;
  this->widgetPool = widgetPool;

  // Validate both services are available before proceeding
  if (this->widgetRegistry && this->widgetPool) {
    this->state = BuilderState::SERVICES_SET;
  } else {
    this->state = BuilderState::ERROR_SERVICES;
  }
}

/**
 * @brief Execute the widget building workflow using state machine logic.
 * 
 * Orchestrates the widget configuration process through a state machine.
 */
void WidgetBuilder::setWidgets() {
  bool building = true;

  while (building) {
    switch (this->state) {

      // Services set.
      case BuilderState::SERVICES_SET:
        this->state = BuilderState::BUILDING;
        break;

      // Build attributes, add widget to pool.
      case BuilderState::BUILDING:
        setAttribute();
        break;

      // Build complete.
      case BuilderState::COMPLETE:
        building = false;
        break;

      // Building failed.
      case BuilderState::ERROR_BUILDING:
        building = false;
        break;

      // State not found / Error.
      default:
        building = false;
        this->state = BuilderState::ERROR_BUILDING;
        break;
    }
  }
}

/**
 * @brief Apply widget attributes from registry to corresponding pool instances.
 * 
 * Iterates through all registered widget types, retrieves their attribute definitions
 * from the registry, and applies these configurations to the matching widget instances
 * in the pool.
 */
void WidgetBuilder::setAttribute() {

  // Process each widget type registered in the system.
  for (auto& entry : *widgetRegistry) {
    Utils::Widget::Type widgetType = entry.key; // Get the widget type.
    Utils::Widget::Attributes* widgetAttributes = entry.value; // Get the widget attributes.
    Widget* widgetPointer = widgetPool->getWidget(widgetType); // Get the widget from the pool.

    if (widgetAttributes && widgetPointer) {
      
      switch (widgetType) {
        
        case Utils::Widget::Type::GRAPH: {
          auto& graph = widgetAttributes->data.graph;
          Graticules* graticules = static_cast<Graticules*>(widgetPointer);
          graticules->configure(
              &widgetAttributes->geometry.boundary,
              graph.originColor,
              graph.originThickness,
              graph.gridlineColor,
              graph.gridlineThickness,
              graph.xDivisionQty,
              graph.yDivisionQty,
              graph.xOriginIndex,
              graph.yOriginIndex
          );
          break;
        }

        default:
          break;
      }
    } else {
      this->state = BuilderState::ERROR_BUILDING;
      return;
    }
  }

  this->state = BuilderState::COMPLETE; // All widgets processed successfully.
}



/**
 * @brief Get the current state of the widget builder
 * 
 * @return Reference to the current BuilderState for monitoring build progress.
 */
BuilderState& WidgetBuilder::getState() {
  return this->state;
}