#ifndef WIDGETPOOL_H
#define WIDGETPOOL_H

#include "utils/HashTable.h"
#include "utils/Constants.h"
#include "ui/widgets/Widget.h"
#include "utils/WidgetAttributes.h"

/**
 * @class WidgetPool
 * @brief Singleton repository for widget instance storage.
 * 
 * The WidgetPool stores all of the widgets in the application.
 * 
 */
class WidgetPool {
private:

  using Pool = HashTable<Utils::Widget::Type, Widget*, Utils::ArrayConstants::MAX_WIDGETS>;
  Pool pool;

  WidgetPool() = default;
  ~WidgetPool() = default;

  WidgetPool(const WidgetPool&) = delete; // No copy.
  WidgetPool& operator=(const WidgetPool&) = delete; // No copy assignment.
  WidgetPool(WidgetPool&&) = delete; // No move.
  WidgetPool& operator=(WidgetPool&&) = delete; // No move assignment.

public:

  static WidgetPool& getInstance();
  void addWidget(const Utils::Widget::Type& key, Widget* const value);
  Widget* getWidget(const Utils::Widget::Type& key) const; 
  bool removeWidget(const Utils::Widget::Type& key);
  bool isEmpty();

  auto begin() { return pool.begin(); }
  auto end() { return pool.end(); }
  auto begin() const { return pool.begin(); }
  auto end()   const { return pool.end(); }
};

#endif