#ifndef WIDGETREGISTRY_H
#define WIDGETREGISTRY_H

#include "utils/HashTable.h"
#include "utils/Constants.h"
#include "utils/WidgetAttributes.h"

/**
 * @class WidgetRegistry
 * @brief Singleton repository for widget attribute storage.
 * 
 * The WidgetRegistry stores all of the widget attributes in the application.
 * 
 */
class WidgetRegistry {
private:

  using Registry = HashTable<Utils::Widget::Type, Utils::Widget::Attributes*, Utils::ArrayConstants::MAX_WIDGETS>;
  Registry registry;

  WidgetRegistry() = default;
  ~WidgetRegistry() = default;

  WidgetRegistry(const WidgetRegistry&) = delete; // No copy.
  WidgetRegistry& operator=(const WidgetRegistry&) = delete; // No copy assignment.
  WidgetRegistry(WidgetRegistry&&) = delete; // No move.
  WidgetRegistry& operator=(WidgetRegistry&&) = delete; // No move assignment.

public:

  static WidgetRegistry& getInstance();
  void registerWidget(const Utils::Widget::Type& key,  Utils::Widget::Attributes* const value);
  Utils::Widget::Attributes* getAttribute(const Utils::Widget::Type& key) const;
  void removeWidget(const Utils::Widget::Type& key);
  bool isEmpty();

  auto begin() { return registry.begin(); }
  auto end() { return registry.end(); }
  auto begin() const { return registry.begin(); }
  auto end()   const { return registry.end(); }
};



#endif