#include "carta_sport.h"
#include "esphome/core/log.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

#ifdef USE_ESP32

namespace esphome {
namespace carta_sport {

static const char *const TAG = "carta_sport";

void CartaSportClient::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Focus V Carta Sport BLE Client...");

  // Initialize the Carta Sport service UUID
  this->carta_sport_service_uuid_ = esp32_ble_tracker::ESPBTUUID::from_string(CARTA_SPORT_SERVICE_UUID);

  // Set up auto-connect behavior based on whether MAC address is provided
  this->auto_connect_enabled_ = this->target_mac_address_.empty();
  this->service_found_ = false;
  this->last_scan_time_ = 0;

  if (this->auto_connect_enabled_) {
    ESP_LOGCONFIG(TAG, "Auto-connect enabled - will scan for Carta Sport devices");
    this->start_scan_for_device_();
  } else {
    ESP_LOGCONFIG(TAG, "Connecting to specific Carta Sport at MAC: %s", this->target_mac_address_.c_str());
  }
}

void CartaSportClient::loop() {
  // Let the parent class handle the main loop
  ble_client::BLEClient::loop();

  // If auto-connect is enabled and we're not connected, keep scanning
  if (this->auto_connect_enabled_ &&
      this->state() == esp32_ble_tracker::ClientState::IDLE &&
      !this->service_found_) {
    uint32_t now = millis();
    if (now - this->last_scan_time_ > 30000) {  // Scan every 30 seconds
      this->start_scan_for_device_();
      this->last_scan_time_ = now;
    }
  }
}

void CartaSportClient::dump_config() {
  ESP_LOGCONFIG(TAG, "Focus V Carta Sport BLE Client:");
  ESP_LOGCONFIG(TAG, "  Service UUID: %s", CARTA_SPORT_SERVICE_UUID);
  if (!this->auto_connect_enabled_) {
    ESP_LOGCONFIG(TAG, "  Target MAC: %s", this->target_mac_address_.c_str());
  } else {
    ESP_LOGCONFIG(TAG, "  Auto-connect: Enabled");
  }
  LOG_UPDATE_INTERVAL(this);
}

void CartaSportClient::start_scan_for_device_() {
  if (!this->auto_connect_enabled_) {
    return;
  }

  ESP_LOGD(TAG, "Scanning for Carta Sport devices...");

  // Get current scan results from the tracker
  auto *tracker = esp32_ble_tracker::global_esp32_ble_tracker;
  if (tracker == nullptr) {
    ESP_LOGW(TAG, "BLE tracker not available");
    return;
  }

  // Check existing discovered devices
  for (auto &device : tracker->get_discovered_devices()) {
    if (this->check_device_service_uuid_(device)) {
      ESP_LOGI(TAG, "Found Carta Sport device: %s", device.address_str().c_str());
      this->connect_to_device_(device);
      this->service_found_ = true;
      return;
    }
  }

  ESP_LOGD(TAG, "No Carta Sport devices found in current scan results");
}

bool CartaSportClient::check_device_service_uuid_(const esp32_ble_tracker::ESPBTDevice &device) {
  // Check if the device advertises the Carta Sport service UUID
  for (auto &service_uuid : device.get_service_uuids()) {
    if (service_uuid.contains(this->carta_sport_service_uuid_)) {
      return true;
    }
  }
  return false;
}

void CartaSportClient::connect_to_device_(const esp32_ble_tracker::ESPBTDevice &device) {
  ESP_LOGI(TAG, "Attempting to connect to Carta Sport: %s", device.address_str().c_str());

  // Set the MAC address and initiate connection
  this->set_address(device.address_uint64());
  this->connect();
}

bool CartaSportClient::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                         esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_CONNECT_EVT: {
      ESP_LOGI(TAG, "Connected to Carta Sport successfully");
      break;
    }
    case ESP_GATTC_DISCONNECT_EVT: {
      ESP_LOGW(TAG, "Disconnected from Carta Sport, reason: %d", param->disconnect.reason);
      this->service_found_ = false;  // Reset to allow reconnection
      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      ESP_LOGD(TAG, "Service discovery complete on Carta Sport");

      // Verify that the Carta Sport service is available
      auto *service = this->get_service(this->carta_sport_service_uuid_);
      if (service == nullptr) {
        ESP_LOGW(TAG, "Carta Sport service not found on device, disconnecting");
        this->disconnect();
        this->service_found_ = false;
        return false;
      } else {
        ESP_LOGI(TAG, "Carta Sport service confirmed on device");
      }
      break;
    }
    default:
      break;
  }

  // Call parent event handler
  return ble_client::BLEClient::gattc_event_handler(event, gattc_if, param);
}

}  // namespace carta_sport
}  // namespace esphome

#endif