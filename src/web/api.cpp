#include "api.h"
#include "../security/auth.h"
#include "../hardware/actuators.h"
#include "../core/system.h"
#include "../network/ntp.h"
#include "../core/logging.h"

// ================= DEKLARACJA ZEWNĘTRZNEGO SERWERA =================
extern AsyncWebServer server;

// ================= IMPLEMENTACJE =================

/**
 * Konfiguracja tras API
 */
void setupAPIRoutes() {
    // ================= API - LED =================
    server.on("/api/led/status", HTTP_GET, handleLEDStatus);
    server.on("/api/led/toggle", HTTP_POST, handleLEDToggle);
    
    // ================= API - SYSTEM =================
    server.on("/api/system/info", HTTP_GET, handleSystemInfo);
    
    // *** ROZSZERZENIE: Dodaj tutaj nowe endpointy API dla różnych urządzeń ***
    
    // ============= API - PRZEKAŹNIKI =============
    /*
    server.on("/api/relay/1/toggle", HTTP_POST, handleRelay1Toggle);
    server.on("/api/relay/2/toggle", HTTP_POST, handleRelay2Toggle);
    server.on("/api/relay/3/toggle", HTTP_POST, handleRelay3Toggle);
    */
    
    // ============= API - SERWOMECHANIZMY =============
    /*
    server.on("/api/servo/1/set", HTTP_POST, handleServo1Set);
    server.on("/api/servo/2/set", HTTP_POST, handleServo2Set);
    */
    
    // ============= API - PWM/DIMMER =============
    /*
    server.on("/api/ledstrip/set", HTTP_POST, handleLEDStripSet);
    server.on("/api/fan/set", HTTP_POST, handleFanSet);
    */
    
    // ============= API - ODCZYT SENSORÓW =============
    /*
    server.on("/api/sensors/read", HTTP_GET, handleSensorsRead);
    */
    
    // ============= API - HARMONOGRAMY =============
    /*
    server.on("/api/schedules/list", HTTP_GET, handleSchedulesList);
    server.on("/api/schedules/add", HTTP_POST, handleScheduleAdd);
    server.on("/api/schedules/remove", HTTP_POST, handleScheduleRemove);
    */
}

// ================= LED API =================

/**
 * API - Stan LED
 */
void handleLEDStatus(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    String json = "{\"state\":" + String(getLEDState() ? "true" : "false") + "}";
    request->send(200, "application/json", json);
}

/**
 * API - Przełączenie LED
 */
void handleLEDToggle(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Toggle stan LED
    setLEDState(!getLEDState());
    
    String json = "{\"state\":" + String(getLEDState() ? "true" : "false") + "}";
    request->send(200, "application/json", json);
}

// ================= SYSTEM API =================

/**
 * API - Informacje o systemie
 */
void handleSystemInfo(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    SystemInfo sysInfo = getSystemInfo();
    TimeInfo timeInfo = getCurrentTime();
    
    String json = "{";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"uptime\":\"" + String(sysInfo.uptime) + "\",";
    json += "\"freeHeap\":" + String(sysInfo.freeHeap) + ",";
    json += "\"rssi\":" + String(sysInfo.wifiRSSI) + ",";
    json += "\"currentTime\":\"" + timeInfo.localTime + "\",";
    json += "\"timezone\":\"" + timeInfo.timezone + "\",";
    json += "\"ntpSynced\":" + String(timeInfo.synced ? "true" : "false");
    json += "}";
    
    request->send(200, "application/json", json);
}

// *** ROZSZERZENIE: Implementacje dla nowych urządzeń ***

// ============= API - PRZEKAŹNIKI =============
/*
void handleRelay1Toggle(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    setRelayState(1, !getRelayState(1));
    String json = "{\"state\":" + String(getRelayState(1) ? "true" : "false") + "}";
    request->send(200, "application/json", json);
}

void handleRelay2Toggle(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    setRelayState(2, !getRelayState(2));
    String json = "{\"state\":" + String(getRelayState(2) ? "true" : "false") + "}";
    request->send(200, "application/json", json);
}

void handleRelay3Toggle(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    setRelayState(3, !getRelayState(3));
    String json = "{\"state\":" + String(getRelayState(3) ? "true" : "false") + "}";
    request->send(200, "application/json", json);
}
*/

// ============= API - SERWOMECHANIZMY =============
/*
void handleServo1Set(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    if (request->hasParam("position", true)) {
        int position = request->getParam("position", true)->value().toInt();
        setServoPosition(1, position);
    }
    
    String json = "{\"position\":" + String(getServoPosition(1)) + "}";
    request->send(200, "application/json", json);
}

void handleServo2Set(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    if (request->hasParam("position", true)) {
        int position = request->getParam("position", true)->value().toInt();
        setServoPosition(2, position);
    }
    
    String json = "{\"position\":" + String(getServoPosition(2)) + "}";
    request->send(200, "application/json", json);
}
*/

// ============= API - PWM/DIMMER =============
/*
void handleLEDStripSet(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    if (request->hasParam("brightness", true)) {
        int brightness = request->getParam("brightness", true)->value().toInt();
        setLEDStripBrightness(brightness);
    }
    
    String json = "{\"brightness\":" + String(getLEDStripBrightness()) + "}";
    request->send(200, "application/json", json);
}

void handleFanSet(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    if (request->hasParam("speed", true)) {
        int speed = request->getParam("speed", true)->value().toInt();
        setFanSpeed(speed);
    }
    
    String json = "{\"speed\":" + String(getFanSpeed()) + "}";
    request->send(200, "application/json", json);
}
*/

// ============= API - ODCZYT SENSORÓW =============
/*
void handleSensorsRead(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    readAllSensors();
    
    String json = "{";
    json += "\"temperature\":" + String(temperature, 1) + ",";
    json += "\"lightLevel\":" + String(lightLevel) + ",";
    json += "\"moistureLevel\":" + String(moistureLevel) + ",";
    json += "\"motionDetected\":" + String(motionDetected ? "true" : "false") + ",";
    json += "\"doorOpen\":" + String(doorOpen ? "true" : "false");
    json += "}";
    
    request->send(200, "application/json", json);
}
*/

// ============= API - HARMONOGRAMY =============
/*
void handleSchedulesList(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    String json = "[";
    for (int i = 0; i < 10; i++) {
        if (schedules[i].enabled) {
            if (json != "[") json += ",";
            json += "{";
            json += "\"id\":" + String(i) + ",";
            json += "\"hour\":" + String(schedules[i].hour) + ",";
            json += "\"minute\":" + String(schedules[i].minute) + ",";
            json += "\"action\":\"" + schedules[i].action + "\"";
            json += "}";
        }
    }
    json += "]";
    
    request->send(200, "application/json", json);
}

void handleScheduleAdd(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Implementacja dodawania harmonogramu
    // Pobierz parametry: hour, minute, action
    
    String json = "{\"success\":true,\"message\":\"Harmonogram dodany\"}";
    request->send(200, "application/json", json);
}

void handleScheduleRemove(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Implementacja usuwania harmonogramu
    // Pobierz parametr: id
    
    String json = "{\"success\":true,\"message\":\"Harmonogram usunięty\"}";
    request->send(200, "application/json", json);
}
*/

// ================= FUNKCJE POMOCNICZE =================

/**
 * Utwórz odpowiedź JSON
 */
String createJSONResponse(const String& data) {
    return "{\"data\":" + data + "}";
}

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