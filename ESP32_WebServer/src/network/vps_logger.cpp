#include "vps_logger.h"
#include "../config/config.h"
#include "../hardware/water_sensors.h"
#include "../network/wifi_manager.h"
#include "../core/logging.h"
#include "../hardware/rtc_controller.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../algorithm/algorithm_config.h"




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

bool logCycleToVPS(const PumpCycle& cycle, const String& timestamp) {
    // Skip logging if pump globally disabled
    if (!pumpGlobalEnabled) {
        return false;
    }

    if (!isWiFiConnected()) {
        LOG_WARNING("WiFi not connected, cannot log cycle to VPS");
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
    payload["event_type"] = "AUTO_CYCLE_COMPLETE";
    payload["volume_ml"] = cycle.volume_dose;
    payload["water_status"] = getWaterStatus();
    payload["system_status"] = (cycle.error_code == 0) ? "OK" : "ERROR";
    
    // Pełne dane algorytmiczne - nowe pola dla rozszerzonego VPS
    payload["time_gap_1"] = cycle.time_gap_1;
    payload["time_gap_2"] = cycle.time_gap_2;
    payload["water_trigger_time"] = cycle.water_trigger_time;
    payload["pump_duration"] = cycle.pump_duration;
    payload["pump_attempts"] = cycle.pump_attempts;
    
    // Wyniki oceny (0/1 flags)
    payload["gap1_fail"] = (cycle.sensor_results & PumpCycle::RESULT_GAP1_FAIL) ? 1 : 0;
    payload["gap2_fail"] = (cycle.sensor_results & PumpCycle::RESULT_GAP2_FAIL) ? 1 : 0;
    payload["water_fail"] = (cycle.sensor_results & PumpCycle::RESULT_WATER_FAIL) ? 1 : 0;
    
    // Dodatkowe dane jako string dla backwards compatibility
    String algorithmSummary = "";
    algorithmSummary += "THRESHOLDS(GAP1:" + String(THRESHOLD_1) + "s,";
    algorithmSummary += "GAP2:" + String(THRESHOLD_2) + "s,";
    algorithmSummary += "WATER:" + String(THRESHOLD_WATER) + "s) ";
    algorithmSummary += "RESULTS(" + String(payload["gap1_fail"].as<int>()) + "-";
    algorithmSummary += String(payload["gap2_fail"].as<int>()) + "-";
    algorithmSummary += String(payload["water_fail"].as<int>()) + ")";
    payload["algorithm_data"] = algorithmSummary;
    
    String jsonString;
    serializeJson(payload, jsonString);
    
    LOG_INFO("JSON size: %d bytes", jsonString.length());
    LOG_INFO("Sending cycle to VPS: %s", jsonString.c_str());
    
    int httpCode = http.POST(jsonString);
    
    if (httpCode == 200) {
        LOG_INFO("VPS cycle log successful");
        http.end();
        return true;
    } else {
        LOG_ERROR("VPS cycle log failed: HTTP %d", httpCode);
        http.end();
        return false;
    }
}

// bool logCycleToVPS(const PumpCycle& cycle, const String& timestamp) {
//     // Skip logging if pump globally disabled
//     if (!pumpGlobalEnabled) {
//         return false;
//     }

//     if (!isWiFiConnected()) {
//         LOG_WARNING("WiFi not connected, cannot log cycle to VPS");
//         return false;
//     }
    
//     HTTPClient http;
//     http.begin(VPS_URL);
//     http.addHeader("Content-Type", "application/json");
//     http.addHeader("Authorization", "Bearer " + String(VPS_AUTH_TOKEN));
//     http.setTimeout(10000);
    
//     JsonDocument payload;
//     payload["device_id"] = DEVICE_ID;
//     payload["timestamp"] = timestamp;
//     payload["unix_time"] = getUnixTimestamp();
//     payload["event_type"] = "AUTO_CYCLE_COMPLETE";
    
//     // Dane algorytmiczne
//     payload["cycle_data"]["time_gap_1"] = cycle.time_gap_1;
//     payload["cycle_data"]["time_gap_2"] = cycle.time_gap_2;
//     payload["cycle_data"]["water_trigger_time"] = cycle.water_trigger_time;
//     payload["cycle_data"]["pump_duration"] = cycle.pump_duration;
//     payload["cycle_data"]["pump_attempts"] = cycle.pump_attempts;
//     payload["cycle_data"]["volume_ml"] = cycle.volume_dose;
    
//     // Wyniki oceny (0/1 flags)
//     payload["results"]["gap1_fail"] = (cycle.sensor_results & PumpCycle::RESULT_GAP1_FAIL) ? 1 : 0;
//     payload["results"]["gap2_fail"] = (cycle.sensor_results & PumpCycle::RESULT_GAP2_FAIL) ? 1 : 0;
//     payload["results"]["water_fail"] = (cycle.sensor_results & PumpCycle::RESULT_WATER_FAIL) ? 1 : 0;
    
//     // Status systemu
//     payload["water_status"] = getWaterStatus();
//     payload["error_code"] = cycle.error_code;
//     payload["system_status"] = (cycle.error_code == 0) ? "OK" : "ERROR";
    
//     String jsonString;
//     serializeJson(payload, jsonString);

//     LOG_INFO("JSON size: %d bytes", jsonString.length());
//     LOG_INFO("Sending cycle to VPS: %s", jsonString.c_str());
    
//     int httpCode = http.POST(jsonString);
    
//     if (httpCode == 200) {
//         LOG_INFO("VPS cycle log successful");
//         http.end();
//         return true;
//     } else {
//         LOG_ERROR("VPS cycle log failed: HTTP %d", httpCode);
//         http.end();
//         return false;
//     }
// }