#include "uart.h"
#include "../core/logging.h"

// ================= ZMIENNE GLOBALNE =================
UARTCommand pendingCommands[MAX_PENDING_COMMANDS];
int pendingCommandsCount = 0;
unsigned long lastHeartbeat = 0;
bool iotConnected = false;
String lastError = "";
String uartBuffer = "";

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
    Serial1.setTimeout(100); // Timeout dla readString
    
    pendingCommandsCount = 0;
    lastHeartbeat = 0;
    iotConnected = false;
    lastError = "";
    uartBuffer = "";
    
    // Wyczyść pending commands
    for (int i = 0; i < MAX_PENDING_COMMANDS; i++) {
        pendingCommands[i].pending = false;
        pendingCommands[i].retryCount = 0;
    }
    
    Serial.println("\n============ UART ENHANCED DEBUG ============");
    Serial.printf("[UART] Konfiguracja UART:\n");
    Serial.printf("   RX Pin: %d (GPIO%d)\n", UART_RX_PIN, UART_RX_PIN);
    Serial.printf("   TX Pin: %d (GPIO%d)\n", UART_TX_PIN, UART_TX_PIN);
    Serial.printf("   Baud: %d\n", UART_BAUD_RATE);
    Serial.printf("   Format: 8N1\n");
    Serial.println("============================================");
    
    LOG_INFO_MSG("UART", "UART zainicjalizowany");
    
    // TEST: Seria testowych wiadomości
    delay(2000);
    Serial.println("[UART TEST] Wysyłam serię testowych wiadomości...");
    for (int i = 0; i < 5; i++) {
        String testMsg = "TEST_ESP32_MSG_" + String(i) + "_BAUD_" + String(UART_BAUD_RATE);
        Serial1.println(testMsg);
        Serial1.flush();
        Serial.println("[UART TEST] Sent: " + testMsg);
        delay(500);
        
        // Sprawdź czy coś przyszło
        if (Serial1.available()) {
            Serial.printf("[UART TEST] Response available: %d bytes\n", Serial1.available());
            while (Serial1.available()) {
                char c = Serial1.read();
                Serial.printf("[UART TEST] Received: '%c' (%d)\n", c, (int)c);
            }
        }
    }
    Serial.println("[UART TEST] Test zakończony\n");
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
    // Znajdź pozycję CRC w JSON i usuń ją do kalkulacji
    int crcPos = jsonStr.indexOf("\"crc\":");
    if (crcPos == -1) return false;
    
    String dataForCrc = jsonStr.substring(0, crcPos);
    dataForCrc += "}"; // Zamknij JSON
    
    uint16_t calculatedCrc = calculateCRC16(dataForCrc);
    String calculatedCrcStr = String(calculatedCrc, HEX);
    calculatedCrcStr.toUpperCase();
    
    return (calculatedCrcStr == expectedCrc);
}

/**
 * Generuj ID komendy
 */
uint32_t generateCommandId() {
    return millis(); // Używamy timestamp jako ID
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
    
    // Parsuj params jako JSON jeśli to możliwe
    if (params.length() > 0) {
        JsonDocument paramsDoc;
        DeserializationError error = deserializeJson(paramsDoc, params);
        if (error) {
            // Jeśli nie JSON, traktuj jako string
            doc["params"] = params;
        } else {
            doc["params"] = paramsDoc;
        }
    }
    
    // Serializuj bez CRC
    String jsonWithoutCrc;
    serializeJson(doc, jsonWithoutCrc);
    
    // Oblicz CRC
    uint16_t crc = calculateCRC16(jsonWithoutCrc);
    String crcStr = String(crc, HEX);
    crcStr.toUpperCase();
    
    // Dodaj CRC
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
    
    // Sprawdź wymagane pola
    if (!doc["id"].is<uint32_t>() || !doc["type"].is<String>() || !doc["crc"].is<String>()) {
        lastError = "Missing required fields in response";
        return false;
    }
    
    response.id = doc["id"].as<uint32_t>();
    response.type = doc["type"].as<String>();
    response.status = doc["status"].as<String>();
    response.crc = doc["crc"].as<String>();
    
    // Walidacja CRC
    if (!validateCRC(jsonStr, response.crc)) {
        lastError = "CRC validation failed";
        return false;
    }
    
    // Serializuj result i sensors jako stringi
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
    
    // ENHANCED DEBUG: Sprawdź czy pin TX faktycznie wysyła
    Serial.printf("[UART DEBUG] About to send %d bytes on TX pin %d\n", jsonCommand.length(), UART_TX_PIN);
    Serial.printf("[UART DEBUG] First 50 chars: %s\n", jsonCommand.substring(0, 50).c_str());
    
    // Wyślij przez UART
    Serial1.println(jsonCommand);
    Serial1.flush();  // Wymuś wysłanie
    
    // Log na Serial Monitor
    Serial.println("[UART TX] " + jsonCommand);
    
    // ENHANCED DEBUG: Sprawdź czy coś jest w buforze RX
    delay(100);  // Daj czas na odpowiedź
    if (Serial1.available()) {
        Serial.printf("[UART DEBUG] %d bytes available after send\n", Serial1.available());
    } else {
        Serial.println("[UART DEBUG] No response in RX buffer");
    }
    
    return true;
}

/**
 * Wyślij heartbeat
 */
bool sendHeartbeat() {
    String heartbeatJson = createCommandJSON("ping", "{}");
    Serial1.println(heartbeatJson);
    
    Serial.println("[UART TX PING] " + heartbeatJson);
    lastHeartbeat = millis();
    
    return true;
}

/**
 * Przetwarzaj dane UART
 */
void processUARTData() {
    // Sprawdź timeouty komend
    checkCommandTimeouts();
    
    // Heartbeat co minutę
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
        sendHeartbeat();
    }
    
    // Debug: sprawdź czy są jakieś dane do odczytu
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 5000) { // Co 5 sekund
        lastCheck = millis();
        if (Serial1.available()) {
            Serial.printf("[UART DEBUG] %d bytes available\n", Serial1.available());
        }
    }
    
    // Czytaj dane z UART
    while (Serial1.available()) {
        char c = Serial1.read();
        
        // Debug każdy znak
        Serial.printf("[UART DEBUG] Received: '%c' (%d)\n", c, (int)c);
        
        if (c == '\n' || c == '\r') {
            if (uartBuffer.length() > 0) {
                // Mamy kompletną linię - przetwórz
                Serial.println("[UART RX] " + uartBuffer);
                
                // NAPRAWA: Sprawdź czy to JSON przed parsowaniem
                if (uartBuffer.startsWith("{") && uartBuffer.endsWith("}")) {
                    // To prawdopodobnie JSON - spróbuj sparsować
                    UARTResponse response;
                    if (parseResponseJSON(uartBuffer, response)) {
                        // Znajdź odpowiednie pending command
                        bool commandFound = false;
                        for (int i = 0; i < MAX_PENDING_COMMANDS; i++) {
                            if (pendingCommands[i].pending && pendingCommands[i].id == response.id) {
                                pendingCommands[i].pending = false;
                                pendingCommandsCount--;
                                commandFound = true;
                                
                                if (response.status == "ok") {
                                    iotConnected = true;
                                    Serial.printf("[UART] Command %u completed successfully\n", response.id);
                                } else {
                                    Serial.printf("[UART] Command %u failed: %s\n", response.id, response.status.c_str());
                                }
                                break;
                            }
                        }
                        
                        // Sprawdź czy to pong (może nie mieć matching command)
                        if (response.type == "pong") {
                            iotConnected = true;
                            Serial.println("[UART] Heartbeat pong received");
                        }
                        
                        if (!commandFound && response.type != "pong") {
                            Serial.printf("[UART] Received response for unknown command ID: %u\n", response.id);
                        }
                    } else {
                        Serial.println("[UART] Failed to parse JSON response: " + lastError);
                    }
                } else {
                    // NAPRAWA: To nie JSON - prawdopodobnie keepalive lub inne
                    Serial.println("[UART] Non-JSON message (keepalive?): " + uartBuffer);
                    
                    // Sprawdź czy to keepalive od IoT
                    if (uartBuffer.indexOf("KEEPALIVE") != -1) {
                        iotConnected = true;  // Jeśli dostajemy keepalive, IoT działa
                        Serial.println("[UART] Keepalive received - IoT is alive");
                    }
                }
                
                uartBuffer = "";
            }
        } else {
            uartBuffer += c;
            
            // Zabezpieczenie przed przepełnieniem bufora
            if (uartBuffer.length() > UART_BUFFER_SIZE) {
                Serial.println("[UART DEBUG] Buffer overflow! Clearing...");
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
                    // Command failed - usuń z kolejki
                    Serial.printf("[UART] Command %u timeout after %d retries\n", 
                                 pendingCommands[i].id, MAX_RETRIES);
                    pendingCommands[i].pending = false;
                    pendingCommandsCount--;
                    lastError = "Command timeout";
                    iotConnected = false;
                } else {
                    // Retry command
                    Serial.printf("[UART] Retrying command %u (attempt %d)\n", 
                                 pendingCommands[i].id, pendingCommands[i].retryCount);
                    
                    String retryJson = createCommandJSON(pendingCommands[i].action, pendingCommands[i].params);
                    Serial1.println(retryJson);
                    Serial.println("[UART TX RETRY] " + retryJson);
                    
                    pendingCommands[i].timestamp = now;
                }
            }
        }
    }
}

/**
 * Pobierz ostatnią odpowiedź
 */
UARTResponse getLastResponse(uint32_t commandId) {
    UARTResponse emptyResponse;
    emptyResponse.valid = false;
    // W tej implementacji nie cachujemy responses
    // Można rozszerzyć w przyszłości
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
    // Ta funkcja jest wywoływana przez checkCommandTimeouts()
    // Można dodać dodatkowe czyszczenie jeśli potrzeba
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