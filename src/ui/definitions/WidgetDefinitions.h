#ifndef WIDGETDEFINITIONS_H
#define WIDGETDEFINITIONS_H

/**
 * @file WidgetDefinitions.h
 * @brief Pre-configured widget attribute definitions.
 * 
 * This file declares widget configurations that get registered in the
 * WidgetRegistry.
 * 
 */

#include "utils/WidgetAttributes.h"
#include "ui/registry/WidgetRegistry.h"

namespace UI {
  namespace Definitions {
    namespace Widgets {

      // Global widget attributes
      extern Utils::Widget::Attributes graticules;
      extern Utils::Widget::Attributes label;

      void registerWidgets(WidgetRegistry& registry);
    } 
  }
}

#endif