#include "uart_protocol.h"
#include "../logic/water_system.h"
#include "../config/settings.h"
#include "../hardware/sd_logger.h"
#include "../hardware/rtc_ds3231.h"
#include "../config.h"
#include <ArduinoJson.h>
#include <HardwareSerial.h>

// ================= ZMIENNE GLOBALNE =================
String uartBuffer = "";
unsigned long lastHeartbeat = 0;
bool webServerConnected = false;
String lastUARTError = "";

// ================= CRC16 LOOKUP TABLE (zgodna z WebServerem) =================
const uint16_t crc16_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja komunikacji UART dla Pico2
 */
void initializeUARTProtocol() {
    // Konfiguruj UART1 dla Pico2
    Serial1.setTX(UART_TX_PIN);
    Serial1.setRX(UART_RX_PIN);
    Serial1.begin(UART_BAUD_RATE);
    Serial1.setTimeout(100);
    
    uartBuffer = "";
    lastHeartbeat = 0;
    webServerConnected = false;
    lastUARTError = "";
    
    Serial.println("[IOT-UART-INIT] Inicjalizacja protokołu UART dla Pico2");
    Serial.printf("[IOT-UART-CONFIG] RX Pin: GP%d, TX Pin: GP%d, Baud: %d\n", 
                  UART_RX_PIN, UART_TX_PIN, UART_BAUD_RATE);
    Serial.println("[IOT-UART-PROTOCOL] Protokół JSON + CRC16 (kompatybilny z WebServerem)");
    Serial.println();
}

/**
 * Oblicz CRC16 (identyczna z WebServerem)
 */
uint16_t calculateCRC16(const String& data) {
    uint16_t crc = 0xFFFF;
    
    for (int i = 0; i < data.length(); i++) {
        uint8_t tbl_idx = ((crc >> 8) ^ data[i]) & 0xFF;
        crc = ((crc << 8) ^ crc16_table[tbl_idx]) & 0xFFFF;
    }
    
    return crc;
}

/**
 * Walidacja CRC
 */
bool validateCRC(const String& jsonStr, const String& expectedCrc) {
    int crcPos = jsonStr.indexOf("\"crc\":");
    if (crcPos == -1) return false;
    
    String dataForCrc = jsonStr.substring(0, crcPos);
    dataForCrc += "}";
    
    uint16_t calculatedCrc = calculateCRC16(dataForCrc);
    String calculatedCrcStr = String(calculatedCrc, HEX);
    calculatedCrcStr.toUpperCase();
    
    return (calculatedCrcStr == expectedCrc);
}

/**
 * Utwórz odpowiedź JSON z CRC
 */
String createResponseJSON(uint32_t commandId, const String& status, const String& result) {
    JsonDocument doc;
    doc["id"] = commandId;
    doc["type"] = "response";
    doc["status"] = status;
    
    if (result.length() > 0) {
        JsonDocument resultDoc;
        DeserializationError error = deserializeJson(resultDoc, result);
        if (error) {
            doc["result"] = result;  // Jeśli nie JSON, zapisz jako string
        } else {
            doc["result"] = resultDoc;
        }
    }
    
    // Generuj JSON bez CRC
    String jsonWithoutCrc;
    serializeJson(doc, jsonWithoutCrc);
    
    // Oblicz CRC
    uint16_t crc = calculateCRC16(jsonWithoutCrc);
    String crcStr = String(crc, HEX);
    crcStr.toUpperCase();
    
    doc["crc"] = crcStr;
    
    // Final JSON
    String finalJson;
    serializeJson(doc, finalJson);
    
    return finalJson;
}

/**
 * Wyślij odpowiedź przez UART
 */
void sendUARTResponse(uint32_t commandId, const String& status, const String& result) {
    String response = createResponseJSON(commandId, status, result);
    
    Serial1.println(response);
    Serial1.flush();
    
    Serial.println("[IOT-UART-TX] Wysłano odpowiedź do WebServera");
    Serial.println(response);
    Serial.println();
}

/**
 * Wyślij pong (odpowiedź na ping heartbeat)
 */
void sendPong() {
    JsonDocument doc;
    doc["id"] = millis();
    doc["type"] = "pong";
    doc["timestamp"] = getCurrentTimeString();
    doc["system"] = SYSTEM_NAME;
    
    // Bez CRC dla pong (uproszczenie)
    String pongJson;
    serializeJson(doc, pongJson);
    
    Serial1.println(pongJson);
    Serial1.flush();
    
    Serial.println("[IOT-UART-PONG] Wysłano pong do WebServera");
    Serial.println(pongJson);
    Serial.println();
}

/**
 * Przetwarzaj dane UART
 */
void processUARTProtocol() {
    // Czytaj dane z UART
    while (Serial1.available()) {
        char c = Serial1.read();
        
        if (c == '\n' || c == '\r') {
            if (uartBuffer.length() > 0) {
                
                Serial.println("[WEBSERVER-UART-RX] Otrzymano komendę z WebServera");
                Serial.println(uartBuffer);
                Serial.println();
                
                // Przetwórz komendę
                processCommand(uartBuffer);
                
                uartBuffer = "";
            }
        } else {
            uartBuffer += c;
            
            // Zabezpieczenie przed przepełnieniem bufora
            if (uartBuffer.length() > 512) {
                Serial.println("[IOT-UART-ERROR] Przepełnienie bufora UART");
                uartBuffer = "";
                lastUARTError = "Buffer overflow";
            }
        }
    }
    
    // Sprawdź heartbeat
    #if ENABLE_HEARTBEAT
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL * 2) {
        webServerConnected = false;
    }
    #endif
}

/**
 * Przetwórz pojedynczą komendę JSON
 */
void processCommand(const String& jsonCommand) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonCommand);
    
    if (error) {
        lastUARTError = "JSON parse error: " + String(error.c_str());
        Serial.println("[IOT-UART-ERROR] Błąd parsowania JSON: " + lastUARTError);
        return;
    }
    
    // Sprawdź czy mamy wszystkie wymagane pola
    if (!doc["id"].is<uint32_t>() || !doc["type"].is<String>()) {
        lastUARTError = "Missing required fields";
        Serial.println("[IOT-UART-ERROR] Brakuje wymaganych pól w komendzie");
        return;
    }
    
    uint32_t commandId = doc["id"].as<uint32_t>();
    String type = doc["type"].as<String>();
    
    // Walidacja CRC (jeśli jest)
    if (doc["crc"].is<String>()) {
        String expectedCrc = doc["crc"].as<String>();
        if (!validateCRC(jsonCommand, expectedCrc)) {
            lastUARTError = "CRC validation failed";
            Serial.println("[IOT-UART-ERROR] Błędna suma kontrolna CRC");
            sendUARTResponse(commandId, "crc_error", "");
            return;
        }
    }
    
    // Przetwórz różne typy komend
    if (type == "command") {
        processCommandType(commandId, doc);
    } else if (type == "ping") {
        lastHeartbeat = millis();
        webServerConnected = true;
        sendPong();
    } else {
        Serial.println("[IOT-UART-WARNING] Nieznany typ komendy: " + type);
        sendUARTResponse(commandId, "unknown_type", "");
    }
}

/**
 * Przetwórz komendę typu "command"
 */
void processCommandType(uint32_t commandId, JsonDocument& doc) {
    if (!doc["action"].is<String>()) {
        sendUARTResponse(commandId, "missing_action", "");
        return;
    }
    
    String action = doc["action"].as<String>();
    
    Serial.println("[IOT-UART-COMMAND] Przetwarzanie komendy: " + action);
    
    // ================= OBSŁUGA RÓŻNYCH KOMEND =================
    
    if (action == "manual_pump") {
        handleManualPump(commandId, doc);
        
    } else if (action == "set_pump_config") {
        handleSetPumpConfig(commandId, doc);
        
    } else if (action == "get_water_status") {
        handleGetWaterStatus(commandId, doc);
        
    } else if (action == "get_recent_logs") {
        handleGetRecentLogs(commandId, doc);
        
    } else if (action == "set_led") {
        // Kompatybilność z oryginalnym WebServerem (ignoruj)
        sendUARTResponse(commandId, "ok", "{\"led_state\":false}");
        
    } else if (action == "get_status") {
        // Ogólny status systemu
        handleGetSystemStatus(commandId, doc);
        
    } else {
        Serial.println("[IOT-UART-ERROR] Nieznana akcja: " + action);
        sendUARTResponse(commandId, "unknown_action", "");
    }
}

/**
 * Obsłuż ręczne uruchomienie pompy
 */
void handleManualPump(uint32_t commandId, JsonDocument& doc) {
    Serial.println("[IOT-COMMAND-MANUAL-PUMP] Wykonywanie ręcznego uruchomienia pompy");
    
    bool success = triggerManualPump();
    
    if (success) {
        PumpConfig config = getPumpConfig();
        String result = "{";
        result += "\"success\":true,";
        result += "\"volume_ml\":" + String(config.volumeML) + ",";
        result += "\"duration_s\":" + String(config.pumpTimeSeconds) + ",";
        result += "\"timestamp\":\"" + getCurrentTimeString() + "\"";
        result += "}";
        
        sendUARTResponse(commandId, "ok", result);
    } else {
        sendUARTResponse(commandId, "pump_error", "");
    }
}

/**
 * Obsłuż konfigurację pompy
 */
void handleSetPumpConfig(uint32_t commandId, JsonDocument& doc) {
    if (!doc["params"].is<JsonObject>()) {
        sendUARTResponse(commandId, "missing_params", "");
        return;
    }
    
    JsonObject params = doc["params"];
    
    if (!params["volume_ml"].is<int>() || !params["time_s"].is<int>()) {
        sendUARTResponse(commandId, "invalid_params", "");
        return;
    }
    
    int volumeML = params["volume_ml"];
    int timeS = params["time_s"];
    
    Serial.printf("[IOT-COMMAND-SET-CONFIG] Ustawianie konfiguracji: %dml, %ds\n", volumeML, timeS);
    
    bool success = setPumpConfig(volumeML, timeS);
    
    if (success) {
        String result = "{";
        result += "\"volume_ml\":" + String(volumeML) + ",";
        result += "\"time_s\":" + String(timeS) + "";
        result += "}";
        
        sendUARTResponse(commandId, "ok", result);
    } else {
        sendUARTResponse(commandId, "invalid_config", "");
    }
}

/**
 * Obsłuż pobieranie statusu wody
 */
void handleGetWaterStatus(uint32_t commandId, JsonDocument& doc) {
    WaterSystemStatus status = getWaterSystemStatus();
    PumpConfig config = getPumpConfig();
    
    String result = "{";
    result += "\"water_level\":\"" + String(status.waterLevel == WATER_LEVEL_LOW ? "LOW" : "OK") + "\",";
    result += "\"pump_active\":" + String(status.pumpActive ? "true" : "false") + ",";
    result += "\"last_pump_time\":\"" + status.lastPumpTime + "\",";
    result += "\"events_today\":" + String(status.eventsToday) + ",";
    result += "\"total_events\":" + String(status.totalEvents) + ",";
    result += "\"volume_ml\":" + String(config.volumeML) + ",";
    result += "\"pump_time_s\":" + String(config.pumpTimeSeconds) + ",";
    result += "\"system_ready\":" + String(status.systemReady ? "true" : "false");
    result += "}";
    
    sendUARTResponse(commandId, "ok", result);
}

/**
 * Obsłuż pobieranie ostatnich logów
 */
void handleGetRecentLogs(uint32_t commandId, JsonDocument& doc) {
    int count = 10;  // Domyślnie 10 ostatnich logów
    
    if (doc["params"].is<JsonObject>() && doc["params"]["count"].is<int>()) {
        count = doc["params"]["count"];
        count = constrain(count, 1, 50);  // Limit 1-50
    }
    
    String logsJson = getRecentEvents(count);
    sendUARTResponse(commandId, "ok", logsJson);
}

/**
 * Obsłuż pobieranie ogólnego statusu systemu
 */
void handleGetSystemStatus(uint32_t commandId, JsonDocument& doc) {
    String result = "{";
    result += "\"system\":\"" + String(SYSTEM_NAME) + "\",";
    result += "\"version\":\"" + String(VERSION) + "\",";
    result += "\"rtc_time\":\"" + getCurrentTimeString() + "\",";
    result += "\"rtc_working\":" + String(isRTCWorking() ? "true" : "false") + ",";
    result += "\"sd_working\":" + String(isSDWorking() ? "true" : "false") + ",";
    result += "\"uptime_ms\":" + String(millis());
    result += "}";
    
    sendUARTResponse(commandId, "ok", result);
}

/**
 * Sprawdź połączenie z WebServerem
 */
bool isWebServerConnected() {
    return webServerConnected;
}

/**
 * Pobierz ostatni błąd UART
 */
String getLastUARTError() {
    return lastUARTError;
}

/**
 * Debug protokołu UART
 */
void debugUARTProtocol() {
    Serial.println("[IOT-UART-DEBUG] === STATUS PROTOKOŁU UART ===");
    Serial.printf("[IOT-UART-DEBUG] WebServer połączony: %s\n", webServerConnected ? "TAK" : "NIE");
    Serial.printf("[IOT-UART-DEBUG] Ostatni heartbeat: %lu ms temu\n", millis() - lastHeartbeat);
    Serial.printf("[IOT-UART-DEBUG] Ostatni błąd: %s\n", lastUARTError.c_str());
    Serial.printf("[IOT-UART-DEBUG] Bufor UART: %d znaków\n", uartBuffer.length());
    Serial.println("[IOT-UART-DEBUG] ==============================");
    Serial.println();
}