#include "api.h"
#include "../security/auth.h"
#include "../communication/uart.h" // Changed from actuators to uart
#include <WiFi.h> // For WiFi status
#include "time.h" // For time functions

// ================= DEKLARACJA ZEWNĘTRZNEGO SERWERA =================
extern AsyncWebServer server;

// ================= IMPLEMENTACJE =================

/**
 * Konfiguracja tras API
 */
void setupAPIRoutes() {
    // ================= API - LED ONLY =================
    server.on("/api/led/status", HTTP_GET, handleLEDStatus);
    server.on("/api/led/toggle", HTTP_POST, handleLEDToggle);

        // ========== NOWE: WATER SYSTEM API ==========
    server.on("/api/water/status", HTTP_GET, handleWaterStatus);
    server.on("/api/water/pump", HTTP_POST, handleManualPump);
    server.on("/api/water/config", HTTP_POST, handlePumpConfig);
    server.on("/api/water/logs", HTTP_GET, handleWaterLogs);
    
    // ================= API - BASIC STATUS =================
    server.on("/api/status", HTTP_GET, handleBasicStatus);
}

// ================= LED API =================

/**
 * API - Stan LED (z cache WebServer)
 */
void handleLEDStatus(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Return state from cache (may be outdated if IoT doesn't respond)
    bool iotConnected = isIoTConnected();
    String json = "{";
    json += "\"state\":" + String(getLEDState() ? "true" : "false") + ",";
    json += "\"iot_connected\":" + String(iotConnected ? "true" : "false") + ",";
    json += "\"source\":\"" + String(iotConnected ? "iot" : "cache") + "\"";
    json += "}";
    
    Serial.println("[WEBSERVER-API-LED-STATUS] Zwrócono stan LED: " + String(getLEDState() ? "ON" : "OFF"));
    Serial.println();
    
    request->send(200, "application/json", json);
}

// ================= STATUS API =================

/**
 * API - Podstawowe informacje statusu (WiFi, NTP, IoT, Heartbeat)
 */
void handleBasicStatus(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // WiFi info
    String wifiIP = WiFi.localIP().toString();
    bool wifiConnected = WiFi.status() == WL_CONNECTED;
    
    // NTP info  
    bool ntpSynced = false;
    String currentTime = "Nie zsynchronizowany";
    time_t now = time(nullptr);
    if (now >= 1609459200) { // Check if time is after Jan 1 2021
        ntpSynced = true;
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            char timeString[64];
            strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
            currentTime = String(timeString);
        }
    }
    
    // IoT/UART info
    bool iotConnected = isIoTConnected();
    unsigned long lastHB = getLastHeartbeat();
    unsigned long heartbeatAgo = (millis() - lastHB) / 1000; // seconds
    
    String json = "{";
    json += "\"wifi\":{";
    json += "\"connected\":" + String(wifiConnected ? "true" : "false") + ",";
    json += "\"ip\":\"" + wifiIP + "\"";
    json += "},";
    json += "\"ntp\":{";
    json += "\"synced\":" + String(ntpSynced ? "true" : "false") + ",";
    json += "\"time\":\"" + currentTime + "\"";
    json += "},";
    json += "\"iot\":{";
    json += "\"connected\":" + String(iotConnected ? "true" : "false") + ",";
    json += "\"heartbeat_ago\":" + String(heartbeatAgo);
    json += "}";
    json += "}";
    
    Serial.println("[WEBSERVER-API-STATUS] Zwrócono podstawowy status");
    Serial.println();
    
    request->send(200, "application/json", json);
}

/**
 * API - Przełączenie LED (przez UART)
 */
void handleLEDToggle(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Get current state and toggle
    bool currentState = getLEDState();
    bool newState = !currentState;
    
    Serial.println("[WEBSERVER-API-LED-TOGGLE] Żądanie przełączenia LED: " + String(currentState ? "ON" : "OFF") + " -> " + String(newState ? "ON" : "OFF"));
    Serial.println();
    
    // Send command via UART
    setLEDState(newState);
    
    // Check command status
    bool iotConnected = isIoTConnected();
    int pendingCommands = getPendingCommandsCount();
    
    String json = "{";
    json += "\"state\":" + String(newState ? "true" : "false") + ",";
    json += "\"command_sent\":" + String("true") + ",";
    json += "\"iot_connected\":" + String(iotConnected ? "true" : "false") + ",";
    json += "\"pending_commands\":" + String(pendingCommands) + ",";
    json += "\"status\":\"" + String(iotConnected ? "sent" : "queued") + "\"";
    json += "}";
    
    request->send(200, "application/json", json);
}


// API - Status systemu wody
void handleWaterStatus(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Żądaj świeżego statusu z IoT
    requestWaterStatus();
    
    // Zwróć cache (może być nieaktualny jeśli IoT nie odpowiada)
    WaterSystemStatus status = getWaterSystemStatus();
    bool iotConnected = isIoTConnected();
    
    String json = "{";
    json += "\"water_level\":\"" + status.waterLevel + "\",";
    json += "\"pump_active\":" + String(status.pumpActive ? "true" : "false") + ",";
    json += "\"events_today\":" + String(status.eventsToday) + ",";
    json += "\"last_pump_time\":\"" + status.lastPumpTime + "\",";
    json += "\"iot_connected\":" + String(iotConnected ? "true" : "false") + ",";
    json += "\"last_update\":" + String(status.lastUpdate);
    json += "}";
    
    request->send(200, "application/json", json);
}

// API - Ręczne uruchomienie pompy
void handleManualPump(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Wyślij komendę do IoT
    triggerManualPump();
    
    String json = "{";
    json += "\"command_sent\":true,";
    json += "\"iot_connected\":" + String(isIoTConnected() ? "true" : "false");
    json += "}";
    
    request->send(200, "application/json", json);
}

// API - Konfiguracja pompy
void handlePumpConfig(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    if (!request->hasParam("volume", true) || !request->hasParam("time", true)) {
        request->send(400, "application/json", "{\"error\":\"Missing parameters\"}");
        return;
    }
    
    int volumeML = request->getParam("volume", true)->value().toInt();
    int timeS = request->getParam("time", true)->value().toInt();
    
    if (volumeML < 1 || volumeML > 1000 || timeS < 1 || timeS > 120) {
        request->send(400, "application/json", "{\"error\":\"Invalid parameters\"}");
        return;
    }
    
    // Wyślij konfigurację do IoT
    setPumpConfiguration(volumeML, timeS);
    
    String json = "{";
    json += "\"volume_ml\":" + String(volumeML) + ",";
    json += "\"time_s\":" + String(timeS) + ",";
    json += "\"command_sent\":true";
    json += "}";
    
    request->send(200, "application/json", json);
}

// API - Ostatnie logi
void handleWaterLogs(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    int count = 10;
    if (request->hasParam("count")) {
        count = request->getParam("count")->value().toInt();
        count = constrain(count, 1, 50);
    }
    
    // Żądaj logów z IoT
    requestRecentLogs(count);
    
    // Tymczasowa odpowiedź (w przyszłości można cache'ować odpowiedź)
    String json = "{";
    json += "\"logs_requested\":true,";
    json += "\"count\":" + String(count) + ",";
    json += "\"iot_connected\":" + String(isIoTConnected() ? "true" : "false");
    json += "}";
    
    request->send(200, "application/json", json);
}

// ================= FUNKCJE POMOCNICZE =================

/**
 * Wyślij błąd JSON
 */
void sendJSONError(AsyncWebServerRequest* request, const String& error) {
    String json = "{\"error\":\"" + error + "\"}";
    request->send(400, "application/json", json);
}

/**
 * Wyślij sukces JSON
 */
void sendJSONSuccess(AsyncWebServerRequest* request, const String& data) {
    String json = "{\"success\":true,\"data\":" + data + "}";
    request->send(200, "application/json", json);
}

/**
 * Wyślij odpowiedź z informacją o stanie IoT
 */
void sendJSONWithIoTStatus(AsyncWebServerRequest* request, const String& data) {
    bool iotConnected = isIoTConnected();
    String json = "{";
    json += "\"success\":true,";
    json += "\"data\":" + data + ",";
    json += "\"iot_connected\":" + String(iotConnected ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
}