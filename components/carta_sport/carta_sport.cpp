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

  // Initialize the Carta Sport service UUID - try different methods based on ESPHome version
  auto uuid_str = std::string(CARTA_SPORT_SERVICE_UUID);
  this->carta_sport_service_uuid_ = esp32_ble_tracker::ESPBTUUID::from_raw(uuid_str);

  // Set up auto-connect behavior based on whether MAC address is provided
  this->auto_connect_enabled_ = this->target_mac_address_.empty();
  this->last_log_time_ = 0;

  // Register as a device listener
  esp32_ble_tracker::esphome::esp32_ble_tracker::global_esp32_ble_tracker->register_listener(this);
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

  if (this->connected_) {
      // `esphome::esp32_ble_tracker::global_esp32_ble_tracker->is_connected()` checks the last known status
      bool still_connected = false;
      if (esphome::esp32_ble_tracker::global_esp32_ble_tracker) {
          still_connected = esphome::esp32_ble_tracker::global_esp32_ble_tracker->is_connected(this->device_address_);
      }

      if (!still_connected) {
          ESP_LOGW(TAG, "Lost connection to %s", this->device_address_.c_str());
          this->reset_connection();

          // Restart BLE discovery so we can find the device again
          if (esphome::esp32_ble_tracker::global_esp32_ble_tracker) {
              ESP_LOGD(TAG, "Restarting BLE discovery");
              esphome::esp32_ble_tracker::global_esp32_ble_tracker->start_discovering();
          }
      }
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
  }

  // Check if this device has the Carta Sport service
  if (this->check_device_service_uuid_(device)) {
    if (this->discovered_mac_ != device.address_str()) {
      ESP_LOGI(TAG, "Discovered Carta Sport device: %s", device.address_str().c_str());
      this->discovered_mac_ = device.address_str();

      // Stop the global BLE scanner
      if (esphome::esp32_ble_tracker::global_esp32_ble_tracker != nullptr) {
        ESP_LOGD(TAG, "Stopping BLE discovery");
        esphome::esp32_ble_tracker::global_esp32_ble_tracker->stop_discovering();
      }

      // Store device name for template access
      std::string device_name = device.get_name();
      if (device_name.empty()) {
        device_name = "Unknown Carta Sport";
      }
      this->discovered_device_name_ = device_name;
      ESP_LOGI(TAG, "Device name: %s", device_name.c_str());
    }
    // Attempt to connect if we aren't already
    if (!this->connected_) {
      this->try_connect(device);
    }
    return true;
  }

  return false;
}

bool CartaSportDiscovery::check_device_service_uuid_(const esp32_ble_tracker::ESPBTDevice &device) {
  // Method 1: Check device name pattern
  std::string device_name = device.get_name();
  if (!device_name.empty()) {
    // Look for "CARTA SPORT" in the device name
    if (device_name.find("CARTA SPORT") != std::string::npos) {
      ESP_LOGD(TAG, "Found device by name pattern: %s", device_name.c_str());
      return true;
    }
  }

  // Method 2: Check manufacturer specific data
  auto manufacturer_datas = device.get_manufacturer_datas();
  for (auto &data : manufacturer_datas) {
    // Carta Sport manufacturer ID is 0x0211 (529 decimal)
    if (data.uuid == esp32_ble_tracker::ESPBTUUID::from_uint32(0x0211)) {
      ESP_LOGD(TAG, "Found device by manufacturer ID: 0x0211");
      return true;
    }
  }

  // Method 3: Original service UUID check (keep as fallback)
  auto service_datas = device.get_service_datas();
  for (auto &data : service_datas) {
    if (data.uuid == this->carta_sport_service_uuid_) {
      ESP_LOGD(TAG, "Found device by service UUID");
      return true;
    }
  }

  auto service_uuids = device.get_service_uuids();
  for (auto &service_uuid : service_uuids) {
    if (service_uuid == this->carta_sport_service_uuid_) {
      ESP_LOGD(TAG, "Found device by advertised service UUID");
      return true;
    }
  }

  return false;
}

void CartaSportDiscovery::try_connect(const esp32_ble_tracker::ESPBTDevice &device) {
  if (!esphome::esp32_ble_tracker::global_esp32_ble_tracker) {
    ESP_LOGE(TAG, "No BLE tracker available");
    return;
  }

  // ESP32 BLE Tracker handles the connection life‑cycle
  ESP_LOGI(TAG, "Attempting connection to %s", device.address_str().c_str());

  // `connect_to_device` returns a bool but we can ignore it for now
  esphome::esp32_ble_tracker::global_esp32_ble_tracker->connect_to_device(device, /*reconnect=*/true);

  // The ESPBTDevice will set a callback on disconnect, so we can set the flag here.
  // For ESPHome, the device becomes connected immediately after this call
  // (or after the next loop cycle).
  this->connected_ = true;
  this->device_address_ = device.address_str();
}

}  // namespace carta_sport
}  // namespace esphome

#endif