#include "utils/WidgetAttributes.h"
#include "ui/registry/WidgetRegistry.h"

/**
 * @brief Get the singleton instance of WidgetRegistry.
 * 
 * The instance is created on first access and persists for the application lifetime.
 * 
 * @return Reference to the singleton WidgetRegistry instance.
 */
WidgetRegistry& WidgetRegistry::getInstance() {
  static WidgetRegistry instance;
  return instance;
}

/**
 * @brief Registers a widget attribute in the pool for global access.
 * 
 * Associates a widget attribute with its type identifier in the internal hash table.
 * If a widget of the same type already exists, it will be replaced with the new instance.
 * The registry maintains ownership references but does not manage attribute lifecycle.
 * 
 * @param key Widget type identifier.
 * @param value Pointer to widget attribute to store (must not be nullptr)
 * 
 */
void WidgetRegistry::registerWidget(const Utils::Widget::Type& key, Utils::Widget::Attributes* const value) {
  this->registry.insert(key, value);
}

/**
 * @brief Retrieves a widget attribute from the registry by type identifier.
 * 
 * Performs hash table lookup to locate widget attribute associated with the given type.
 * 
 * @param key Widget type identifier.
 * @return Pointer to widget attribute if found, nullptr if the type is not registered.
 * 
 */
Utils::Widget::Attributes* WidgetRegistry::getAttribute(const Utils::Widget::Type& key) const {
  auto* result = this->registry.find(key); // Returns a pointer to a pointer or a nullptr.
  return result ? *result : nullptr; // Derefence the pointer.
}

/**
 * @brief Removes a widget attribute from the registry by type identifier.
 * 
 * Performs hash table removal to disassociate the widget attribute from its type key.
 * Does not delete the widget attribute itself. If the specified type is not found, 
 * the operation is ignored.
 * 
 * @param key Widget type identifier.
 * @return true if widget attribute was found and removed, false if type not registered.
 *
 */
void WidgetRegistry::removeWidget(const Utils::Widget::Type& key) {
  this->registry.remove(key);
}

/**
 * @brief Evaluates if the registry is empty.
 * 
 */
bool WidgetRegistry::isEmpty() {
  bool registryEmpty = this->registry.isEmpty();
  return registryEmpty;
}