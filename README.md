# ESPHome External Component for Focus V Carta Sport

This component connects to the Focus V Carta Sport vaporizer over BLE and exposes:

- Temperature (°C)
- Countdown timer (seconds)
- Connected state

## Usage

```yaml
external_components:
  - source: github://yourusername/esphome-focusv-carta-sport

esp32_ble_tracker:

focusv_carta_sport:
  temperature:
    name: "Carta Sport Temperature"
  countdown:
    name: "Carta Sport Countdown"
  connected:
    name: "Carta Sport Connected"

```