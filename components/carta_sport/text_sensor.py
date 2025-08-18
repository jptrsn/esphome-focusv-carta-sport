import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

# Import from the parent __init__.py file
from . import carta_sport_ns, CartaSportComponent

DEPENDENCIES = ["carta_sport"]
AUTO_LOAD = ["carta_sport"]

CONF_CARTA_SPORT_ID = "carta_sport_id"
CONF_DEVICE_NAME = "device_name"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_CARTA_SPORT_ID): cv.use_id(CartaSportComponent),
            cv.Optional(CONF_DEVICE_NAME): text_sensor.text_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
        }
    ),
    cv.has_at_least_one_key(CONF_DEVICE_NAME),
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_CARTA_SPORT_ID])

    if CONF_DEVICE_NAME in config:
        sens = await text_sensor.new_text_sensor(config[CONF_DEVICE_NAME])
        cg.add(parent.set_device_name_sensor(sens))