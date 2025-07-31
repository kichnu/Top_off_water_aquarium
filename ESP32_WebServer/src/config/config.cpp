#include "config.h"


const char* WIFI_SSID = "KiG";
const char* WIFI_PASSWORD = "3YqvXx5s3Z";
const IPAddress STATIC_IP(192, 168, 0, 164);
const IPAddress GATEWAY(192, 168, 0, 1);
const IPAddress SUBNET(255, 255, 255, 0);

const char* ADMIN_PASSWORD_HASH = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8";

const IPAddress ALLOWED_IPS[] = {
    IPAddress(192, 168, 0, 124),
    IPAddress(192, 168, 1, 102),
    IPAddress(192, 168, 1, 103),
    IPAddress(192, 168, 1, 1)
};
const int ALLOWED_IPS_COUNT = sizeof(ALLOWED_IPS) / sizeof(ALLOWED_IPS[0]);

const char* VPS_URL = "http://195.117.36.43:5000/api/water-events";
const char* VPS_AUTH_TOKEN = "sha256:7b4f8a9c2e6d5a1b8f7e4c9a6d3b2f8e5c1a7b4f9e6d3c8a5b2f7e4c9a6d1b8f";;
// const char* VPS_AUTH_TOKEN = "WaterSystem2024_StaticToken_ESP32C3";

const char* DEVICE_ID = "ESP32C3_WaterPump_001";

PumpSettings currentPumpSettings;