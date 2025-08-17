import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MAC_ADDRESS

DEPENDENCIES = ["esp32", "esp32_ble_tracker"]
CODEOWNERS = ["@yourusername"]

AUTO_LOAD = []

# Define the namespace for the carta_sport component
carta_sport_ns = cg.esphome_ns.namespace("carta_sport")

# Define the main component class
CartaSportDiscovery = carta_sport_ns.class_("CartaSportDiscovery", cg.Component)

CONF_DEVICE_NAME = "device_name"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CartaSportDiscovery),
    cv.Optional(CONF_MAC_ADDRESS): cv.mac_address,
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

    # expose the connected flag for templates
    cg.add_define("CARTA_SPORT_HAVE_CONNECTED", True)
    # In YAML you can now use: {{ carta_sport.is_connected }} as a template