#include "focusv_carta_sport.h"
#include "const.h"
#include "esphome/core/log.h"

namespace esphome {
namespace focusv_carta_sport {

static const char *const TAG = "focusv_carta_sport";

void FocusVCartaSportComponent::setup() {
  ESP_LOGI(TAG, "Setting up FocusV Carta Sport...");
  this->set_address_type(ble::BLEClient::ADDRESS_TYPE_RANDOM);
  this->set_service_uuid(ESPBTUUID::from_raw(SERVICE_UUID));
}

void FocusVCartaSportComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "FocusV Carta Sport:");
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor);
  LOG_SENSOR("  ", "Countdown", this->countdown_sensor);
  LOG_BINARY_SENSOR("  ", "Connected", this->connected_sensor);
}

void FocusVCartaSportComponent::gattc_event_handler(esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "Connected to device");
      this->connected_ = true;
      if (this->connected_sensor)
        this->connected_sensor->publish_state(true);
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGW(TAG, "Disconnected from device");
      this->connected_ = false;
      if (this->connected_sensor)
        this->connected_sensor->publish_state(false);
      break;

    case ESP_GATTC_NOTIFY_EVT: {
      auto &notify = param->notify;
      ESP_LOGD(TAG, "Notification received: handle=0x%04x, len=%d",
               notify.handle, notify.value_len);

      // TODO: parse actual temperature & countdown from notify.value
      if (this->temperature_sensor) {
        float temp = notify.value[0];  // placeholder
        this->temperature_sensor->publish_state(temp);
      }
      if (this->countdown_sensor) {
        float countdown = notify.value[1];  // placeholder
        this->countdown_sensor->publish_state(countdown);
      }
      break;
    }

    default:
      break;
  }
}

}  // namespace focusv_carta_sport
}  // namespace esphome
