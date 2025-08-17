#include "focusv_carta_sport.h"
#include "esphome/core/log.h"

namespace esphome {
namespace focusv_carta_sport {

static const char *const TAG = "focusv_carta_sport";

// Replace with real UUIDs once identified
static const char *SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0";
static const char *CHAR_NOTIFY_UUID = "12345678-1234-5678-1234-56789abcdef1";

void FocusVCartaSportComponent::setup() {
  ESP_LOGI(TAG, "Setting up FocusV Carta Sport...");
  this->parent_->register_client(this);
}

void FocusVCartaSportComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "FocusV Carta Sport");
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor);
  LOG_SENSOR("  ", "Countdown", this->countdown_sensor);
  LOG_BINARY_SENSOR("  ", "Connected", this->connected_sensor);
}

void FocusVCartaSportComponent::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                                    esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "Connected to Carta Sport");
      this->connected_ = true;
      if (this->connected_sensor != nullptr)
        this->connected_sensor->publish_state(true);
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGW(TAG, "Disconnected from Carta Sport");
      this->connected_ = false;
      if (this->connected_sensor != nullptr)
        this->connected_sensor->publish_state(false);
      break;

    case ESP_GATTC_NOTIFY_EVT: {
      auto &notify = param->notify;
      ESP_LOGD(TAG, "Notification received (%d bytes)", notify.value_len);

      // Example decoding:
      if (notify.value_len > 18) {
        uint8_t countdown = notify.value[18];
        if (this->countdown_sensor != nullptr)
          this->countdown_sensor->publish_state(countdown);
      }
      if (notify.value_len > 5) {
        uint16_t temp = notify.value[5];  // TODO: real position
        if (this->temperature_sensor != nullptr)
          this->temperature_sensor->publish_state(temp);
      }
      break;
    }

    default:
      break;
  }
}

}  // namespace focusv_carta_sport
}  // namespace esphome
