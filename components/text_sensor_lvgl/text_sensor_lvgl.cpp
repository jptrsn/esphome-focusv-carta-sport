#include "text_sensor_lvgl.h"

namespace esphome {
namespace text_sensor_lvgl {

using namespace esphome;

class TextSensorLVGLComponent : public TextSensorLVGL, public Component {
 public:
  TextSensorLVGLComponent() {}

  void setup() override {
    // ESPHome will call this after construction
  }
};

// -----------------------------
// Platform registration
// -----------------------------
#include "esphome/core/defines.h"
#include "esphome/core/application.h"
#include "esphome/components/text_sensor/text_sensor.h"

TextSensorLVGL *make_text_sensor_lvgl(lv_obj_t *widget) {
  auto sensor = new TextSensorLVGL();
  sensor->set_widget(widget);
  App.register_component(sensor);
  App.register_text_sensor(sensor);
  return sensor;
}

// YAML config schema
static const text_sensor::TextSensorSchema CONFIG_SCHEMA = text_sensor::text_sensor_schema_base();

void register_text_sensor_lvgl(const std::string &widget_id,
                               text_sensor::TextSensor *sensor) {
  lv_obj_t *widget = id(widget_id);
  if (widget) {
    dynamic_cast<TextSensorLVGL *>(sensor)->set_widget(widget);
  }
}

}  // namespace text_sensor_lvgl
}  // namespace esphome
