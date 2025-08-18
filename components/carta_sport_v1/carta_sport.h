#pragma once

#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace carta_sport {

// Focus V Carta Sport service UUID
static const char *const CARTA_SPORT_SERVICE_UUID = "1011123e-8535-b5a0-7140-a304d2495cb7";

class CartaSportDiscovery : public Component, public esp32_ble_tracker::ESPBTDeviceListener {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_BLUETOOTH; }

  void set_target_mac_address(const std::string &mac) { this->target_mac_address_ = mac; }

  // Get the discovered device name (for use in templates)
  std::string get_discovered_device_name() const { return this->discovered_device_name_; }

  // Get device connection status
  bool is_connected() const { return connected_; }
  std::string get_connected_mac() const { return device_address_; }

  bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;

  // Get the discovered MAC address (for use by other components)
  std::string get_discovered_mac_address() const { return this->discovered_mac_; }
  bool has_discovered_device() const { return !this->discovered_mac_.empty(); }

  // Reset the state of the connected device
  void reset_connection() {
    this->connected_ = false;
    this->device_address_.clear();
  }

 protected:
  std::string target_mac_address_;
  std::string discovered_mac_;
  std::string discovered_device_name_;
  bool auto_connect_enabled_;
  esp32_ble_tracker::ESPBTUUID carta_sport_service_uuid_;
  uint32_t last_log_time_;
  bool connected_ = false;                     // true when we are connected
  std::string device_address_ = "";            // address we are connected to

  bool check_device_service_uuid_(const esp32_ble_tracker::ESPBTDevice &device);
};

// Global pointer for other components to access
extern CartaSportDiscovery *global_carta_sport_discovery;

}  // namespace carta_sport
}  // namespace esphome

#endif