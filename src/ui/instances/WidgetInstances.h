#ifndef WIDGETINSTANCES_H
#define WIDGETINSTANCES_H

/**
 * @file WidgetInstances.h
 * @brief Global widget instance declarations.
 * 
 * This file declares global instances of widgets used throughout the application and
 * adds them to the widget pool.
 */

#include "ui/widgets/Widget.h"
#include "ui/pool/WidgetPool.h"
#include "ui/widgets/Graticules.h"

namespace UI {
  namespace Instances {
    namespace Widgets {
      
      // Global widget instances.
      extern Graticules graticules;
      extern Widget label;
      
      void addWidgets(WidgetPool& pool);
    }
  }
}

#endif