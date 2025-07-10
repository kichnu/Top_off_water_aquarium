#include "uart.h"

// ================= ZMIENNE GLOBALNE =================
UARTCommand pendingCommands[MAX_PENDING_COMMANDS];
int pendingCommandsCount = 0;
unsigned long lastHeartbeat = 0;
bool iotConnected = false;
String lastError = "";
String uartBuffer = "";
bool ledState = false; // LED state cache (moved from actuators)

// ================= CRC16 LOOKUP TABLE =================
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
 * Inicjalizacja UART
 */
void initializeUART() {
    Serial1.begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
    Serial1.setTimeout(100);
    
    pendingCommandsCount = 0;
    lastHeartbeat = 0;
    iotConnected = false;
    lastError = "";
    uartBuffer = "";
    ledState = false; // Initialize LED state cache
    
    // Wyczyść pending commands
    for (int i = 0; i < MAX_PENDING_COMMANDS; i++) {
        pendingCommands[i].pending = false;
        pendingCommands[i].retryCount = 0;
    }
    
    // Improved logging with labels and empty lines
    Serial.println("[WEBSERVER-UART-INIT] Inicjalizacja komunikacji UART");
    Serial.printf("[WEBSERVER-UART-CONFIG] RX Pin: %d, TX Pin: %d, Baud: %d\n", UART_RX_PIN, UART_TX_PIN, UART_BAUD_RATE);
    Serial.println();
    
    // LED state initialization
    Serial.println("[WEBSERVER-LED-INIT] Cache stanu LED zainicjalizowany: OFF");
    Serial.println();
}

/**
 * Oblicz CRC16
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
 * Generuj ID komendy
 */
uint32_t generateCommandId() {
    return millis();
}

/**
 * Utwórz JSON komendy
 */
String createCommandJSON(const String& action, const String& params) {
    uint32_t id = generateCommandId();
    
    JsonDocument doc;
    doc["id"] = id;
    doc["type"] = "command";
    doc["action"] = action;
    
    if (params.length() > 0) {
        JsonDocument paramsDoc;
        DeserializationError error = deserializeJson(paramsDoc, params);
        if (error) {
            doc["params"] = params;
        } else {
            doc["params"] = paramsDoc;
        }
    }
    
    String jsonWithoutCrc;
    serializeJson(doc, jsonWithoutCrc);
    
    uint16_t crc = calculateCRC16(jsonWithoutCrc);
    String crcStr = String(crc, HEX);
    crcStr.toUpperCase();
    
    doc["crc"] = crcStr;
    
    String finalJson;
    serializeJson(doc, finalJson);
    
    return finalJson;
}

/**
 * Parsuj odpowiedź JSON
 */
bool parseResponseJSON(const String& jsonStr, UARTResponse& response) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        lastError = "JSON parse error: " + String(error.c_str());
        return false;
    }
    
    if (!doc["id"].is<uint32_t>() || !doc["type"].is<String>() || !doc["crc"].is<String>()) {
        lastError = "Missing required fields in response";
        return false;
    }
    
    response.id = doc["id"].as<uint32_t>();
    response.type = doc["type"].as<String>();
    response.status = doc["status"].as<String>();
    response.crc = doc["crc"].as<String>();
    
    if (!validateCRC(jsonStr, response.crc)) {
        lastError = "CRC validation failed";
        return false;
    }
    
    if (doc["result"].is<JsonObject>() || doc["result"].is<JsonArray>()) {
        serializeJson(doc["result"], response.result);
    }
    if (doc["sensors"].is<JsonObject>() || doc["sensors"].is<JsonArray>()) {
        serializeJson(doc["sensors"], response.sensors);
    }
    
    response.valid = true;
    return true;
}

/**
 * Wyślij komendę
 */
bool sendCommand(const String& action, const String& params) {
    if (pendingCommandsCount >= MAX_PENDING_COMMANDS) {
        lastError = "Command queue full";
        Serial.println("[WEBSERVER-UART-ERROR] Kolejka komend pełna");
        Serial.println();
        return false;
    }
    
    String jsonCommand = createCommandJSON(action, params);
    
    // Zapisz komendę jako pending
    for (int i = 0; i < MAX_PENDING_COMMANDS; i++) {
        if (!pendingCommands[i].pending) {
            uint32_t id = generateCommandId();
            pendingCommands[i].id = id;
            pendingCommands[i].type = "command";
            pendingCommands[i].action = action;
            pendingCommands[i].params = params;
            pendingCommands[i].timestamp = millis();
            pendingCommands[i].retryCount = 0;
            pendingCommands[i].pending = true;
            pendingCommandsCount++;
            break;
        }
    }
    
    // Wyślij przez UART z logowaniem
    Serial1.println(jsonCommand);
    Serial1.flush();
    
    Serial.println("[WEBSERVER-UART-TX] Wysłano JSON do IoT: " + action);
    Serial.println(jsonCommand);
    Serial.println();
    
    return true;
}

/**
 * Wyślij heartbeat
 */
bool sendHeartbeat() {
    String heartbeatJson = createCommandJSON("ping", "{}");
    Serial1.println(heartbeatJson);
    
    Serial.println("[WEBSERVER-UART-HEARTBEAT] Wysłano ping do IoT");
    Serial.println(heartbeatJson);
    Serial.println();
    
    lastHeartbeat = millis();
    return true;
}

/**
 * Przetwarzaj dane UART
 */
void processUARTData() {
    checkCommandTimeouts();
    
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
        sendHeartbeat();
    }
    
    while (Serial1.available()) {
        char c = Serial1.read();
        
        if (c == '\n' || c == '\r') {
            if (uartBuffer.length() > 0) {
                // Log received data
                Serial.println("[IOT-UART-RX] Otrzymano odpowiedź z IoT");
                Serial.println(uartBuffer);
                Serial.println();
                
                if (uartBuffer.startsWith("{") && uartBuffer.endsWith("}")) {
                    UARTResponse response;
                    if (parseResponseJSON(uartBuffer, response)) {
                        bool commandFound = false;
                        for (int i = 0; i < MAX_PENDING_COMMANDS; i++) {
                            if (pendingCommands[i].pending && pendingCommands[i].id == response.id) {
                                pendingCommands[i].pending = false;
                                pendingCommandsCount--;
                                commandFound = true;
                                
                                if (response.status == "ok") {
                                    iotConnected = true;
                                    Serial.println("[IOT-RESPONSE-SUCCESS] Komenda wykonana pomyślnie: " + pendingCommands[i].action);
                                    Serial.println();
                                    
                                    // Update state cache from response
                                    updateStateFromResponse(pendingCommands[i].action, response.result);
                                } else {
                                    Serial.println("[IOT-RESPONSE-ERROR] Komenda nieudana: " + pendingCommands[i].action + " - " + response.status);
                                    Serial.println();
                                }
                                break;
                            }
                        }
                        
                        if (response.type == "pong") {
                            iotConnected = true;
                            Serial.println("[IOT-HEARTBEAT] Otrzymano pong z IoT - połączenie aktywne");
                            Serial.println();
                        }
                    } else {
                        Serial.println("[IOT-PARSE-ERROR] Błąd parsowania JSON: " + lastError);
                        Serial.println();
                    }
                } else {
                    if (uartBuffer.indexOf("KEEPALIVE") != -1) {
                        iotConnected = true;
                        Serial.println("[IOT-KEEPALIVE] Otrzymano keepalive z IoT - połączenie aktywne");
                        Serial.println();
                    }
                }
                
                uartBuffer = "";
            }
        } else {
            uartBuffer += c;
            
            if (uartBuffer.length() > UART_BUFFER_SIZE) {
                Serial.println("[WEBSERVER-UART-ERROR] Przepełnienie bufora UART");
                Serial.println();
                uartBuffer = "";
                lastError = "Buffer overflow";
            }
        }
    }
}

/**
 * Sprawdź timeouty komend
 */
void checkCommandTimeouts() {
    unsigned long now = millis();
    
    for (int i = 0; i < MAX_PENDING_COMMANDS; i++) {
        if (pendingCommands[i].pending) {
            if (now - pendingCommands[i].timestamp > COMMAND_TIMEOUT) {
                pendingCommands[i].retryCount++;
                
                if (pendingCommands[i].retryCount >= MAX_RETRIES) {
                    Serial.println("[WEBSERVER-UART-TIMEOUT] Komenda przekroczyła limit prób: " + pendingCommands[i].action);
                    Serial.println();
                    
                    pendingCommands[i].pending = false;
                    pendingCommandsCount--;
                    lastError = "Command timeout";
                    iotConnected = false;
                } else {
                    Serial.println("[WEBSERVER-UART-RETRY] Ponowna próba komendy: " + pendingCommands[i].action + " (próba " + String(pendingCommands[i].retryCount) + ")");
                    Serial.println();
                    
                    String retryJson = createCommandJSON(pendingCommands[i].action, pendingCommands[i].params);
                    Serial1.println(retryJson);
                    pendingCommands[i].timestamp = now;
                }
            }
        }
    }
}

// ================= LED CONTROL FUNCTIONS (Moved from actuators) =================

/**
 * Sterowanie LED przez UART
 */
void setLEDState(bool state) {
    String params = "{\"state\":" + String(state ? "true" : "false") + "}";
    
    if (sendCommand("set_led", params)) {
        ledState = state; // Update cache optimistically
        
        Serial.println("[WEBSERVER-LED-COMMAND] Wysłano komendę LED: " + String(state ? "ON" : "OFF"));
        Serial.println();
    } else {
        Serial.println("[WEBSERVER-LED-ERROR] Błąd wysyłania komendy LED");
        Serial.println();
    }
}

/**
 * Pobierz stan LED z cache
 */
bool getLEDState() {
    return ledState;
}

/**
 * Aktualizuj cache na podstawie odpowiedzi z IoT
 */
void updateStateFromResponse(const String& action, const String& result) {
    if (action == "set_led") {
        if (result.indexOf("\"led_state\":true") != -1) {
            ledState = true;
            Serial.println("[WEBSERVER-LED-SYNC] Stan LED potwierdzony przez IoT: ON");
            Serial.println();
        } else if (result.indexOf("\"led_state\":false") != -1) {
            ledState = false;
            Serial.println("[WEBSERVER-LED-SYNC] Stan LED potwierdzony przez IoT: OFF");
            Serial.println();
        }
    }
}

/**
 * Żądaj synchronizacji stanu z IoT
 */
void requestStateSync() {
    Serial.println("[WEBSERVER-SYNC-REQUEST] Żądanie synchronizacji stanu z IoT");
    Serial.println();
    
    if (sendCommand("get_status", "{}")) {
        Serial.println("[WEBSERVER-SYNC-SENT] Komenda synchronizacji wysłana");
        Serial.println();
    } else {
        Serial.println("[WEBSERVER-SYNC-ERROR] Błąd wysyłania synchronizacji");
        Serial.println();
    }
}

/**
 * Przetwórz odpowiedź synchronizacji stanu
 */
void processSyncResponse(const String& result) {
    Serial.println("[WEBSERVER-SYNC-RESPONSE] Przetwarzanie odpowiedzi synchronizacji");
    Serial.println(result);
    Serial.println();
    
    if (result.indexOf("\"led_state\":true") != -1) {
        ledState = true;
    } else if (result.indexOf("\"led_state\":false") != -1) {
        ledState = false;
    }
}

// ================= STATUS FUNCTIONS =================

/**
 * Pobierz ostatnią odpowiedź
 */
UARTResponse getLastResponse(uint32_t commandId) {
    UARTResponse emptyResponse;
    emptyResponse.valid = false;
    return emptyResponse;
}

/**
 * Sprawdź połączenie z IoT
 */
bool isIoTConnected() {
    return iotConnected && (millis() - lastHeartbeat < HEARTBEAT_INTERVAL * 2);
}

/**
 * Posprzątaj stare komendy
 */
void cleanupOldCommands() {
    // Handled by checkCommandTimeouts()
}

/**
 * Pobierz liczbę oczekujących komend
 */
int getPendingCommandsCount() {
    return pendingCommandsCount;
}

/**
 * Pobierz czas ostatniego heartbeat
 */
unsigned long getLastHeartbeat() {
    return lastHeartbeat;
}

/**
 * Pobierz status połączenia
 */
String getConnectionStatus() {
    if (isIoTConnected()) {
        return "Connected";
    } else if (pendingCommandsCount > 0) {
        return "Sending...";
    } else {
        return "Disconnected";
    }
}

/**
 * Pobierz ostatni błąd
 */
String getLastError() {
    return lastError;
}