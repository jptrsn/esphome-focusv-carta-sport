#pragma once
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "focusv_carta_sport.h"

namespace esphome {
namespace focusv_carta_sport {

class TemperatureSensor : public sensor::Sensor, public PollingComponent {
 public:
  void update() override {}  // handled via notifications
};

class CountdownSensor : public sensor::Sensor, public PollingComponent {
 public:
  void update() override {}
};

class ConnectedBinarySensor : public binary_sensor::BinarySensor, public Component {};

}  // namespace focusv_carta_sport
}  // namespace esphome
