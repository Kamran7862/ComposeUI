#include "ui/widgets/Widget.h"
#include "ui/pool/WidgetPool.h"
#include "ui/widgets/Graticules.h"
#include "render/GraticulesRenderer.h"
#include "ui/instances/WidgetInstances.h"

// Defines the widgets.
namespace UI {
  namespace Instances {
    namespace Widgets {

      // Define global widget instances.
      Graticules graticules;
      Widget label;

      // Custom widget callback helper function.
      void setDrawEvent() {
        graticules.setCallback(graticule_draw_event_cb);
      }

      /**
       * @brief Initialize all global widget instances.
       * 
       * Sets up the configuration and initial state of all global widget instances.
       *
       * @param pool Reference to the widget pool where widgets will be stored.
       */
      void addWidgets(WidgetPool& pool) {
        // Helper functions to set custom widget callbacks.
        setDrawEvent();

        // Add widgets to the global widget pool.
        pool.addWidget(Utils::Widget::Type::GRAPH, &graticules);
        pool.addWidget(Utils::Widget::Type::LABEL, &label);
      }
    }
  }
}