#include "vps_logger.h"
#include "../hardware/water_sensor.h"
#include "../hardware/rtc_ds3231.h"
#include "../core/logging.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <mbedtls/md.h>
#include <WiFi.h>

const char* VPS_URL = "http://195.117.36.43:5000/api/water-events";
const char* HMAC_SECRET = "WaterSystem2024_SecretKey_ESP32C6";
const char* USER_AGENT = "ESP32-WaterSystem/1.0";

void initializeVPSLogger() {
    LOG_INFO("VPS Logger initialized for %s", VPS_URL);
}

String calculateHMAC(const String& message) {
    byte hmacResult[32];
    
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
    
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char*)HMAC_SECRET, strlen(HMAC_SECRET));
    mbedtls_md_hmac_update(&ctx, (const unsigned char*)message.c_str(), message.length());
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);
    
    String hmacString = "";
    for (int i = 0; i < 32; i++) {
        char str[3];
        sprintf(str, "%02x", (int)hmacResult[i]);
        hmacString += str;
    }
    
    return hmacString;
}

bool logToVPS(const String& event_type, int volume_ml, const String& timestamp) {
    if (WiFi.status() != WL_CONNECTED) {
        LOG_WARNING("WiFi not connected, cannot log to VPS");
        return false;
    }
    
    HTTPClient http;
    http.begin(VPS_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", USER_AGENT);
    http.setTimeout(15000); // 15 second timeout
    
    // Optional Basic Auth (additional security layer)
    // http.setAuthorization("esp32_water", "VPS_PASSWORD_2024");
    
    // Create event data JSON
    JsonDocument eventData;
    eventData["czas"] = timestamp;
    eventData["czujnik_poziomu"] = getCombinedWaterStatus();
    eventData["akcja"] = event_type; // "AUTO_PUMP", "MANUAL_NORMAL", "MANUAL_1MIN"
    eventData["dolano"] = volume_ml;
    eventData["kontrola"] = "OK";
    
    String eventDataStr;
    serializeJson(eventData, eventDataStr);
    
    // Create message for HMAC
    unsigned long unix_timestamp = getUnixTimestamp();
    String message = String(HMAC_SECRET) + String(unix_timestamp) + eventDataStr;
    String hmac = calculateHMAC(message);
    
    // Final payload
    JsonDocument payload;
    payload["timestamp"] = unix_timestamp;
    payload["data"] = eventData;
    payload["hmac"] = hmac;
    
    String payloadStr;
    serializeJson(payload, payloadStr);
    
    LOG_DEBUG("VPS payload: %s", payloadStr.c_str());
    
    // Send request
    int httpResponseCode = http.POST(payloadStr);
    
    if (httpResponseCode == 200) {
        LOG_INFO("VPS-LOG-SUCCESS: Event logged - %s", event_type.c_str());
        http.end();
        return true;
    } else {
        LOG_ERROR("VPS-LOG-ERROR: HTTP %d - %s", httpResponseCode, http.getString().c_str());
        http.end();
        return false;
    }
}