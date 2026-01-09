#include <lvgl.h>
#include "ui/widgets/Widget.h"

/**
 * @brief Retrieves the lvgl object pointer.
 * 
 * @return Pointer to the lvgl object, or nullptr if not set.
 * 
 */
lv_obj_t* Widget::getLVGL() const {
  return this->lvgl_object_pointer;
}

/**
 * @brief Retrieves the lvgl event callback function pointer.
 * 
 * @return Function pointer to event callback, or nullptr if no callback set.
 * 
 */
void (*Widget::getCallback() const)(lv_event_t* event) {
  return this->callback;
}

/**
 * @brief Associates an lvgl object with the widget instance.
 * 
 * @param lvgl_object_pointer Pointer to an initialized lvgl object (must not be nullptr).
 * 
 */
void Widget::setLVGL(lv_obj_t* const lvgl_object_pointer) {
  this->lvgl_object_pointer = lvgl_object_pointer;
}

/**
 * @brief Registers an event callback function for the widget.
 * 
 * @param callback Function pointer accepting lv_event_t* parameter, or nullptr to clear.
 * 
 */
void Widget::setCallback(void (*callback)(lv_event_t* event)) {
  this->callback = callback;
}

