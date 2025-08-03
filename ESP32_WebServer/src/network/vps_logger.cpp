#include "vps_logger.h"
#include "../config/config.h"
#include "../hardware/water_sensors.h"
#include "../network/wifi_manager.h"
#include "../core/logging.h"
#include "../hardware/rtc_controller.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>




void initVPSLogger() {
    LOG_INFO("VPS Logger initialized - endpoint: %s", VPS_URL);
}

bool logEventToVPS(const String& eventType, uint16_t volumeML, const String& timestamp) {

    // Skip logging if pump globally disabled
    if (!pumpGlobalEnabled) {
        return false;
    }

    if (!isWiFiConnected()) {
        LOG_WARNING("WiFi not connected, cannot log to VPS");
        return false;
    }
    
    HTTPClient http;
    http.begin(VPS_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(VPS_AUTH_TOKEN));
    http.setTimeout(10000);
    
    JsonDocument payload;
    payload["device_id"] = DEVICE_ID;
    payload["timestamp"] = timestamp;
    payload["unix_time"] = getUnixTimestamp();
    payload["event_type"] = eventType;
    payload["volume_ml"] = volumeML;
    payload["water_status"] = getWaterStatus();
    payload["system_status"] = "OK";
    
    String jsonString;
    serializeJson(payload, jsonString);
    
    LOG_INFO("Sending to VPS: %s", jsonString.c_str());
    
    int httpCode = http.POST(jsonString);
    
    if (httpCode == 200) {
        LOG_INFO("VPS log successful: %s", eventType.c_str());
        http.end();
        return true;
    } else {
        LOG_ERROR("VPS log failed: HTTP %d", httpCode);
        http.end();
        return false;
    }
}