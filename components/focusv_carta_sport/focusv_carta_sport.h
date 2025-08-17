#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/esp32_ble_client/esp32_ble_client.h"

namespace esphome {
namespace focusv_carta_sport {

class FocusVCartaSportComponent : public Component, public esp32_ble_client::BLEClientNode {
 public:
  void set_temperature_sensor(sensor::Sensor *s) { this->temperature_sensor = s; }
  void set_countdown_sensor(sensor::Sensor *s) { this->countdown_sensor = s; }
  void set_connected_sensor(binary_sensor::BinarySensor *s) { this->connected_sensor = s; }

  void setup() override;
  void dump_config() override;
  void loop() override {}

  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;

 protected:
  sensor::Sensor *temperature_sensor{nullptr};
  sensor::Sensor *countdown_sensor{nullptr};
  binary_sensor::BinarySensor *connected_sensor{nullptr};

  bool connected_ = false;
};

}  // namespace focusv_carta_sport
}  // namespace esphome
