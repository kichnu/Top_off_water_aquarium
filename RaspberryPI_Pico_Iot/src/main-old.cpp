// #include <Arduino.h>

// #include <ArduinoJson.h>





// // ================= KONFIGURACJA =================
// const int LED_PIN = 25;        // LED pin dla Beetle RP2350
// const int UART_BAUD = 1200;     // ZMIENIONE: 1200 baud działa lepiej
// const int BUFFER_SIZE = 512;

// // RP2350 UART Pins
// const int UART_TX_PIN = 0;     // UART0 TX
// const int UART_RX_PIN = 1;     // UART0 RX

// // ================= STANY URZĄDZEŃ =================
// bool ledState = false;
// float temperature = 23.5;
// int lightLevel = 512;
// String uartBuffer = "";

// // ================= CRC16 LOOKUP TABLE =================
// const uint16_t crc16_table[256] = {
//     0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
//     0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
//     0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
//     0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
//     0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
//     0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
//     0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
//     0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
//     0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
//     0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
//     0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
//     0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
//     0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
//     0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
//     0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
//     0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
//     0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
//     0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
//     0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
//     0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
//     0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
//     0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
//     0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
//     0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
//     0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
//     0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
//     0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
//     0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
//     0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
//     0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
//     0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
//     0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
// };

// // ================= PROTOTYPY FUNKCJI =================
// uint16_t calculateCRC16(const String& data);
// bool validateCRC(const String& jsonStr, const String& expectedCrc);
// String findJsonValue(const String& json, const String& key);
// String createResponse(const String& id, const String& status, const String& result);
// String createPong(const String& id);
// String executeCommand(const String& action, const String& params);
// void processCommand(const String& jsonStr);

// // ================= FUNKCJE =================

// /**
//  * Oblicz CRC16
//  */
// uint16_t calculateCRC16(const String& data) {
//     uint16_t crc = 0xFFFF;
    
//     for (int i = 0; i < data.length(); i++) {
//         uint8_t tbl_idx = ((crc >> 8) ^ data[i]) & 0xFF;
//         crc = ((crc << 8) ^ crc16_table[tbl_idx]) & 0xFFFF;
//     }
    
//     return crc;
// }

// /**
//  * Waliduj CRC
//  */
// bool validateCRC(const String& jsonStr, const String& expectedCrc) {
//     int crcPos = jsonStr.indexOf("\"crc\":");
//     if (crcPos == -1) return false;
    
//     String dataForCrc = jsonStr.substring(0, crcPos);
//     dataForCrc += "}";
    
//     uint16_t calculatedCrc = calculateCRC16(dataForCrc);
//     String calculatedCrcStr = String(calculatedCrc, HEX);
//     calculatedCrcStr.toUpperCase();
    
//     return (calculatedCrcStr == expectedCrc);
// }

// /**
//  * Znajdź wartość w JSON string (prosty parser)
//  */
// String findJsonValue(const String& json, const String& key) {
//     String searchKey = "\"" + key + "\":";
//     int startPos = json.indexOf(searchKey);
//     if (startPos == -1) return "";
    
//     startPos += searchKey.length();
    
//     // Pomiń białe znaki
//     while (startPos < json.length() && (json[startPos] == ' ' || json[startPos] == '\t')) {
//         startPos++;
//     }
    
//     if (startPos >= json.length()) return "";
    
//     char firstChar = json[startPos];
//     String value = "";
    
//     if (firstChar == '"') {
//         // String value
//         startPos++; // Pomiń opening quote
//         int endPos = json.indexOf('"', startPos);
//         if (endPos != -1) {
//             value = json.substring(startPos, endPos);
//         }
//     } else if (firstChar >= '0' && firstChar <= '9') {
//         // Number value
//         int endPos = startPos;
//         while (endPos < json.length() && 
//                ((json[endPos] >= '0' && json[endPos] <= '9') || 
//                 json[endPos] == '.' || json[endPos] == '-')) {
//             endPos++;
//         }
//         value = json.substring(startPos, endPos);
//     } else if (json.substring(startPos, startPos + 4) == "true") {
//         value = "true";
//     } else if (json.substring(startPos, startPos + 5) == "false") {
//         value = "false";
//     }
    
//     return value;
// }

// /**
//  * Utwórz prostą odpowiedź JSON z CRC
//  */
// String createResponse(const String& id, const String& status, const String& result) {
//     // Utwórz JSON bez CRC
//     String jsonWithoutCrc = "{";
//     jsonWithoutCrc += "\"id\":" + id + ",";
//     jsonWithoutCrc += "\"type\":\"response\",";
//     jsonWithoutCrc += "\"status\":\"" + status + "\",";
//     jsonWithoutCrc += "\"result\":" + result + ",";
//     jsonWithoutCrc += "\"sensors\":{";
//     jsonWithoutCrc += "\"temperature\":" + String(temperature, 1) + ",";
//     jsonWithoutCrc += "\"light_level\":" + String(lightLevel) + ",";
//     jsonWithoutCrc += "\"uptime\":" + String(millis());
//     jsonWithoutCrc += "}";
//     jsonWithoutCrc += "}";
    
//     // Oblicz CRC
//     uint16_t crc = calculateCRC16(jsonWithoutCrc);
//     String crcStr = String(crc, HEX);
//     crcStr.toUpperCase();
    
//     // Utwórz finalny JSON z CRC
//     String response = "{";
//     response += "\"id\":" + id + ",";
//     response += "\"type\":\"response\",";
//     response += "\"status\":\"" + status + "\",";
//     response += "\"result\":" + result + ",";
//     response += "\"sensors\":{";
//     response += "\"temperature\":" + String(temperature, 1) + ",";
//     response += "\"light_level\":" + String(lightLevel) + ",";
//     response += "\"uptime\":" + String(millis());
//     response += "},";
//     response += "\"crc\":\"" + crcStr + "\"";
//     response += "}";
    
//     return response;
// }

// /**
//  * Utwórz pong response z CRC (bez ArduinoJson)
//  */
// String createPong(const String& id) {
//     // Utwórz JSON bez CRC
//     String jsonWithoutCrc = "{";
//     jsonWithoutCrc += "\"id\":" + id + ",";
//     jsonWithoutCrc += "\"type\":\"pong\",";
//     jsonWithoutCrc += "\"uptime\":" + String(millis());
//     jsonWithoutCrc += "}";
    
//     // Oblicz CRC
//     uint16_t crc = calculateCRC16(jsonWithoutCrc);
//     String crcStr = String(crc, HEX);
//     crcStr.toUpperCase();
    
//     // Utwórz finalny JSON z CRC
//     String response = "{";
//     response += "\"id\":" + id + ",";
//     response += "\"type\":\"pong\",";
//     response += "\"uptime\":" + String(millis()) + ",";
//     response += "\"crc\":\"" + crcStr + "\"";
//     response += "}";
    
//     return response;
// }

// /**
//  * Wykonaj komendę
//  */
// String executeCommand(const String& action, const String& params) {
//     Serial.println("[RP2350] Executing: " + action);
    
//     if (action == "set_led") {
//         String stateStr = findJsonValue(params, "state");
//         bool state = (stateStr == "true");
        
//         ledState = state;
//         digitalWrite(LED_PIN, state ? HIGH : LOW);
        
//         String result = "{\"led_state\":" + String(state ? "true" : "false") + "}";
//         Serial.println("[RP2350] LED: " + String(state ? "ON" : "OFF"));
//         return result;
//     }
//     else if (action == "get_status") {
//         String result = "{\"led_state\":" + String(ledState ? "true" : "false") + "}";
//         return result;
//     }
//     else if (action == "ping") {
//         Serial.println("[RP2350] PING received");
//         return "{\"pong\":true}";
//     }
//     else {
//         Serial.println("[RP2350] Unknown: " + action);
//         return "{\"error\":\"unknown\"}";
//     }
// }

// /**
//  * Przetwórz komendę
//  */
// void processCommand(const String& jsonStr) {
//     Serial.println("[RP2350 RX] " + jsonStr);
    
//     String id = findJsonValue(jsonStr, "id");
//     String action = findJsonValue(jsonStr, "action");
    
//     if (id == "" || action == "") {
//         Serial.println("[RP2350] Invalid JSON");
//         return;
//     }
    
//     // Znajdź params
//     String params = "{}";
//     int paramsStart = jsonStr.indexOf("\"params\":");
//     if (paramsStart != -1) {
//         paramsStart += 9;
//         while (paramsStart < jsonStr.length() && 
//                (jsonStr[paramsStart] == ' ' || jsonStr[paramsStart] == '\t')) {
//             paramsStart++;
//         }
        
//         if (paramsStart < jsonStr.length() && jsonStr[paramsStart] == '{') {
//             int braceCount = 0;
//             int paramsEnd = paramsStart;
//             do {
//                 if (jsonStr[paramsEnd] == '{') braceCount++;
//                 if (jsonStr[paramsEnd] == '}') braceCount--;
//                 paramsEnd++;
//             } while (braceCount > 0 && paramsEnd < jsonStr.length());
            
//             params = jsonStr.substring(paramsStart, paramsEnd);
//         }
//     }
    
//     // Wykonaj
//     String result = executeCommand(action, params);
    
//     // Odpowiedź
//     String response;
//     if (action == "ping") {
//         response = createPong(id);
//     } else {
//         response = createResponse(id, "ok", result);
//     }
    
//     // UWAGA: RP2350 używa Serial1 inaczej - sprawdź dokumentację
//     Serial1.println(response);
//     Serial.println("[RP2350 TX] " + response);
// }

// // ================= SETUP =================
// void setup() {
//     // Debug Serial (USB)
//     Serial.begin(115200);
//     delay(2000);
    
//     // LED
//     pinMode(LED_PIN, OUTPUT);
//     digitalWrite(LED_PIN, LOW);
    
//     // *** POPRAWIONA KONFIGURACJA UART dla RP2350 ***
//     Serial1.setTX(UART_TX_PIN);  // Ustaw TX pin
//     Serial1.setRX(UART_RX_PIN);  // Ustaw RX pin
//     Serial1.begin(UART_BAUD);
//     Serial1.setTimeout(100);
    
//     // DODAJ: Sprawdź konfigurację
//     Serial.println("\n========================================");
//     Serial.println("🤖 Beetle RP2350 IoT Test - ENHANCED DEBUG");
//     Serial.println("📡 UART Config:");
//     Serial.println("   Baud: " + String(UART_BAUD));
//     Serial.println("   TX Pin: " + String(UART_TX_PIN) + " (GPIO0)");
//     Serial.println("   RX Pin: " + String(UART_RX_PIN) + " (GPIO1)");
//     Serial.println("   Data: 8N1");
//     Serial.println("💡 LED Pin: " + String(LED_PIN));
//     Serial.println("========================================");
//     Serial.println("Starting UART test...\n");
    
//     // Test LED sequence
//     for (int i = 0; i < 5; i++) {
//         digitalWrite(LED_PIN, HIGH);
//         delay(200);
//         digitalWrite(LED_PIN, LOW);
//         delay(200);
//     }
    
//     // ENHANCED: Test UART immediately after setup
//     Serial.println("[RP2350 SETUP] Testing UART transmission...");
//     for (int i = 0; i < 3; i++) {
//         String testMsg = "TEST_" + String(i) + "_FROM_RP2350_BAUD_" + String(UART_BAUD);
//         Serial1.println(testMsg);
//         Serial1.flush();
//         Serial.println("[RP2350 SETUP] Sent: " + testMsg);
//         delay(1000);
//     }
    
//     Serial.println("[RP2350 SETUP] Setup complete, entering main loop...\n");
// }

// // ================= LOOP =================
// void loop() {
//     // ENHANCED DEBUG: Sprawdź dostępność danych co 2 sekundy (częściej)
//     static unsigned long lastUartCheck = 0;
//     if (millis() - lastUartCheck > 2000) {
//         lastUartCheck = millis();
        
//         int available = Serial1.available();
//         if (available > 0) {
//             Serial.printf("[RP2350 DEBUG] *** %d bytes available in UART buffer ***\n", available);
            
//             // Przeczytaj wszystkie dostępne bajty do debugowania
//             String debugData = "";
//             while (Serial1.available() && debugData.length() < 100) {
//                 char c = Serial1.read();
//                 debugData += c;
//                 Serial.printf("[RP2350 DEBUG] Raw byte: '%c' (%d)\n", c, (int)c);
//             }
//             if (debugData.length() > 0) {
//                 Serial.println("[RP2350 DEBUG] Complete data: " + debugData);
//             }
//         } else {
//             Serial.println("[RP2350 DEBUG] No UART data available");
//         }
//     }
    
//     // Sprawdź dostępność danych UART (normalne przetwarzanie)
//     if (Serial1.available()) {
//         Serial.printf("[RP2350] *** UART data available! (%d bytes) ***\n", Serial1.available());
//     }
    
//     // Czytaj z UART
//     while (Serial1.available()) {
//         char c = Serial1.read();
//         Serial.printf("[RP2350] Received char: '%c' (%d)\n", c, (int)c);
        
//         if (c == '\n' || c == '\r') {
//             if (uartBuffer.length() > 0) {
//                 processCommand(uartBuffer);
//                 uartBuffer = "";
//             }
//         } else {
//             uartBuffer += c;
            
//             if (uartBuffer.length() > BUFFER_SIZE) {
//                 uartBuffer = "";
//                 Serial.println("[RP2350] Buffer overflow!");
//             }
//         }
//     }
    
//     // Test wysyłania co 15 sekund (rzadziej, żeby nie spamować)
//     static unsigned long lastTest = 0;
//     if (millis() - lastTest > 15000) {
//         lastTest = millis();
//         String keepalive = "KEEPALIVE_RP2350_" + String(millis()) + "_BAUD_" + String(UART_BAUD);
//         Serial1.println(keepalive);
//         Serial1.flush();
//         Serial.println("[RP2350 DEBUG] Sent keepalive: " + keepalive);
//     }
    
//     // Aktualizuj sensory co 10s
//     static unsigned long lastUpdate = 0;
//     if (millis() - lastUpdate > 10000) {
//         lastUpdate = millis();
//         temperature += random(-10, 10) / 100.0;
//         lightLevel += random(-20, 20);
//         temperature = constrain(temperature, 15.0, 35.0);
//         lightLevel = constrain(lightLevel, 0, 1023);
        
//         Serial.printf("[RP2350] Sensors: T=%.1f°C, L=%d\n", temperature, lightLevel);
//     }
    
//     // Blink LED co sekundę
//     static unsigned long lastBlink = 0;
//     if (millis() - lastBlink > 1000) {
//         lastBlink = millis();
        
//         static bool blinkState = false;
//         if (!ledState) {
//             blinkState = !blinkState;
//             digitalWrite(LED_PIN, blinkState ? HIGH : LOW);
//         }
//     }
    
//     delay(10);
// }