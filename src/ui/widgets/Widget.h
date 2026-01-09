#ifndef WIDGET_H
#define WIDGET_H

#include <lvgl.h>

/**
 * @class Widget
 * @brief The base class for all display widgets.
 * 
 * The Widget class is the parent for all custom/builtin widgets. 
 * Manages object pointers and event callback mechanisms.
 * 
 */
class Widget {
private:
  lv_obj_t* lvgl_object_pointer = nullptr; // LVGL object pointer.
  void (*callback)(lv_event_t* event) = nullptr; // LVGL event callback (e.g., draw, input, update).
public:
  lv_obj_t* getLVGL() const;
  void (*getCallback() const)(lv_event_t* event);
  void setLVGL(lv_obj_t* const lvgl_object_pointer);
  void setCallback(void (*callback)(lv_event_t* event));
};

#endif