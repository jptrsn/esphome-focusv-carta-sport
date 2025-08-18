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

# Carta Sport service UUID and characteristics from BLE discovery
CONF_DEVICE_NAME_CHARACTERISTIC = "device_name_characteristic"
CONF_TEMPERATURE_CHARACTERISTIC = "temperature_characteristic"
CONF_BATTERY_CHARACTERISTIC = "battery_characteristic"
CONF_CARTA_SERVICE_UUID = "carta_service_uuid"
CONF_SECONDARY_SERVICE_UUID = "secondary_service_uuid"

# Primary Carta Sport custom service (likely contains temperature and device control)
DEFAULT_CARTA_SERVICE_UUID = "1011123e-8535-b5a0-7140-a304-d2495cb7"
DEFAULT_CARTA_CHAR1_UUID = "1011123e-8535-b5a0-7140-a304-d2495cb8"  # First custom characteristic
DEFAULT_CARTA_CHAR2_UUID = "1011123e-8535-b5a0-7140-a304-d2495cb9"  # Second custom characteristic

# Secondary custom service
DEFAULT_SECONDARY_SERVICE_UUID = "00010203-0405-0607-0809-0a0b0c0d1912"
DEFAULT_SECONDARY_CHAR_UUID = "00010203-0405-0607-0809-0a0b0c0d2b12"

# Standard services
DEFAULT_DEVICE_NAME_CHAR = "00002a00-0000-1000-8000-00805f9b34fb"  # Standard Device Name
DEFAULT_BATTERY_CHAR = "00002a19-0000-1000-8000-00805f9b34fb"  # Standard Battery Level

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(CartaSportComponent),
            cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
            cv.Optional(CONF_UPDATE_INTERVAL, default="30s"): cv.update_interval,
            cv.Optional(
                CONF_CARTA_SERVICE_UUID, default=DEFAULT_CARTA_SERVICE_UUID
            ): cv.string,
            cv.Optional(
                CONF_SECONDARY_SERVICE_UUID, default=DEFAULT_SECONDARY_SERVICE_UUID
            ): cv.string,
            cv.Optional(
                CONF_DEVICE_NAME_CHARACTERISTIC, default=DEFAULT_DEVICE_NAME_CHAR
            ): cv.string,
            cv.Optional(
                CONF_TEMPERATURE_CHARACTERISTIC, default=DEFAULT_CARTA_CHAR1_UUID
            ): cv.string,
            cv.Optional(
                CONF_BATTERY_CHARACTERISTIC, default=DEFAULT_BATTERY_CHAR
            ): cv.string,
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
    cg.add(var.set_carta_service_uuid(config[CONF_CARTA_SERVICE_UUID]))
    cg.add(var.set_secondary_service_uuid(config[CONF_SECONDARY_SERVICE_UUID]))
    cg.add(var.set_device_name_char_uuid(config[CONF_DEVICE_NAME_CHARACTERISTIC]))
    cg.add(var.set_temperature_char_uuid(config[CONF_TEMPERATURE_CHARACTERISTIC]))
    cg.add(var.set_battery_char_uuid(config[CONF_BATTERY_CHARACTERISTIC]))