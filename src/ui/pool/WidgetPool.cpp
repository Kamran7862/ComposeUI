#include "ui/widgets/Widget.h"
#include "ui/pool/WidgetPool.h"
#include "utils/WidgetAttributes.h"

/**
 * @brief Get the singleton instance of WidgetPool.
 * 
 * The instance is created on first access and persists for the application lifetime.
 * 
 * @return Reference to the singleton WidgetPool instance.
 */
WidgetPool& WidgetPool::getInstance() {
  static WidgetPool instance;
  return instance;
}

/**
 * @brief Stores a widget instance in the pool for global access.
 * 
 * Associates a widget pointer with its type identifier in the internal hash table.
 * If a widget of the same type already exists, it will be replaced with the new instance.
 * The pool maintains ownership references but does not manage widget lifecycle.
 * 
 * @param key Widget type identifier.
 * @param value Pointer to widget instance to store (must not be nullptr)
 * 
 */
void WidgetPool::addWidget(const Utils::Widget::Type& key, Widget* const value) {
  this->pool.insert(key, value);
}

/**
 * @brief Retrieves a widget instance from the pool by type identifier.
 * 
 * Performs hash table lookup to locate widget pointer associated with the given type.
 * 
 * @param key Widget type identifier.
 * @return Pointer to widget instance if found, nullptr if the type is not registered.
 * 
 */
Widget* WidgetPool::getWidget(const Utils::Widget::Type& key) const {
  auto* result = this->pool.find(key); // Returns a pointer to a pointer or a nullptr.
  return result ? *result : nullptr; // Derefence the pointer.
}

/**
 * @brief Removes a widget from the pool by type identifier.
 * 
 * Performs hash table removal to disassociate the widget pointer from its type key.
 * Does not delete the widget instance itself. If the specified type is not found, 
 * the operation is ignored.
 * 
 * @param key Widget type identifier.
 * @return true if widget was found and removed, false if type not stored.
 *
 */
bool WidgetPool::removeWidget(const Utils::Widget::Type& key) {
  return this->pool.remove(key);
}

/**
 * @brief Evaluates if the pool is empty.
 * 
 */
bool WidgetPool::isEmpty() {
  bool poolEmpty = this->pool.isEmpty();
  return poolEmpty;
}