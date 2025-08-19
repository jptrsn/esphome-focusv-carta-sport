#pragma once
#include "esphome.h"
#include "lvgl.h"

using namespace esphome;

class TextSensorLVGL : public text_sensor::TextSensor, public Component {
 public:
  lv_obj_t* widget = nullptr;

  void set_widget(lv_obj_t* w) { widget = w; }

  void publish_state(const std::string &state) override {
    // Update LVGL widget
    if (widget) {
      const char* type = lv_obj_get_type(widget);
      if (type && strstr(type, "label")) {
        lv_label_set_text(widget, state.c_str());
      } else if (type && strstr(type, "textarea")) {
        lv_textarea_set_text(widget, state.c_str());
      }
      lv_obj_invalidate(widget);
    }

    // Normal text sensor publish
    text_sensor::TextSensor::publish_state(state);
  }
};
