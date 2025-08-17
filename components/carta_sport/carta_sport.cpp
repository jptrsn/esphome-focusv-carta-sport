#include "carta_sport.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace carta_sport {

static const char *const TAG = "carta_sport";

CartaSportDiscovery *global_carta_sport_discovery = nullptr;

void CartaSportDiscovery::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Focus V Carta Sport Discovery...");

  global_carta_sport_discovery = this;

  // Initialize the Carta Sport service UUID
  this->carta_sport_service_uuid_ = esp32_ble_tracker::ESPBTUUID::from_uuid128_string(CARTA_SPORT_SERVICE_UUID);

  // Set up auto-connect behavior based on whether MAC address is provided
  this->auto_connect_enabled_ = this->target_mac_address_.empty();
  this->last_log_time_ = 0;

  // Register as a device listener
  esp32_ble_tracker::global_esp32_ble_tracker->register_listener(this);
}

void CartaSportDiscovery::loop() {
  // Periodically log discovery status
  uint32_t now = millis();
  if (now - this->last_log_time_ > 30000) {  // Every 30 seconds
    if (this->auto_connect_enabled_ && this->discovered_mac_.empty()) {
      ESP_LOGD(TAG, "Still scanning for Carta Sport devices...");
    }
    this->last_log_time_ = now;
  }
}

void CartaSportDiscovery::dump_config() {
  ESP_LOGCONFIG(TAG, "Focus V Carta Sport Discovery:");
  ESP_LOGCONFIG(TAG, "  Service UUID: %s", CARTA_SPORT_SERVICE_UUID);
  if (!this->auto_connect_enabled_) {
    ESP_LOGCONFIG(TAG, "  Target MAC: %s", this->target_mac_address_.c_str());
  } else {
    ESP_LOGCONFIG(TAG, "  Auto-discovery: Enabled");
  }
  if (!this->discovered_mac_.empty()) {
    ESP_LOGCONFIG(TAG, "  Discovered Device: %s", this->discovered_mac_.c_str());
  }
}

bool CartaSportDiscovery::parse_device(const esp32_ble_tracker::ESPBTDevice &device) {
  // If we have a specific MAC address target, only check that device
  if (!this->auto_connect_enabled_) {
    if (device.address_str() != this->target_mac_address_) {
      return false;
}

void CartaSportDeviceNameSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Carta Sport Device Name Sensor...");
}

void CartaSportDeviceNameSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "Carta Sport Device Name Sensor");
  LOG_TEXT_SENSOR("", "Device Name", this);
}

void CartaSportDeviceNameSensor::update_device_name(const std::string &name) {
  if (this->state != name) {
    ESP_LOGD(TAG, "Device name updated: %s", name.c_str());
    this->publish_state(name);
  }
}

void CartaSportDeviceNameSensor::clear_device_name() {
  if (!this->state.empty()) {
    ESP_LOGD(TAG, "Device name cleared");
    this->publish_state("");
  }
    }
  }

  // Check if this device has the Carta Sport service
  if (this->check_device_service_uuid_(device)) {
    if (this->discovered_mac_ != device.address_str()) {
      ESP_LOGI(TAG, "Discovered Carta Sport device: %s", device.address_str().c_str());
      this->discovered_mac_ = device.address_str();

      // Store device name for template access
      std::string device_name = device.get_name();
      if (device_name.empty()) {
        device_name = "Unknown Carta Sport";
      }
      this->discovered_device_name_ = device_name;
      ESP_LOGI(TAG, "Device name: %s", device_name.c_str());
    }
    return true;
  }

  return false;
}

bool CartaSportDiscovery::check_device_service_uuid_(const esp32_ble_tracker::ESPBTDevice &device) {
  // Check if the device advertises the Carta Sport service UUID
  for (auto &service_uuid : device.get_service_uuids()) {
    if (service_uuid == this->carta_sport_service_uuid_) {
      return true;
    }
  }
  return false;
}

}  // namespace carta_sport
}  // namespace esphome

#endif