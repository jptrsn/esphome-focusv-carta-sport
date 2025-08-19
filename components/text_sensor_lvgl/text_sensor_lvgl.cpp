#include "text_sensor_lvgl.h"

namespace esphome {
namespace text_sensor_lvgl {

class TextSensorLVGLPlatform : public text_sensor::TextSensor {
 public:
  TextSensorLVGLPlatform(TextSensorLVGL *sensor, lv_obj_t* widget) {
    sensor->set_widget(widget);
  }
};

}  // namespace text_sensor_lvgl
}  // namespace esphome
