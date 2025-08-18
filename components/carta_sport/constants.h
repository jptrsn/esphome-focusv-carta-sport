#pragma once

namespace esphome {
namespace carta_sport {

// Focus V Carta Sport BLE Service and Characteristic UUIDs
// These were discovered through BLE snoop log analysis

// Primary Carta Sport custom service - contains device-specific functionality
static const char* const CARTA_SPORT_SERVICE_UUID = "1011123e-8535-b5a0-7140-a304-d2495cb7";

// Carta Sport custom characteristics
static const char* const CARTA_SPORT_CHAR1_UUID = "1011123e-8535-b5a0-7140-a304-d2495cb8";  // Likely temperature/sensor data
static const char* const CARTA_SPORT_CHAR2_UUID = "1011123e-8535-b5a0-7140-a304-d2495cb9";  // Additional sensor data

// Secondary custom service - purpose unknown, may contain additional functionality
static const char* const CARTA_SPORT_SECONDARY_SERVICE_UUID = "00010203-0405-0607-0809-0a0b0c0d1912";
static const char* const CARTA_SPORT_SECONDARY_CHAR_UUID = "00010203-0405-0607-0809-0a0b0c0d2b12";

// Standard Bluetooth services and characteristics
static const char* const STANDARD_DEVICE_NAME_CHAR_UUID = "00002a00-0000-1000-8000-00805f9b34fb";  // Device Name
static const char* const STANDARD_BATTERY_CHAR_UUID = "00002a19-0000-1000-8000-00805f9b34fb";     // Battery Level

// Standard service UUIDs (16-bit, will be converted to 128-bit)
static const uint16_t GAP_SERVICE_UUID = 0x1800;     // Generic Access Profile
static const uint16_t BATTERY_SERVICE_UUID = 0x180F; // Battery Service

// BLE handle ranges from discovery logs
static const uint16_t GAP_SERVICE_HANDLE_START = 0x0001;
static const uint16_t GAP_SERVICE_HANDLE_END = 0x0007;
static const uint16_t CARTA_SERVICE_HANDLE_START = 0x0019;
static const uint16_t CARTA_SERVICE_HANDLE_END = 0x0020;
static const uint16_t SECONDARY_SERVICE_HANDLE_START = 0x0021;
static const uint16_t SECONDARY_SERVICE_HANDLE_END = 0x0024;
static const uint16_t BATTERY_SERVICE_HANDLE = 0x0027;

// Default configuration values
static const uint32_t DEFAULT_UPDATE_INTERVAL_MS = 30000;  // 30 seconds
static const int DEFAULT_RSSI_THRESHOLD = -80;            // dBm

// Data parsing constants (may need adjustment based on actual data format)
static const float TEMPERATURE_SCALE_FACTOR = 100.0f;     // Divide raw value by this
static const uint8_t BATTERY_MAX_VALUE = 100;             // Maximum battery percentage

}  // namespace carta_sport
}  // namespace esphome