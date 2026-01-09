#include "utils/WidgetAttributes.h"
#include "ui/registry/WidgetRegistry.h"
#include "ui/definitions/WidgetDefinitions.h"

// Defines the widget configurations.
namespace UI {
  namespace Definitions {
    namespace Widgets {

      // Global widget attributes.
      Utils::Widget::Attributes graticules;
      Utils::Widget::Attributes label;
      
      // Attribute helper functions.
      void setGraticules() {
        // Metadata
        graticules.isCustom = true;
        graticules.type = Utils::Widget::Type::GRAPH;
        graticules.role = Utils::Widget::Role::INFORMATIVE;
        graticules.name = "Graticule";
        
        // Layout & appearance.
        graticules.geometry.mode = Utils::Widget::SizingMode::AREA_PERCENT;
        graticules.geometry.percentSize = 70;
        
        // Type-specific widget configuration data.
        graticules.data.graph.originColor = Utils::Widget::Color::RED;
        graticules.data.graph.originThickness = 2;
        graticules.data.graph.gridlineColor = Utils::Widget::Color::BLACK;
        graticules.data.graph.gridlineThickness = 1;
        graticules.data.graph.xDivisionQty = 10;
        graticules.data.graph.yDivisionQty = 8;
        graticules.data.graph.xOriginIndex = 5;
        graticules.data.graph.yOriginIndex = 4;
      }

      void setLabel() {
        // Metadata
        label.isCustom = false; // Just being explicit, false = default.
        label.type = Utils::Widget::Type::LABEL;
        label.role = Utils::Widget::Role::FUNCTIONAL;
        label.name = "Label";

        // Layout & appearance.
        label.geometry.mode = Utils::Widget::SizingMode::ABSOLUTE;
        label.geometry.width = 150;
        label.geometry.height = 25;
        label.position.alignment = LV_ALIGN_BOTTOM_LEFT;

        label.label.text = "A Label!";
        label.text.textColor = Utils::Widget::Color::BLACK;
        label.text.textAlign = LV_TEXT_ALIGN_CENTER;
        label.text.letterSpacing = 5;
        label.text.lineSpacing = 10;

        label.background.backgroundColor = Utils::Widget::Color::WHITE;
        label.background.backgroundOpacity = 255;
        label.border.borderWidth = 2;
        label.border.borderColor = Utils::Widget::Color::BLACK;
        label.border.borderOpacity = 255;
      }

      /**
       * @brief Register all predefined widget configurations in the widget registry.
       * 
       * This function is the central registration point for all
       * widgets.
       * 
       * @param registry Reference to the widget registry where configurations will be stored.
       */
      void registerWidgets(WidgetRegistry& registry) {
        // Helper functions.
        setGraticules();
        setLabel();
        
        // Registration
        registry.registerWidget(Utils::Widget::Type::GRAPH, &graticules);
        registry.registerWidget(Utils::Widget::Type::LABEL, &label);
      }
    }
  }
}

