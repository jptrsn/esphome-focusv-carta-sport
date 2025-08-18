import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_BATTERY_LEVEL,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_DECIBEL_MILLIWATT,
    STATE_CLASS_MEASUREMENT,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

# Import from the parent __init__.py file
from . import carta_sport_ns, CartaSportComponent, CONF_ID as CARTA_SPORT_CONF_ID

DEPENDENCIES = ["carta_sport"]
AUTO_LOAD = ["carta_sport"]

CONF_CARTA_SPORT_ID = "carta_sport_id"
CONF_RSSI = "rssi"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_CARTA_SPORT_ID): cv.use_id(CartaSportComponent),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_RSSI): sensor.sensor_schema(
                unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
        }
    ),
    cv.has_at_least_one_key(CONF_TEMPERATURE, CONF_BATTERY_LEVEL, CONF_RSSI),
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_CARTA_SPORT_ID])

    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(parent.set_temperature_sensor(sens))

    if CONF_BATTERY_LEVEL in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_LEVEL])
        cg.add(parent.set_battery_sensor(sens))

    if CONF_RSSI in config:
        sens = await sensor.new_sensor(config[CONF_RSSI])
        cg.add(parent.set_rssi_sensor(sens))