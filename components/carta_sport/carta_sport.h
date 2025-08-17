#pragma once

#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace carta_sport {

// Focus V Carta Sport service UUID
static const char *const CARTA_SPORT_SERVICE_UUID = "1011123e-8535-b5a0-7140-a304d2495cb7";

class CartaSportDeviceNameSensor;

class CartaSportDiscovery : public Component, public esp32_ble_tracker::ESPBTDeviceListener {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_BLUETOOTH; }

  void set_target_mac_address(const std::string &mac) { this->target_mac_address_ = mac; }
  void set_device_name_sensor(CartaSportDeviceNameSensor *sensor) { this->device_name_sensor_ = sensor; }

  bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;

  // Get the discovered MAC address (for use by other components)
  std::string get_discovered_mac_address() const { return this->discovered_mac_; }
  bool has_discovered_device() const { return !this->discovered_mac_.empty(); }

 protected:
  std::string target_mac_address_;
  std::string discovered_mac_;
  bool auto_connect_enabled_;
  esp32_ble_tracker::ESPBTUUID carta_sport_service_uuid_;
  uint32_t last_log_time_;
  CartaSportDeviceNameSensor *device_name_sensor_{nullptr};

  bool check_device_service_uuid_(const esp32_ble_tracker::ESPBTDevice &device);
};

// Global pointer for other components to access
extern CartaSportDiscovery *global_carta_sport_discovery;

class CartaSportDeviceNameSensor : public text_sensor::TextSensor, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA - 1.0f; }

  void update_device_name(const std::string &name);
  void clear_device_name();
};

}  // namespace carta_sport
}  // namespace esphome

#endif