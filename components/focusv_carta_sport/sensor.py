import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    UNIT_CELSIUS, ICON_THERMOMETER, DEVICE_CLASS_TEMPERATURE,
    UNIT_SECOND, ICON_TIMER, DEVICE_CLASS_DURATION
)

from . import focusv_carta_sport_ns, FocusVCartaSportComponent

DEPENDENCIES = ["esp32_ble_tracker"]

TemperatureSensor = focusv_carta_sport_ns.class_("TemperatureSensor", sensor.Sensor)
CountdownSensor = focusv_carta_sport_ns.class_("CountdownSensor", sensor.Sensor)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional("temperature"): sensor.sensor_schema(
            TemperatureSensor,
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional("countdown"): sensor.sensor_schema(
            CountdownSensor,
            unit_of_measurement=UNIT_SECOND,
            icon=ICON_TIMER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DURATION,
        ),
    }
)

async def to_code(config):
    hub = await cg.get_variable("focusv_carta_sport_hub")

    if "temperature" in config:
        sens = await sensor.new_sensor(config["temperature"])
        cg.add(hub.set_temperature_sensor(sens))

    if "countdown" in config:
        sens = await sensor.new_sensor(config["countdown"])
        cg.add(hub.set_countdown_sensor(sens))
