import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client
from esphome.const import (
    CONF_ID,
    CONF_MAC_ADDRESS,
    CONF_UPDATE_INTERVAL,
)

CODEOWNERS = ["@yourusername"]
DEPENDENCIES = ["ble_client", "esp32"]

carta_sport_ns = cg.esphome_ns.namespace("carta_sport")
CartaSportComponent = carta_sport_ns.class_(
    "CartaSportComponent", ble_client.BLEClientNode, cg.PollingComponent
)

# Simple configuration - all UUIDs are now hard-coded in constants.h
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(CartaSportComponent),
            cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
            cv.Optional(CONF_UPDATE_INTERVAL, default="30s"): cv.update_interval,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("30s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_address(config[CONF_MAC_ADDRESS].as_hex))