import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, CONF_MAC_ADDRESS

DEPENDENCIES = ["esp32", "esp32_ble_tracker"]

# Define the namespace for the carta_sport component
carta_sport_ns = cg.esphome_ns.namespace("carta_sport")

# Define the main component class
CartaSportDiscovery = carta_sport_ns.class_("CartaSportDiscovery", cg.Component)
CartaSportDeviceNameSensor = carta_sport_ns.class_("CartaSportDeviceNameSensor", text_sensor.TextSensor, cg.Component)

CONF_DEVICE_NAME = "device_name"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CartaSportDiscovery),
    cv.Optional(CONF_MAC_ADDRESS): cv.mac_address,
    cv.Optional(CONF_DEVICE_NAME): text_sensor.text_sensor_schema(CartaSportDeviceNameSensor),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Set MAC address if provided
    if CONF_MAC_ADDRESS in config:
        cg.add(var.set_target_mac_address(config[CONF_MAC_ADDRESS].as_hex))
    else:
        # Set to empty to indicate we should scan for devices
        cg.add(var.set_target_mac_address(""))

    # Set up device name sensor if configured
    if CONF_DEVICE_NAME in config:
        sens = cg.new_Pvariable(config[CONF_DEVICE_NAME][CONF_ID])
        await text_sensor.register_text_sensor(sens, config[CONF_DEVICE_NAME])
        await cg.register_component(sens, config[CONF_DEVICE_NAME])
        cg.add(var.set_device_name_sensor(sens))