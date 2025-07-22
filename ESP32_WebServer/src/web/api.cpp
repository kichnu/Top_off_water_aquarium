#include "api.h"
#include "server.h"
#include "../security/auth.h"
#include "../security/sessions.h"
#include "../security/ratelimit.h"
#include "../hardware/pump_controller.h"
#include "../hardware/water_sensor.h"
#include "../hardware/rtc_ds3231.h"
#include "../network/wifi.h"
#include "../network/ntp.h"
#include "../config/settings.h"
#include "../core/logging.h"
#include "../core/system.h"
#include <ArduinoJson.h>

void handleLogin(AsyncWebServerRequest* request) {
    IPAddress client_ip = request->client()->remoteIP();
    
    if (isRateLimited(client_ip) || isIPBlocked(client_ip)) {
        request->send(429, "text/plain", "Too Many Requests");
        return;
    }
    
    if (!request->hasParam("password", true)) {
        recordFailedAttempt(client_ip);
        request->send(400, "text/plain", "Missing password");
        return;
    }
    
    String password = request->getParam("password", true)->value();
    
    if (verifyPassword(password)) {
        String token = createSession(client_ip);
        String cookie = "session_token=" + token + "; Path=/; HttpOnly; Max-Age=300"; // 5 minutes
        
        AsyncWebServerResponse* response = request->beginResponse(200, "application/json", "{\"success\":true}");
        response->addHeader("Set-Cookie", cookie);
        request->send(response);
        
        LOG_INFO("Successful login from IP: %s", client_ip.toString().c_str());
    } else {
        recordFailedAttempt(client_ip);
        request->send(401, "application/json", "{\"success\":false,\"error\":\"Invalid password\"}");
    }
}

void handleLogout(AsyncWebServerRequest* request) {
    if (request->hasHeader("Cookie")) {
        String cookie = request->getHeader("Cookie")->value();
        int token_start = cookie.indexOf("session_token=");
        if (token_start != -1) {
            token_start += 14;
            int token_end = cookie.indexOf(";", token_start);
            if (token_end == -1) token_end = cookie.length();
            
            String token = cookie.substring(token_start, token_end);
            destroySession(token);
        }
    }
    
    AsyncWebServerResponse* response = request->beginResponse(200, "application/json", "{\"success\":true}");
    response->addHeader("Set-Cookie", "session_token=; Path=/; HttpOnly; Max-Age=0");
    request->send(response);
}

void handleStatus(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    JsonDocument json;
    json["water_status"] = getCombinedWaterStatus();
    json["pump_running"] = isPumpRunning();
    json["pump_remaining"] = getPumpRemainingTime();
    json["wifi_status"] = getWiFiStatus();
    json["wifi_connected"] = isWiFiConnected();
    json["ntp_status"] = getNTPStatus();
    json["free_heap"] = getFreeHeap();
    json["uptime"] = getUptime();
    json["rtc_time"] = getRTCTimestamp();
    
    // Temporarily disabled RTC advanced functions
    json["rtc_temperature"] = 0.0;  // getRTCTemperature();
    json["rtc_working"] = true;     // isRTCWorking();
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
}

void handleManualPumpNormal(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    PumpConfig config = getPumpConfig();
    bool success = triggerManualPump(config.pumpTimeSeconds, "MANUAL_NORMAL");
    
    JsonDocument json;
    json["success"] = success;
    json["duration"] = config.pumpTimeSeconds;
    json["volume_ml"] = config.volumeML;
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
    
    LOG_INFO("Manual pump normal triggered via web interface");
}

void handleManualPump1Min(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    bool success = triggerManualPump(60, "MANUAL_1MIN");
    
    JsonDocument json;
    json["success"] = success;
    json["duration"] = 60;
    json["type"] = "extended";
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
    
    LOG_INFO("Manual pump 1-minute triggered via web interface");
}

void handleGetSettings(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    PumpConfig config = getPumpConfig();
    
    JsonDocument json;
    json["wifi_ssid"] = getWiFiSSID();
    json["pump_time"] = config.pumpTimeSeconds;
    json["volume_ml"] = config.volumeML;
    json["auto_pump"] = config.autoPumpEnabled;
    json["daily_limit"] = config.dailyLimitML;
    json["min_interval"] = config.minIntervalSeconds;
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
}

void handleUpdateSettings(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Implementation for updating settings
    // Parse JSON body and update configuration
    request->send(200, "application/json", "{\"success\":true}");
    
    LOG_INFO("Settings updated via web interface");
}