#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

#ifdef USE_ESP32

namespace esphome {
namespace carta_sport {

static const char *const TAG = "carta_sport";

class CartaSportComponent : public ble_client::BLEClientNode, public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_address(uint64_t address) { this->address_ = address; }
  void set_carta_service_uuid(const std::string &uuid) { this->carta_service_uuid_ = uuid; }
  void set_secondary_service_uuid(const std::string &uuid) { this->secondary_service_uuid_ = uuid; }
  void set_device_name_char_uuid(const std::string &uuid) { this->device_name_char_uuid_ = uuid; }
  void set_temperature_char_uuid(const std::string &uuid) { this->temperature_char_uuid_ = uuid; }
  void set_battery_char_uuid(const std::string &uuid) { this->battery_char_uuid_ = uuid; }

  // Sensor setters
  void set_device_name_sensor(text_sensor::TextSensor *sensor) { this->device_name_sensor_ = sensor; }
  void set_temperature_sensor(sensor::Sensor *sensor) { this->temperature_sensor_ = sensor; }
  void set_battery_sensor(sensor::Sensor *sensor) { this->battery_sensor_ = sensor; }
  void set_rssi_sensor(sensor::Sensor *sensor) { this->rssi_sensor_ = sensor; }

 protected:
  uint64_t address_;
  std::string carta_service_uuid_;
  std::string secondary_service_uuid_;
  std::string device_name_char_uuid_;
  std::string temperature_char_uuid_;
  std::string battery_char_uuid_;

  text_sensor::TextSensor *device_name_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *battery_sensor_{nullptr};
  sensor::Sensor *rssi_sensor_{nullptr};

  bool connected_{false};
  esp32_ble_tracker::ESPBTUUID carta_service_uuid_obj_;
  esp32_ble_tracker::ESPBTUUID secondary_service_uuid_obj_;
  esp32_ble_tracker::ESPBTUUID device_name_char_uuid_obj_;
  esp32_ble_tracker::ESPBTUUID temperature_char_uuid_obj_;
  esp32_ble_tracker::ESPBTUUID battery_char_uuid_obj_;

  // BLE Client callbacks
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                          esp_ble_gattc_cb_param_t *param) override;

 private:
  void read_device_characteristics_();
  void handle_device_name_read_(const uint8_t *data, uint16_t length);
  void handle_temperature_read_(const uint8_t *data, uint16_t length);
  void handle_battery_read_(const uint8_t *data, uint16_t length);

  // Service handles
  uint16_t carta_service_handle_{0};
  uint16_t secondary_service_handle_{0};
  uint16_t gap_service_handle_{0};  // Generic Access Profile service
  uint16_t battery_service_handle_{0};

  // Characteristic handles
  uint16_t device_name_char_handle_{0};
  uint16_t temperature_char_handle_{0};
  uint16_t battery_char_handle_{0};

  bool characteristics_found_{false};
};

}  // namespace carta_sport
}  // namespace esphome

#endif