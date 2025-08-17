import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import DEVICE_CLASS_CONNECTIVITY

from . import focusv_carta_sport_ns, FocusVCartaSportComponent

ConnectedBinarySensor = focusv_carta_sport_ns.class_("ConnectedBinarySensor", binary_sensor.BinarySensor)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional("connected"): binary_sensor.binary_sensor_schema(
            ConnectedBinarySensor,
            device_class=DEVICE_CLASS_CONNECTIVITY,
        ),
    }
)

async def to_code(config):
    hub = await cg.get_variable("focusv_carta_sport_hub")

    if "connected" in config:
        bsens = await binary_sensor.new_binary_sensor(config["connected"])
        cg.add(hub.set_connected_sensor(bsens))
