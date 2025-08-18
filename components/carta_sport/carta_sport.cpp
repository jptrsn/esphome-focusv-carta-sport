#include "carta_sport.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#ifdef USE_ESP32

namespace esphome {
namespace carta_sport {

static const char *const TAG = "carta_sport";

void CartaSportComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Carta Sport...");

  // Convert string UUIDs to ESPBTUUID objects
  this->carta_service_uuid_obj_ = esp32_ble_tracker::ESPBTUUID::from_string(this->carta_service_uuid_);
  this->secondary_service_uuid_obj_ = esp32_ble_tracker::ESPBTUUID::from_string(this->secondary_service_uuid_);
  this->device_name_char_uuid_obj_ = esp32_ble_tracker::ESPBTUUID::from_string(this->device_name_char_uuid_);
  this->temperature_char_uuid_obj_ = esp32_ble_tracker::ESPBTUUID::from_string(this->temperature_char_uuid_);
  this->battery_char_uuid_obj_ = esp32_ble_tracker::ESPBTUUID::from_string(this->battery_char_uuid_);

  // Set the device address for connection
  this->set_address(this->address_);
}

void CartaSportComponent::loop() {
  // Handle connection state changes and other real-time processing
}

void CartaSportComponent::update() {
  if (this->connected_ && this->characteristics_found_) {
    ESP_LOGD(TAG, "Updating Carta Sport sensors...");
    this->read_device_characteristics_();

    // Update RSSI
    if (this->rssi_sensor_ != nullptr) {
      int rssi = this->parent()->get_rssi();
      if (rssi != 0) {
        this->rssi_sensor_->publish_state(rssi);
      }
    }
  }
}

void CartaSportComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Carta Sport:");
  ESP_LOGCONFIG(TAG, "  MAC Address: %s", format_hex(this->address_).c_str());
  ESP_LOGCONFIG(TAG, "  Carta Service UUID: %s", this->carta_service_uuid_.c_str());
  ESP_LOGCONFIG(TAG, "  Secondary Service UUID: %s", this->secondary_service_uuid_.c_str());
  ESP_LOGCONFIG(TAG, "  Device Name Char: %s", this->device_name_char_uuid_.c_str());
  ESP_LOGCONFIG(TAG, "  Temperature Char: %s", this->temperature_char_uuid_.c_str());
  ESP_LOGCONFIG(TAG, "  Battery Char: %s", this->battery_char_uuid_.c_str());
  LOG_UPDATE_INTERVAL(this);
}

void CartaSportComponent::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                             esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      if (param->open.status == ESP_GATT_OK) {
        ESP_LOGI(TAG, "Connected to Carta Sport device");
        this->connected_ = true;
      } else {
        ESP_LOGW(TAG, "Failed to connect to Carta Sport device");
        this->connected_ = false;
      }
      break;
    }

    case ESP_GATTC_DISCONNECT_EVT: {
      ESP_LOGI(TAG, "Disconnected from Carta Sport device");
      this->connected_ = false;
      this->characteristics_found_ = false;
      break;
    }

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      // Service discovery complete, now find characteristics
      ESP_LOGD(TAG, "Service discovery complete");

      // Find Carta Sport main service
      auto *carta_srv = this->parent()->get_service(this->carta_service_uuid_obj_);
      if (carta_srv != nullptr) {
        this->carta_service_handle_ = carta_srv->handle;
        ESP_LOGD(TAG, "Found Carta service at handle 0x%04X", carta_srv->handle);

        // Look for temperature characteristic in main service
        auto *temp_char = carta_srv->get_characteristic(this->temperature_char_uuid_obj_);
        if (temp_char != nullptr) {
          this->temperature_char_handle_ = temp_char->handle;
          ESP_LOGD(TAG, "Found temperature char at handle 0x%04X", temp_char->handle);
        }
      }

      // Find secondary service
      auto *secondary_srv = this->parent()->get_service(this->secondary_service_uuid_obj_);
      if (secondary_srv != nullptr) {
        this->secondary_service_handle_ = secondary_srv->handle;
        ESP_LOGD(TAG, "Found secondary service at handle 0x%04X", secondary_srv->handle);
      }

      // Find standard GAP service for device name (handle 0x0001-0x0007 from logs)
      esp32_ble_tracker::ESPBTUUID gap_service_uuid = esp32_ble_tracker::ESPBTUUID::from_uint16(0x1800);
      auto *gap_srv = this->parent()->get_service(gap_service_uuid);
      if (gap_srv != nullptr) {
        this->gap_service_handle_ = gap_srv->handle;
        auto *device_name_char = gap_srv->get_characteristic(this->device_name_char_uuid_obj_);
        if (device_name_char != nullptr) {
          this->device_name_char_handle_ = device_name_char->handle;
          ESP_LOGD(TAG, "Found device name char at handle 0x%04X", device_name_char->handle);
        }
      }

      // Find standard battery service (handle 0x0027 from logs)
      esp32_ble_tracker::ESPBTUUID battery_service_uuid = esp32_ble_tracker::ESPBTUUID::from_uint16(0x180F);
      auto *battery_srv = this->parent()->get_service(battery_service_uuid);
      if (battery_srv != nullptr) {
        this->battery_service_handle_ = battery_srv->handle;
        auto *battery_char = battery_srv->get_characteristic(this->battery_char_uuid_obj_);
        if (battery_char != nullptr) {
          this->battery_char_handle_ = battery_char->handle;
          ESP_LOGD(TAG, "Found battery char at handle 0x%04X", battery_char->handle);
        }
      }

      this->characteristics_found_ = true;
      ESP_LOGI(TAG, "Service discovery complete, device ready");
      break;
    }

    case ESP_GATTC_READ_CHAR_EVT: {
      if (param->read.status != ESP_GATT_OK) {
        ESP_LOGW(TAG, "Error reading characteristic: %d", param->read.status);
        break;
      }

      // Handle different characteristic reads
      if (param->read.handle == this->device_name_char_handle_) {
        this->handle_device_name_read_(param->read.value, param->read.value_len);
      } else if (param->read.handle == this->temperature_char_handle_) {
        this->handle_temperature_read_(param->read.value, param->read.value_len);
      } else if (param->read.handle == this->battery_char_handle_) {
        this->handle_battery_read_(param->read.value, param->read.value_len);
      }
      break;
    }

    default:
      break;
  }
}

void CartaSportComponent::read_device_characteristics_() {
  if (!this->connected_ || !this->characteristics_found_) {
    return;
  }

  // Read device name
  if (this->device_name_char_handle_ != 0 && this->device_name_sensor_ != nullptr) {
    esp_err_t status = esp_ble_gattc_read_char(
        this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
        this->device_name_char_handle_, ESP_GATT_AUTH_REQ_NONE);
    if (status != ESP_OK) {
      ESP_LOGW(TAG, "Failed to read device name characteristic");
    }
  }

  // Read temperature
  if (this->temperature_char_handle_ != 0 && this->temperature_sensor_ != nullptr) {
    esp_err_t status = esp_ble_gattc_read_char(
        this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
        this->temperature_char_handle_, ESP_GATT_AUTH_REQ_NONE);
    if (status != ESP_OK) {
      ESP_LOGW(TAG, "Failed to read temperature characteristic");
    }
  }

  // Read battery
  if (this->battery_char_handle_ != 0 && this->battery_sensor_ != nullptr) {
    esp_err_t status = esp_ble_gattc_read_char(
        this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
        this->battery_char_handle_, ESP_GATT_AUTH_REQ_NONE);
    if (status != ESP_OK) {
      ESP_LOGW(TAG, "Failed to read battery characteristic");
    }
  }
}

void CartaSportComponent::handle_device_name_read_(const uint8_t *data, uint16_t length) {
  if (this->device_name_sensor_ != nullptr && data != nullptr && length > 0) {
    std::string device_name((char*)data, length);
    ESP_LOGD(TAG, "Device name: %s", device_name.c_str());
    this->device_name_sensor_->publish_state(device_name);
  }
}

void CartaSportComponent::handle_temperature_read_(const uint8_t *data, uint16_t length) {
  if (this->temperature_sensor_ != nullptr && data != nullptr && length >= 2) {
    // Assuming temperature is sent as 2-byte integer (adjust based on actual protocol)
    uint16_t temp_raw = (data[1] << 8) | data[0];
    float temperature = temp_raw / 100.0f;  // Adjust scaling as needed
    ESP_LOGD(TAG, "Temperature: %.2f°C", temperature);
    this->temperature_sensor_->publish_state(temperature);
  }
}

void CartaSportComponent::handle_battery_read_(const uint8_t *data, uint16_t length) {
  if (this->battery_sensor_ != nullptr && data != nullptr && length >= 1) {
    // Assuming battery level is a single byte percentage
    uint8_t battery_level = data[0];
    ESP_LOGD(TAG, "Battery level: %d%%", battery_level);
    this->battery_sensor_->publish_state(battery_level);
  }
}

}  // namespace carta_sport
}  // namespace esphome

#endif