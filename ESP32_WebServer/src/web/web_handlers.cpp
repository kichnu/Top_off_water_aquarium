#include "web_handlers.h"
#include "web_server.h"
#include "html_pages.h"
#include "../security/auth_manager.h"
#include "../security/session_manager.h"
#include "../security/rate_limiter.h"
#include "../hardware/pump_controller.h"
#include "../hardware/water_sensors.h"
#include "../hardware/rtc_controller.h"
#include "../network/wifi_manager.h"
#include "../config/config.h"
#include "../core/logging.h"
#include <ArduinoJson.h>


void handleDashboard(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->redirect("/login");
        return;
    }
    request->send(200, "text/html", getDashboardHTML());
}

void handleLoginPage(AsyncWebServerRequest* request) {
    request->send(200, "text/html", getLoginHTML());
}

void handleLogin(AsyncWebServerRequest* request) {
    IPAddress clientIP = request->client()->remoteIP();
    
    if (isRateLimited(clientIP) || isIPBlocked(clientIP)) {
        request->send(429, "text/plain", "Too Many Requests");
        return;
    }
    
    if (!request->hasParam("password", true)) {
        recordFailedAttempt(clientIP);
        request->send(400, "application/json", "{\"success\":false,\"error\":\"Missing password\"}");
        return;
    }
    
    String password = request->getParam("password", true)->value();
    
    if (verifyPassword(password)) {
        String token = createSession(clientIP);
        String cookie = "session_token=" + token + "; Path=/; HttpOnly; Max-Age=300";
        
        AsyncWebServerResponse* response = request->beginResponse(200, "application/json", "{\"success\":true}");
        response->addHeader("Set-Cookie", cookie);
        request->send(response);
    } else {
        recordFailedAttempt(clientIP);
        request->send(401, "application/json", "{\"success\":false,\"error\":\"Invalid password\"}");
    }
}

void handleLogout(AsyncWebServerRequest* request) {
    if (request->hasHeader("Cookie")) {
        String cookie = request->getHeader("Cookie")->value();
        int tokenStart = cookie.indexOf("session_token=");
        if (tokenStart != -1) {
            tokenStart += 14;
            int tokenEnd = cookie.indexOf(";", tokenStart);
            if (tokenEnd == -1) tokenEnd = cookie.length();
            
            String token = cookie.substring(tokenStart, tokenEnd);
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
    json["water_status"] = getWaterStatus();
    json["pump_running"] = isPumpActive();
    json["pump_remaining"] = getPumpRemainingTime();
    json["wifi_status"] = getWiFiStatus();
    json["wifi_connected"] = isWiFiConnected();
    json["rtc_time"] = getCurrentTimestamp();
    json["rtc_working"] = isRTCWorking();
    json["rtc_info"] = getRTCInfo(); // Added RTC type information
    json["free_heap"] = ESP.getFreeHeap();
    json["uptime"] = millis();
    json["device_id"] = DEVICE_ID;
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
}

void handlePumpNormal(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    bool success = triggerPump(currentPumpSettings.normalCycleSeconds, "MANUAL_NORMAL");
    
    JsonDocument json;
    json["success"] = success;
    json["duration"] = currentPumpSettings.normalCycleSeconds;
    json["volume_ml"] = currentPumpSettings.normalCycleSeconds * currentPumpSettings.volumePerSecond;
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
    
    LOG_INFO("Manual normal pump triggered via web");
}

void handlePumpExtended(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    bool success = triggerPump(currentPumpSettings.extendedCycleSeconds, "MANUAL_EXTENDED");
    
    JsonDocument json;
    json["success"] = success;
    json["duration"] = currentPumpSettings.extendedCycleSeconds;
    json["type"] = "extended";
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
    
    LOG_INFO("Manual extended pump triggered via web");
}

void handlePumpStop(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    stopPump();
    
    JsonDocument json;
    json["success"] = true;
    json["message"] = "Pump stopped";
    
    String response;
    serializeJson(json, response);
    request->send(200, "application/json", response);
    
    LOG_INFO("Pump manually stopped via web");
}