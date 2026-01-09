#ifndef WIDGETBUILDER_H
#define WIDGETBUILDER_H

#include "ui/pool/WidgetPool.h"
#include "ui/registry/WidgetRegistry.h"

/**
 * @enum BuilderState
 * @brief States of the builder's state machine.
 */
enum class BuilderState {
  UNINITIALIZED, // The builder is not initialized or services not set.
  SERVICES_SET, // The builders services (WidgetRegistry, WidgetPool) have been set. 
  ERROR_SERVICES, // There is a problem with the builders services.
  BUILDING, // The builder is setting the properties of the custom widgets.
  ERROR_BUILDING, // No build mechanism for the widget found in the pool.
  COMPLETE // The builder has completed.
};

/**
 * @class WidgetBuilder
 * @brief Singleton orchestrator for custom widget configuration.
 * 
 * The WidgetBuilder applies configuration data to custom widgets.
 * 
 * Key responsibilities:
 * - Coordinates widget attribute application from registry to instances
 * - Manages the build process state machine (services setup → building → completion)
 * 
 */
class WidgetBuilder {

private:

  BuilderState state = BuilderState::UNINITIALIZED; // The state of the builder.

  WidgetRegistry* widgetRegistry = nullptr; // A pointer to the singleton WidgetRegistry service.
  WidgetPool* widgetPool = nullptr; // A pointer to the singleton WidgetPool service.

  WidgetBuilder() = default;
  ~WidgetBuilder() = default;

  WidgetBuilder(const WidgetBuilder&) = delete; // No copy.
  WidgetBuilder& operator=(const WidgetBuilder&) = delete; // No copy assignment.
  WidgetBuilder(WidgetBuilder&&) = delete; // No move.
  WidgetBuilder& operator=(WidgetBuilder&&) = delete; // No move assignment.

  void setAttribute();

public:

  static WidgetBuilder& getInstance();
  void setServices(WidgetRegistry* widgetRegistry, WidgetPool* widgetPool);
  void setWidgets();
  BuilderState& getState();
};

#endif