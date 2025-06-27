// #include <WiFi.h>
// #include <ESPAsyncWebServer.h>
// #include "time.h"

// // ================= KONFIGURACJA SIECI =================
// const char* ssid = "KiG";        // ZMIEŃ NA SWOJĄ SIEĆ
// const char* password = "3YqvXx5s3Z";    // ZMIEŃ NA SWOJE HASŁO WIFI

// // ================= KONFIGURACJA BEZPIECZEŃSTWA =================
// const char* admin_password = "admin";  // ZMIEŃ NA SWOJE HASŁO

// // *** ROZSZERZENIE: Dodaj tutaj IP które mają mieć dostęp do ESP32 ***
// const IPAddress allowedIPs[] = {
//     IPAddress(192, 168, 0, 124),  // Przykład: komputer
//     IPAddress(192, 168, 1, 101),  // Przykład: telefon  
//     IPAddress(192, 168, 1, 102)   // Dodaj więcej IP tutaj
// };
// const int allowedIPsCount = sizeof(allowedIPs) / sizeof(allowedIPs[0]);

// // ================= KONFIGURACJA SPRZĘTOWA =================
// const int LED_PIN = 2;  // Pin LED (dla ESP32C6 użyj pin 15)

// // *** ROZSZERZENIE: Dodaj tutaj więcej pinów dla różnych urządzeń ***
// // ============= PRZEKAŹNIKI =============
// // const int RELAY_1_PIN = 4;       // Przekaźnik 1 (np. oświetlenie)
// // const int RELAY_2_PIN = 5;       // Przekaźnik 2 (np. wentylator)
// // const int RELAY_3_PIN = 18;      // Przekaźnik 3 (np. pompa)

// // ============= SERWOMECHANIZMY =============
// // const int SERVO_1_PIN = 9;       // Servo 1 (np. żaluzje)
// // const int SERVO_2_PIN = 10;      // Servo 2 (np. zawór)
// // #include <ESP32Servo.h>
// // Servo servo1, servo2;

// // ============= SENSORY ANALOGOWE =============
// // const int TEMP_SENSOR_PIN = A0;  // Sensor temperatury
// // const int LIGHT_SENSOR_PIN = A1; // Sensor światła
// // const int MOISTURE_SENSOR_PIN = A2; // Sensor wilgotności

// // ============= SENSORY CYFROWE =============
// // const int PIR_SENSOR_PIN = 12;   // Czujnik ruchu PIR
// // const int DOOR_SENSOR_PIN = 13;  // Czujnik drzwi (magnetic)
// // const int BUTTON_PIN = 14;       // Przycisk fizyczny

// // ============= PWM/DIMMER =============
// // const int LED_STRIP_PIN = 16;    // Taśma LED (PWM)
// // const int FAN_SPEED_PIN = 17;    // Sterowanie prędkością wentylatora

// // ================= STRUKTURY SESJI =================
// struct SessionInfo {
//     String token;
//     IPAddress ip;
//     unsigned long createdAt;
//     unsigned long lastActivity;
//     bool isValid;
// };

// const int MAX_SESSIONS = 10;
// const unsigned long SESSION_TIMEOUT = 300000;  // 5 minut w ms
// SessionInfo sessions[MAX_SESSIONS];
// int sessionCount = 0;

// // ================= STRUKTURY RATE LIMITING =================
// struct ClientInfo {
//     IPAddress ip;
//     unsigned long lastRequest;
//     unsigned long blockedUntil;
//     int failedAttempts;
//     int requestCount;
//     unsigned long requestWindow;
// };

// const int MAX_CLIENTS = 50;
// const unsigned long RATE_LIMIT_WINDOW = 1000;    // 1 sekunda
// const int MAX_REQUESTS_PER_WINDOW = 5;            // 5 żądań na sekundę
// const int MAX_FAILED_ATTEMPTS = 20;               // 20 nieudanych prób
// const unsigned long BLOCK_DURATION = 60000;       // 1 minuta blokady

// ClientInfo clients[MAX_CLIENTS];
// int clientCount = 0;

// // ================= ZMIENNE GLOBALNE =================
// AsyncWebServer server(80);  // HTTP na porcie 80
// bool ledState = false;

// // ================= PROTOTYPY FUNKCJI =================
// void setupWiFi();
// void setupNTPTime();
// bool isNTPSynced();

// // *** ROZSZERZENIE: Dodaj tutaj zmienne stanu dla nowych urządzeń ***
// // ============= STANY PRZEKAŹNIKÓW =============
// // bool relay1State = false;  // Stan przekaźnika 1
// // bool relay2State = false;  // Stan przekaźnika 2
// // bool relay3State = false;  // Stan przekaźnika 3

// // ============= POZYCJE SERWOMECHANIZMÓW =============
// // int servo1Position = 90;   // Pozycja servo 1 (0-180°)
// // int servo2Position = 90;   // Pozycja servo 2 (0-180°)

// // ============= WARTOŚCI SENSORÓW =============
// // float temperature = 0.0;  // Temperatura w °C
// // int lightLevel = 0;       // Poziom światła (0-1023)
// // int moistureLevel = 0;    // Poziom wilgotności (0-1023)
// // bool motionDetected = false; // Wykryty ruch
// // bool doorOpen = false;    // Stan drzwi (otwarte/zamknięte)

// // ============= PWM/DIMMER =============
// // int ledStripBrightness = 0;  // Jasność taśmy LED (0-255)
// // int fanSpeed = 0;           // Prędkość wentylatora (0-255)

// // ============= HARMONOGRAMY I AUTOMATYKA =============
// // struct Schedule {
// //     int hour, minute;     // Czas wykonania
// //     bool enabled;         // Czy aktywny
// //     String action;        // Akcja do wykonania
// // };
// // Schedule schedules[10];   // Maksymalnie 10 harmonogramów

// // ================= FUNKCJE POMOCNICZE - IP WHITELIST =================

// /**
//  * Sprawdź czy IP jest na liście dozwolonych
//  */
// bool isIPAllowed(IPAddress ip) {
//     for (int i = 0; i < allowedIPsCount; i++) {
//         if (allowedIPs[i] == ip) {
//             return true;
//         }
//     }
//     return false;
// }

// // ================= FUNKCJE POMOCNICZE - SESJE =================

// /**
//  * Generuj losowy token sesji
//  */
// String generateSessionToken() {
//     String token = "";
//     const char chars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
//     for (int i = 0; i < 32; i++) {
//         token += chars[random(0, 62)];
//     }
    
//     return token;
// }

// /**
//  * Znajdź sesję po tokenie
//  */
// SessionInfo* findSessionByToken(String token) {
//     unsigned long now = millis();
    
//     for (int i = 0; i < sessionCount; i++) {
//         if (sessions[i].isValid && sessions[i].token == token) {
//             // Sprawdź czy sesja nie wygasła
//             if (now - sessions[i].lastActivity < SESSION_TIMEOUT) {
//                 sessions[i].lastActivity = now;  // Odśwież aktywność
//                 return &sessions[i];
//             } else {
//                 // Sesja wygasła
//                 sessions[i].isValid = false;
//                 return nullptr;
//             }
//         }
//     }
//     return nullptr;
// }

// /**
//  * Utwórz nową sesję
//  */
// String createSession(IPAddress ip) {
//     String token = generateSessionToken();
//     unsigned long now = millis();
    
//     // Znajdź wolne miejsce lub najstarszą sesję
//     int index = -1;
//     if (sessionCount < MAX_SESSIONS) {
//         index = sessionCount++;
//     } else {
//         // Znajdź najstarszą sesję
//         unsigned long oldestTime = sessions[0].lastActivity;
//         index = 0;
//         for (int i = 1; i < MAX_SESSIONS; i++) {
//             if (sessions[i].lastActivity < oldestTime) {
//                 oldestTime = sessions[i].lastActivity;
//                 index = i;
//             }
//         }
//     }
    
//     // Utwórz sesję
//     sessions[index].token = token;
//     sessions[index].ip = ip;
//     sessions[index].createdAt = now;
//     sessions[index].lastActivity = now;
//     sessions[index].isValid = true;
    
//     return token;
// }

// /**
//  * Usuń sesję
//  */
// void removeSession(String token) {
//     for (int i = 0; i < sessionCount; i++) {
//         if (sessions[i].token == token) {
//             sessions[i].isValid = false;
//             break;
//         }
//     }
// }

// // ================= FUNKCJE POMOCNICZE - RATE LIMITING =================

// /**
//  * Znajdź lub utwórz wpis dla klienta na podstawie IP
//  */
// ClientInfo* findOrCreateClient(IPAddress ip) {
//     // Sprawdź czy klient już istnieje
//     for (int i = 0; i < clientCount; i++) {
//         if (clients[i].ip == ip) {
//             return &clients[i];
//         }
//     }
    
//     // Utwórz nowy wpis jeśli jest miejsce
//     if (clientCount < MAX_CLIENTS) {
//         clients[clientCount].ip = ip;
//         clients[clientCount].lastRequest = 0;
//         clients[clientCount].blockedUntil = 0;
//         clients[clientCount].failedAttempts = 0;
//         clients[clientCount].requestCount = 0;
//         clients[clientCount].requestWindow = 0;
//         return &clients[clientCount++];
//     }
    
//     // Jeśli brak miejsca, zwróć najstarszy wpis
//     int oldestIndex = 0;
//     unsigned long oldestTime = clients[0].lastRequest;
//     for (int i = 1; i < MAX_CLIENTS; i++) {
//         if (clients[i].lastRequest < oldestTime) {
//             oldestTime = clients[i].lastRequest;
//             oldestIndex = i;
//         }
//     }
    
//     // Zresetuj najstarszy wpis dla nowego IP
//     clients[oldestIndex].ip = ip;
//     clients[oldestIndex].lastRequest = 0;
//     clients[oldestIndex].blockedUntil = 0;
//     clients[oldestIndex].failedAttempts = 0;
//     clients[oldestIndex].requestCount = 0;
//     clients[oldestIndex].requestWindow = 0;
    
//     return &clients[oldestIndex];
// }

// /**
//  * Sprawdź rate limiting dla danego IP
//  */
// bool checkRateLimit(IPAddress ip) {
//     ClientInfo* client = findOrCreateClient(ip);
//     unsigned long now = millis();
    
//     // Sprawdź czy IP jest zablokowane
//     if (client->blockedUntil > now) {
//         return false;
//     }
    
//     // Reset blokady jeśli czas minął
//     if (client->blockedUntil != 0 && client->blockedUntil <= now) {
//         client->blockedUntil = 0;
//         client->failedAttempts = 0;
//     }
    
//     // Sprawdź okno czasowe dla żądań
//     if (now - client->requestWindow > RATE_LIMIT_WINDOW) {
//         client->requestWindow = now;
//         client->requestCount = 1;
//     } else {
//         client->requestCount++;
//     }
    
//     // Sprawdź limit żądań
//     if (client->requestCount > MAX_REQUESTS_PER_WINDOW) {
//         client->blockedUntil = now + BLOCK_DURATION;
//         return false;
//     }
    
//     client->lastRequest = now;
//     return true;
// }

// /**
//  * Zapisz nieudaną próbę logowania
//  */
// void recordFailedLogin(IPAddress ip) {
//     ClientInfo* client = findOrCreateClient(ip);
//     client->failedAttempts++;
    
//     if (client->failedAttempts >= MAX_FAILED_ATTEMPTS) {
//         client->blockedUntil = millis() + BLOCK_DURATION;
//         Serial.printf("[SECURITY] IP %s zablokowane na 1 minutę\n", ip.toString().c_str());
//     }
// }

// /**
//  * Zresetuj licznik nieudanych prób dla IP
//  */
// void resetFailedAttempts(IPAddress ip) {
//     ClientInfo* client = findOrCreateClient(ip);
//     if (client->failedAttempts > 0) {
//         client->failedAttempts = 0;
//     }
// }

// // ================= FUNKCJE POMOCNICZE - UWIERZYTELNIANIE =================

// /**
//  * Sprawdź uwierzytelnienie (IP + sesja)
//  */
// bool checkAuthentication(AsyncWebServerRequest* request) {
//     IPAddress clientIP = request->client()->remoteIP();
    
//     // Sprawdź IP whitelist - TYMCZASOWO WYŁĄCZONE
//     /*
//     if (!isIPAllowed(clientIP)) {
//         return false;
//     }
//     */
    
//     // Sprawdź rate limiting
//     if (!checkRateLimit(clientIP)) {
//         return false;
//     }
    
//     // Sprawdź cookie z tokenem sesji
//     if (request->hasHeader("Cookie")) {
//         String cookies = request->getHeader("Cookie")->value();
//         int sessionStart = cookies.indexOf("session=");
//         if (sessionStart != -1) {
//             sessionStart += 8; // długość "session="
//             int sessionEnd = cookies.indexOf(";", sessionStart);
//             if (sessionEnd == -1) sessionEnd = cookies.length();
            
//             String sessionToken = cookies.substring(sessionStart, sessionEnd);
//             SessionInfo* session = findSessionByToken(sessionToken);
            
//             if (session != nullptr && session->ip == clientIP) {
//                 return true;
//             }
//         }
//     }
    
//     return false;
// }

// // ================= FUNKCJE POMOCNICZE - STEROWANIE =================

// /**
//  * Sterowanie LED
//  */
// void setLedState(bool state) {
//     ledState = state;
//     digitalWrite(LED_PIN, state ? HIGH : LOW);
//     Serial.printf("[LED] Stan LED: %s\n", state ? "WŁĄCZONA" : "WYŁĄCZONA");
// }

// // *** ROZSZERZENIE: Dodaj tutaj funkcje sterowania różnymi urządzeniami ***

// // ============= FUNKCJE STEROWANIA PRZEKAŹNIKAMI =============
// /*
// void setRelayState(int relayNumber, bool state) {
//     switch(relayNumber) {
//         case 1:
//             relay1State = state;
//             digitalWrite(RELAY_1_PIN, state ? HIGH : LOW);
//             Serial.printf("[RELAY] Przekaźnik 1: %s\n", state ? "WŁĄCZONY" : "WYŁĄCZONY");
//             break;
//         case 2:
//             relay2State = state;
//             digitalWrite(RELAY_2_PIN, state ? HIGH : LOW);
//             Serial.printf("[RELAY] Przekaźnik 2: %s\n", state ? "WŁĄCZONY" : "WYŁĄCZONY");
//             break;
//         case 3:
//             relay3State = state;
//             digitalWrite(RELAY_3_PIN, state ? HIGH : LOW);
//             Serial.printf("[RELAY] Przekaźnik 3: %s\n", state ? "WŁĄCZONY" : "WYŁĄCZONY");
//             break;
//     }
// }
// */

// // ============= FUNKCJE STEROWANIA SERWOMECHANIZMAMI =============
// /*
// void setServoPosition(int servoNumber, int position) {
//     position = constrain(position, 0, 180);
    
//     switch(servoNumber) {
//         case 1:
//             servo1Position = position;
//             servo1.write(position);
//             Serial.printf("[SERVO] Servo 1 pozycja: %d°\n", position);
//             break;
//         case 2:
//             servo2Position = position;
//             servo2.write(position);
//             Serial.printf("[SERVO] Servo 2 pozycja: %d°\n", position);
//             break;
//     }
// }
// */

// // ============= FUNKCJE ODCZYTU SENSORÓW =============
// /*
// void readAllSensors() {
//     // Sensory analogowe
//     temperature = (analogRead(TEMP_SENSOR_PIN) * 3.3 / 4095.0 - 0.5) * 100; // TMP36
//     lightLevel = analogRead(LIGHT_SENSOR_PIN);
//     moistureLevel = analogRead(MOISTURE_SENSOR_PIN);
    
//     // Sensory cyfrowe
//     motionDetected = digitalRead(PIR_SENSOR_PIN);
//     doorOpen = digitalRead(DOOR_SENSOR_PIN);
    
//     Serial.printf("[SENSORS] Temp: %.1f°C, Światło: %d, Wilgotność: %d\n", 
//                  temperature, lightLevel, moistureLevel);
// }
// */

// // ============= FUNKCJE PWM/DIMMER =============
// /*
// void setLedStripBrightness(int brightness) {
//     brightness = constrain(brightness, 0, 255);
//     ledStripBrightness = brightness;
//     analogWrite(LED_STRIP_PIN, brightness);
//     Serial.printf("[PWM] Taśma LED jasność: %d/255\n", brightness);
// }

// void setFanSpeed(int speed) {
//     speed = constrain(speed, 0, 255);
//     fanSpeed = speed;
//     analogWrite(FAN_SPEED_PIN, speed);
//     Serial.printf("[PWM] Wentylator prędkość: %d/255\n", speed);
// }
// */

// // ============= FUNKCJE AUTOMATYKI I HARMONOGRAMÓW =============
// /*
// void checkSchedules() {
//     // Sprawdzaj harmonogramy co minutę
//     static unsigned long lastScheduleCheck = 0;
//     if (millis() - lastScheduleCheck > 60000) {
//         lastScheduleCheck = millis();
        
//         // Pobierz aktualny czas (trzeba dodać RTC lub NTP)
//         // struct tm timeinfo;
//         // if (getLocalTime(&timeinfo)) {
//         //     int currentHour = timeinfo.tm_hour;
//         //     int currentMinute = timeinfo.tm_min;
//         //     
//         //     for (int i = 0; i < 10; i++) {
//         //         if (schedules[i].enabled && 
//         //             schedules[i].hour == currentHour && 
//         //             schedules[i].minute == currentMinute) {
//         //             executeScheduledAction(schedules[i].action);
//         //         }
//         //     }
//         // }
//     }
// }

// void executeScheduledAction(String action) {
//     if (action == "led_on") setLedState(true);
//     else if (action == "led_off") setLedState(false);
//     else if (action == "relay1_on") setRelayState(1, true);
//     else if (action == "relay1_off") setRelayState(1, false);
//     // Dodaj więcej akcji...
    
//     Serial.printf("[SCHEDULE] Wykonano akcję: %s\n", action.c_str());
// }
// */

// /**
//  * Konfiguracja WiFi
//  */
// void setupWiFi() {
//     Serial.println("\n[WIFI] Rozpoczynanie połączenia WiFi...");
//     Serial.printf("[WIFI] SSID: %s\n", ssid);
    
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
    
//     int attempts = 0;
//     while (WiFi.status() != WL_CONNECTED && attempts < 30) {
//         delay(1000);
//         Serial.print(".");
//         attempts++;
        
//         if (attempts % 10 == 0) {
//             Serial.printf("\n[WIFI] Próba %d/30...\n", attempts);
//         }
//     }
    
//     if (WiFi.status() == WL_CONNECTED) {
//         Serial.println("\n[WIFI] ✅ Połączono z WiFi!");
//         Serial.printf("[WIFI] IP: %s\n", WiFi.localIP().toString().c_str());
//         Serial.printf("[WIFI] Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
//         Serial.printf("[WIFI] DNS: %s\n", WiFi.dnsIP().toString().c_str());
//         Serial.printf("[WIFI] RSSI: %d dBm\n", WiFi.RSSI());
        
//         // Synchronizacja czasu z serwerami NTP (czas letni/zimowy dla Polski)
//         setupNTPTime();
//     } else {
//         Serial.println("\n[WIFI] ❌ Nie udało się połączyć z WiFi!");
//         Serial.println("[WIFI] Sprawdź SSID i hasło, restartowanie za 10 sekund...");
//         delay(10000);
//         ESP.restart();
//     }
// }

// /**
//  * Konfiguracja NTP - czas dla Polski z automatycznym czasem letnim/zimowym
//  */
// void setupNTPTime() {
//     Serial.println("[NTP] Synchronizacja czasu...");
    
//     // Konfiguracja dla Polski:
//     // CET-1CEST,M3.5.0,M10.5.0/3
//     // CET-1: Central European Time UTC+1 (czas zimowy)
//     // CEST: Central European Summer Time UTC+2 (czas letni)
//     // M3.5.0: ostatnia niedziela marca o 2:00
//     // M10.5.0/3: ostatnia niedziela października o 3:00
//     configTime(0, 0, "pool.ntp.org", "time.nist.gov", "time.google.com");
//     setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
//     tzset();
    
//     // Czekaj na synchronizację (max 15 sekund)
//     int ntpAttempts = 0;
//     time_t now = time(nullptr);
//     while (now < 1609459200 && ntpAttempts < 15) { // 1609459200 = 1 Jan 2021 (sprawdzenie czy czas jest sensowny)
//         delay(1000);
//         now = time(nullptr);
//         ntpAttempts++;
//         Serial.print(".");
//     }
    
//     if (now >= 1609459200) {
//         Serial.println("\n[NTP] ✅ Czas zsynchronizowany!");
//         struct tm timeinfo;
//         if (getLocalTime(&timeinfo)) {
//             Serial.printf("[NTP] Aktualny czas: %02d-%02d-%04d %02d:%02d:%02d\n",
//                          timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
//                          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
//             Serial.printf("[NTP] Strefa czasowa: %s\n", timeinfo.tm_isdst ? "CEST (letni)" : "CET (zimowy)");
//         }
//     } else {
//         Serial.println("\n[NTP] ⚠️ Nie udało się zsynchronizować czasu");
//         Serial.println("[NTP] Używam czasu systemowego (millis), cookie może mieć problemy");
//     }
// }

// /**
//  * Sprawdź czy czas NTP jest zsynchronizowany
//  */
// bool isNTPSynced() {
//     time_t now = time(nullptr);
//     return (now >= 1609459200); // Sprawdź czy czas jest po 1 stycznia 2021
// }

// // ================= STRONY HTML =================

// const char* loginPage = R"rawliteral(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset="UTF-8">
//     <title>ESP32 Secure Login</title>
//     <meta name="viewport" content="width=device-width, initial-scale=1">
//     <style>
//         body {
//             font-family: Arial, sans-serif;
//             background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
//             margin: 0;
//             padding: 0;
//             display: flex;
//             justify-content: center;
//             align-items: center;
//             min-height: 100vh;
//         }
//         .login-container {
//             background: white;
//             padding: 40px;
//             border-radius: 10px;
//             box-shadow: 0 10px 25px rgba(0,0,0,0.2);
//             width: 100%;
//             max-width: 400px;
//         }
//         .logo {
//             text-align: center;
//             margin-bottom: 30px;
//         }
//         .logo h1 {
//             color: #333;
//             margin: 0;
//             font-size: 28px;
//         }
//         .logo p {
//             color: #666;
//             margin: 5px 0 0 0;
//             font-size: 14px;
//         }
//         .form-group {
//             margin-bottom: 20px;
//         }
//         label {
//             display: block;
//             margin-bottom: 8px;
//             color: #333;
//             font-weight: bold;
//         }
//         input[type="password"] {
//             width: 100%;
//             padding: 12px;
//             border: 2px solid #ddd;
//             border-radius: 5px;
//             font-size: 16px;
//             box-sizing: border-box;
//             transition: border-color 0.3s;
//         }
//         input[type="password"]:focus {
//             outline: none;
//             border-color: #667eea;
//         }
//         .login-btn {
//             width: 100%;
//             padding: 12px;
//             background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
//             color: white;
//             border: none;
//             border-radius: 5px;
//             font-size: 16px;
//             font-weight: bold;
//             cursor: pointer;
//             transition: transform 0.2s;
//         }
//         .login-btn:hover {
//             transform: translateY(-2px);
//         }
//         .security-info {
//             margin-top: 20px;
//             padding: 15px;
//             background: #f8f9fa;
//             border-radius: 5px;
//             font-size: 12px;
//             color: #666;
//         }
//     </style>
// </head>
// <body>
//     <div class="login-container">
//         <div class="logo">
//             <h1>🔒 ESP32 Secure</h1>
//             <p>LAN Dashboard</p>
//         </div>
//         <form method="POST" action="/login">
//             <div class="form-group">
//                 <label for="password">Hasło administratora:</label>
//                 <input type="password" id="password" name="password" required>
//             </div>
//             <button type="submit" class="login-btn">Zaloguj się</button>
//         </form>
//         <div class="security-info">
//             <strong>🛡️ Zabezpieczenia:</strong><br>
//             • IP Whitelist + Rate limiting<br>
//             • Sesje z automatycznym timeout<br>
//             • Blokada po przekroczeniu limitów
//         </div>
//     </div>
// </body>
// </html>
// )rawliteral";

// // JavaScript dla dashboard
// const char* dashboardJS = R"rawliteral(
//         function checkLEDStatus() {
//             fetch('/api/led/status')
//                 .then(response => response.json())
//                 .then(data => updateLEDUI(data.state))
//                 .catch(error => console.error('Błąd sprawdzania stanu LED:', error));
//         }
        
//         function updateLEDUI(state) {
//             const ledSwitch = document.getElementById('ledSwitch');
//             const ledStatus = document.getElementById('ledStatus');
            
//             ledSwitch.checked = state;
            
//             if (state) {
//                 ledStatus.textContent = 'LED jest WŁĄCZONA';
//                 ledStatus.className = 'led-status led-on';
//             } else {
//                 ledStatus.textContent = 'LED jest WYŁĄCZONA';
//                 ledStatus.className = 'led-status led-off';
//             }
//         }
        
//         function toggleLED() {
//             const ledSwitch = document.getElementById('ledSwitch');
//             const newState = ledSwitch.checked;
            
//             fetch('/api/led/toggle', {
//                 method: 'POST',
//                 headers: {
//                     'Content-Type': 'application/json'
//                 },
//                 body: JSON.stringify({ state: newState })
//             })
//             .then(response => response.ok ? response.json() : Promise.reject('Błąd serwera'))
//             .then(data => updateLEDUI(data.state))
//             .catch(error => {
//                 console.error('Błąd przełączania LED:', error);
//                 ledSwitch.checked = !newState;
//                 alert('Błąd podczas przełączania LED');
//             });
//         }
        
//         // *** ROZSZERZENIE: Funkcje JavaScript dla nowych urządzeń ***
        
//         // ============= STEROWANIE PRZEKAŹNIKAMI =============
//         /*
//         function toggleRelay(relayNumber) {
//             fetch('/api/relay/' + relayNumber + '/toggle', { method: 'POST' })
//                 .then(response => response.json())
//                 .then(data => updateRelayUI(relayNumber, data.state))
//                 .catch(error => console.error('Błąd sterowania przekaźnikiem:', error));
//         }
        
//         function updateRelayUI(relayNumber, state) {
//             const relaySwitch = document.getElementById('relay' + relayNumber + 'Switch');
//             const relayStatus = document.getElementById('relay' + relayNumber + 'Status');
            
//             relaySwitch.checked = state;
//             relayStatus.textContent = 'Przekaźnik ' + relayNumber + ': ' + (state ? 'WŁĄCZONY' : 'WYŁĄCZONY');
//             relayStatus.className = state ? 'device-status device-on' : 'device-status device-off';
//         }
//         */
        
//         // ============= STEROWANIE SERWOMECHANIZMAMI =============
//         /*
//         function setServoPosition(servoNumber, position) {
//             fetch('/api/servo/' + servoNumber + '/set', {
//                 method: 'POST',
//                 headers: { 'Content-Type': 'application/json' },
//                 body: JSON.stringify({ position: parseInt(position) })
//             })
//             .then(response => response.json())
//             .then(data => {
//                 document.getElementById('servo' + servoNumber + 'Position').textContent = data.position + '°';
//             })
//             .catch(error => console.error('Błąd sterowania servo:', error));
//         }
//         */
        
//         // ============= STEROWANIE PWM/DIMMER =============
//         /*
//         function setLedStripBrightness(brightness) {
//             fetch('/api/ledstrip/set', {
//                 method: 'POST',
//                 headers: { 'Content-Type': 'application/json' },
//                 body: JSON.stringify({ brightness: parseInt(brightness) })
//             })
//             .then(response => response.json())
//             .then(data => {
//                 document.getElementById('ledStripValue').textContent = data.brightness + '/255';
//             });
//         }
        
//         function setFanSpeed(speed) {
//             fetch('/api/fan/set', {
//                 method: 'POST',
//                 headers: { 'Content-Type': 'application/json' },
//                 body: JSON.stringify({ speed: parseInt(speed) })
//             })
//             .then(response => response.json())
//             .then(data => {
//                 document.getElementById('fanSpeedValue').textContent = data.speed + '/255';
//             });
//         }
//         */
        
//         // ============= ODCZYT SENSORÓW =============
//         /*
//         function readSensors() {
//             fetch('/api/sensors/read')
//                 .then(response => response.json())
//                 .then(data => {
//                     document.getElementById('temperature').textContent = data.temperature + '°C';
//                     document.getElementById('lightLevel').textContent = data.lightLevel;
//                     document.getElementById('moistureLevel').textContent = data.moistureLevel;
//                     document.getElementById('motionStatus').textContent = data.motionDetected ? 'WYKRYTY' : 'BRAK';
//                     document.getElementById('doorStatus').textContent = data.doorOpen ? 'OTWARTE' : 'ZAMKNIĘTE';
//                 })
//                 .catch(error => console.error('Błąd odczytu sensorów:', error));
//         }
//         */
        
//         function getSystemInfo() {
//             fetch('/api/system/info')
//                 .then(response => response.json())
//                 .then(data => {
//                     document.getElementById('ipAddress').textContent = data.ip;
//                     document.getElementById('uptime').textContent = formatUptime(parseInt(data.uptime));
//                     document.getElementById('freeHeap').textContent = data.freeHeap + ' bytes';
//                     document.getElementById('rssi').textContent = data.rssi + ' dBm';
                    
//                     // Pokaż informacje o czasie jeśli dostępne
//                     if (data.currentTime) {
//                         document.getElementById('currentTime').textContent = data.currentTime;
//                     }
//                     if (data.timezone) {
//                         document.getElementById('timezone').textContent = data.timezone;
//                     }
//                     if (data.ntpSynced !== undefined) {
//                         document.getElementById('ntpStatus').textContent = data.ntpSynced ? 'Zsynchronizowany' : 'Nie zsynchronizowany';
//                         document.getElementById('ntpStatus').className = data.ntpSynced ? 'ntp-synced' : 'ntp-not-synced';
//                     }
//                 })
//                 .catch(error => console.error('Błąd pobierania informacji o systemie:', error));
//         }
        
//         function formatUptime(milliseconds) {
//             const seconds = Math.floor(milliseconds / 1000);
//             const minutes = Math.floor(seconds / 60);
//             const hours = Math.floor(minutes / 60);
//             const days = Math.floor(hours / 24);
            
//             if (days > 0) {
//                 return days + 'd ' + (hours % 24) + 'h ' + (minutes % 60) + 'm';
//             } else if (hours > 0) {
//                 return hours + 'h ' + (minutes % 60) + 'm ' + (seconds % 60) + 's';
//             } else if (minutes > 0) {
//                 return minutes + 'm ' + (seconds % 60) + 's';
//             } else {
//                 return seconds + 's';
//             }
//         }
        
//         function logout() {
//             fetch('/logout', { method: 'POST' })
//                 .then(() => {
//                     window.location.href = '/';
//                 });
//         }
        
//         window.onload = function() {
//             checkLEDStatus();
//             getSystemInfo();
//             setInterval(getSystemInfo, 30000);
            
//             // *** ROZSZERZENIE: Inicjalizacja nowych funkcji ***
//             // setInterval(readSensors, 5000);     // Odczyt sensorów co 5 sekund
//             // setInterval(checkSchedules, 60000); // Sprawdzanie harmonogramów co minutę
//         };
// )rawliteral";

// const char* dashboardPage = R"rawliteral(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta charset="UTF-8">
//     <title>ESP32 Dashboard</title>
//     <meta name="viewport" content="width=device-width, initial-scale=1">
//     <style>
//         body {
//             font-family: Arial, sans-serif;
//             background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
//             margin: 0;
//             padding: 20px;
//             min-height: 100vh;
//         }
//         .container {
//             max-width: 1200px;
//             margin: 0 auto;
//         }
//         .header {
//             background: white;
//             padding: 20px;
//             border-radius: 10px;
//             box-shadow: 0 5px 15px rgba(0,0,0,0.1);
//             margin-bottom: 20px;
//             text-align: center;
//             position: relative;
//         }
//         .header h1 {
//             margin: 0;
//             color: #333;
//         }
//         .logout-btn {
//             position: absolute;
//             top: 20px;
//             right: 20px;
//             background: #dc3545;
//             color: white;
//             border: none;
//             padding: 8px 16px;
//             border-radius: 5px;
//             cursor: pointer;
//             font-size: 14px;
//         }
//         .logout-btn:hover {
//             background: #c82333;
//         }
//         .card {
//             background: white;
//             padding: 30px;
//             border-radius: 10px;
//             box-shadow: 0 5px 15px rgba(0,0,0,0.1);
//             margin-bottom: 20px;
//         }
//         .card-grid {
//             display: grid;
//             grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
//             gap: 20px;
//         }
//         .device-control {
//             text-align: center;
//         }
//         .device-status {
//             font-size: 18px;
//             margin-bottom: 20px;
//             padding: 15px;
//             border-radius: 5px;
//             font-weight: bold;
//         }
//         .led-on, .device-on {
//             background: #d4edda;
//             color: #155724;
//             border: 1px solid #c3e6cb;
//         }
//         .led-off, .device-off {
//             background: #f8f9fa;
//             color: #6c757d;
//             border: 1px solid #dee2e6;
//         }
//         .switch-container {
//             margin: 20px 0;
//         }
//         .switch {
//             position: relative;
//             display: inline-block;
//             width: 80px;
//             height: 40px;
//         }
//         .switch input {
//             opacity: 0;
//             width: 0;
//             height: 0;
//         }
//         .slider {
//             position: absolute;
//             cursor: pointer;
//             top: 0;
//             left: 0;
//             right: 0;
//             bottom: 0;
//             background-color: #ccc;
//             transition: .4s;
//             border-radius: 34px;
//         }
//         .slider:before {
//             position: absolute;
//             content: "";
//             height: 32px;
//             width: 32px;
//             left: 4px;
//             bottom: 4px;
//             background-color: white;
//             transition: .4s;
//             border-radius: 50%;
//         }
//         input:checked + .slider {
//             background-color: #4CAF50;
//         }
//         input:checked + .slider:before {
//             transform: translateX(40px);
//         }
//         .info-grid {
//             display: grid;
//             grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
//             gap: 20px;
//             margin-top: 20px;
//         }
//         .info-item {
//             background: #f8f9fa;
//             padding: 15px;
//             border-radius: 5px;
//             border-left: 4px solid #667eea;
//         }
//         .info-label {
//             font-weight: bold;
//             color: #333;
//             margin-bottom: 5px;
//         }
//         .info-value {
//             color: #666;
//             font-family: monospace;
//         }
        
//         /* *** ROZSZERZENIE: Style CSS dla nowych elementów *** */
        
//         .control-button {
//             background: #007bff;
//             color: white;
//             border: none;
//             padding: 10px 20px;
//             border-radius: 5px;
//             cursor: pointer;
//             font-size: 16px;
//             margin: 5px;
//         }
//         .control-button:hover {
//             background: #0056b3;
//         }
//         .control-button.danger {
//             background: #dc3545;
//         }
//         .control-button.danger:hover {
//             background: #c82333;
//         }
//         .slider-control {
//             width: 100%;
//             margin: 20px 0;
//         }
//         .slider-control input[type="range"] {
//             width: 100%;
//             height: 25px;
//         }
//         .slider-control .value-display {
//             font-weight: bold;
//             color: #333;
//             margin-left: 10px;
//         }
//         .sensor-grid {
//             display: grid;
//             grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
//             gap: 15px;
//             margin-top: 15px;
//         }
//         .sensor-item {
//             background: #e9ecef;
//             padding: 10px;
//             border-radius: 5px;
//             text-align: center;
//         }
//         .sensor-value {
//             font-size: 24px;
//             font-weight: bold;
//             color: #495057;
//         }
//         .sensor-label {
//             font-size: 12px;
//             color: #6c757d;
//         }
//         .ntp-synced {
//             color: #28a745;
//             font-weight: bold;
//         }
//         .ntp-not-synced {
//             color: #dc3545;
//             font-weight: bold;
//         }
//     </style>
// </head>
// <body>
//     <div class="container">
//         <div class="header">
//             <button class="logout-btn" onclick="logout()">Wyloguj</button>
//             <h1>🚀 ESP32 LAN Dashboard</h1>
//             <p>Inteligentny dom - Panel sterowania</p>
//         </div>
        
//         <!-- Główne sterowanie LED -->
//         <div class="card">
//             <div class="device-control">
//                 <h2>💡 Sterowanie LED</h2>
//                 <div id="ledStatus" class="device-status led-off">
//                     LED jest WYŁĄCZONA
//                 </div>
                
//                 <div class="switch-container">
//                     <label class="switch">
//                         <input type="checkbox" id="ledSwitch" onchange="toggleLED()">
//                         <span class="slider"></span>
//                     </label>
//                 </div>
                
//                 <p>Użyj przełącznika powyżej aby włączyć/wyłączyć LED</p>
//             </div>
//         </div>
        
//         <!-- *** ROZSZERZENIE: Karty dla nowych urządzeń *** -->
        
//         <!-- PRZEKAŹNIKI -->
//         <!--
//         <div class="card-grid">
//             <div class="card">
//                 <h2>🔌 Przekaźnik 1 - Oświetlenie</h2>
//                 <div class="device-control">
//                     <div id="relay1Status" class="device-status device-off">Przekaźnik 1: WYŁĄCZONY</div>
//                     <div class="switch-container">
//                         <label class="switch">
//                             <input type="checkbox" id="relay1Switch" onchange="toggleRelay(1)">
//                             <span class="slider"></span>
//                         </label>
//                     </div>
//                 </div>
//             </div>
            
//             <div class="card">
//                 <h2>🌀 Przekaźnik 2 - Wentylator</h2>
//                 <div class="device-control">
//                     <div id="relay2Status" class="device-status device-off">Przekaźnik 2: WYŁĄCZONY</div>
//                     <div class="switch-container">
//                         <label class="switch">
//                             <input type="checkbox" id="relay2Switch" onchange="toggleRelay(2)">
//                             <span class="slider"></span>
//                         </label>
//                     </div>
//                 </div>
//             </div>
            
//             <div class="card">
//                 <h2>💧 Przekaźnik 3 - Pompa</h2>
//                 <div class="device-control">
//                     <div id="relay3Status" class="device-status device-off">Przekaźnik 3: WYŁĄCZONY</div>
//                     <div class="switch-container">
//                         <label class="switch">
//                             <input type="checkbox" id="relay3Switch" onchange="toggleRelay(3)">
//                             <span class="slider"></span>
//                         </label>
//                     </div>
//                 </div>
//             </div>
//         </div>
//         -->
        
//         <!-- SERWOMECHANIZMY -->
//         <!--
//         <div class="card-grid">
//             <div class="card">
//                 <h2>🎛️ Servo 1 - Żaluzje</h2>
//                 <div class="slider-control">
//                     <label>Pozycja (0-180°):</label>
//                     <input type="range" min="0" max="180" value="90" 
//                            onchange="setServoPosition(1, this.value)">
//                     <span id="servo1Position" class="value-display">90°</span>
//                 </div>
//                 <div style="text-align: center; margin-top: 10px;">
//                     <button class="control-button" onclick="setServoPosition(1, 0)">Zamknij</button>
//                     <button class="control-button" onclick="setServoPosition(1, 90)">Środek</button>
//                     <button class="control-button" onclick="setServoPosition(1, 180)">Otwórz</button>
//                 </div>
//             </div>
            
//             <div class="card">
//                 <h2>🚰 Servo 2 - Zawór</h2>
//                 <div class="slider-control">
//                     <label>Pozycja (0-180°):</label>
//                     <input type="range" min="0" max="180" value="90" 
//                            onchange="setServoPosition(2, this.value)">
//                     <span id="servo2Position" class="value-display">90°</span>
//                 </div>
//                 <div style="text-align: center; margin-top: 10px;">
//                     <button class="control-button" onclick="setServoPosition(2, 0)">Zamknij</button>
//                     <button class="control-button" onclick="setServoPosition(2, 180)">Otwórz</button>
//                 </div>
//             </div>
//         </div>
//         -->
        
//         <!-- PWM/DIMMER -->
//         <!--
//         <div class="card-grid">
//             <div class="card">
//                 <h2>🌈 Taśma LED - Dimmer</h2>
//                 <div class="slider-control">
//                     <label>Jasność (0-255):</label>
//                     <input type="range" min="0" max="255" value="0" 
//                            onchange="setLedStripBrightness(this.value)">
//                     <span id="ledStripValue" class="value-display">0/255</span>
//                 </div>
//             </div>
            
//             <div class="card">
//                 <h2>💨 Wentylator - Regulacja prędkości</h2>
//                 <div class="slider-control">
//                     <label>Prędkość (0-255):</label>
//                     <input type="range" min="0" max="255" value="0" 
//                            onchange="setFanSpeed(this.value)">
//                     <span id="fanSpeedValue" class="value-display">0/255</span>
//                 </div>
//             </div>
//         </div>
//         -->
        
//         <!-- SENSORY -->
//         <!--
//         <div class="card">
//             <h2>📊 Sensory</h2>
//             <div class="sensor-grid">
//                 <div class="sensor-item">
//                     <div id="temperature" class="sensor-value">--</div>
//                     <div class="sensor-label">Temperatura</div>
//                 </div>
//                 <div class="sensor-item">
//                     <div id="lightLevel" class="sensor-value">--</div>
//                     <div class="sensor-label">Poziom światła</div>
//                 </div>
//                 <div class="sensor-item">
//                     <div id="moistureLevel" class="sensor-value">--</div>
//                     <div class="sensor-label">Wilgotność</div>
//                 </div>
//                 <div class="sensor-item">
//                     <div id="motionStatus" class="sensor-value">--</div>
//                     <div class="sensor-label">Ruch</div>
//                 </div>
//                 <div class="sensor-item">
//                     <div id="doorStatus" class="sensor-value">--</div>
//                     <div class="sensor-label">Drzwi</div>
//                 </div>
//             </div>
//         </div>
//         -->
        
//         <!-- HARMONOGRAMY I AUTOMATYKA -->
//         <!--
//         <div class="card">
//             <h2>⏰ Harmonogramy</h2>
//             <div style="text-align: center;">
//                 <button class="control-button">Dodaj harmonogram</button>
//                 <button class="control-button">Edytuj harmonogramy</button>
//                 <button class="control-button danger">Wyłącz wszystkie</button>
//             </div>
//             <div id="schedulesList" style="margin-top: 20px;">
//                 <!-- Lista harmonogramów będzie tutaj -->
//             </div>
//         </div>
//         -->
        
//         <!-- Informacje o systemie -->
//         <div class="card">
//             <h2>📊 Informacje o systemie</h2>
//             <div class="info-grid">
//                 <div class="info-item">
//                     <div class="info-label">Adres IP</div>
//                     <div class="info-value" id="ipAddress">Ładowanie...</div>
//                 </div>
//                 <div class="info-item">
//                     <div class="info-label">Czas działania</div>
//                     <div class="info-value" id="uptime">Ładowanie...</div>
//                 </div>
//                 <div class="info-item">
//                     <div class="info-label">Wolna pamięć</div>
//                     <div class="info-value" id="freeHeap">Ładowanie...</div>
//                 </div>
//                 <div class="info-item">
//                     <div class="info-label">Siła sygnału WiFi</div>
//                     <div class="info-value" id="rssi">Ładowanie...</div>
//                 </div>
//                 <div class="info-item">
//                     <div class="info-label">Aktualny czas</div>
//                     <div class="info-value" id="currentTime">Ładowanie...</div>
//                 </div>
//                 <div class="info-item">
//                     <div class="info-label">Strefa czasowa</div>
//                     <div class="info-value" id="timezone">Ładowanie...</div>
//                 </div>
//                 <div class="info-item">
//                     <div class="info-label">Synchronizacja NTP</div>
//                     <div class="info-value" id="ntpStatus">Ładowanie...</div>
//                 </div>
//             </div>
//         </div>
//     </div>
//     <script>
// )rawliteral";

// const char* dashboardPageEnd = R"rawliteral(
//     </script>
// </body>
// </html>
// )rawliteral";

// // ================= SETUP FUNKCJI =================
// void setupServer() {
//     Serial.println("[SERVER] Konfigurowanie endpointów...");
    
//     // ================= GŁÓWNA STRONA =================
//     server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
//         IPAddress clientIP = request->client()->remoteIP();
        
//         // Sprawdź IP whitelist - TYMCZASOWO WYŁĄCZONE
//         /*
//         if (!isIPAllowed(clientIP)) {
//             request->send(403, "text/plain", "Access Denied");
//             return;
//         }
//         */
        
//         // Sprawdź czy użytkownik ma już ważną sesję
//         bool hasValidSession = false;
//         if (request->hasHeader("Cookie")) {
//             String cookies = request->getHeader("Cookie")->value();
            
//             int sessionStart = cookies.indexOf("session=");
//             if (sessionStart != -1) {
//                 sessionStart += 8;
//                 int sessionEnd = cookies.indexOf(";", sessionStart);
//                 if (sessionEnd == -1) sessionEnd = cookies.length();
                
//                 String sessionToken = cookies.substring(sessionStart, sessionEnd);
//                 SessionInfo* session = findSessionByToken(sessionToken);
//                 if (session != nullptr && session->ip == clientIP) {
//                     hasValidSession = true;
//                 }
//             }
//         }
        
//         if (hasValidSession) {
//             // Użytkownik ma ważną sesję - pokaż dashboard
//             String completePage = String(dashboardPage) + String(dashboardJS) + String(dashboardPageEnd);
//             request->send(200, "text/html", completePage);
//         } else {
//             // Brak sesji - pokaż login
//             request->send(200, "text/html", loginPage);
//         }
//     });
    
//     // ================= ENDPOINT LOGOWANIA =================
//     server.on("/login", HTTP_POST, [](AsyncWebServerRequest* request) {
//         IPAddress clientIP = request->client()->remoteIP();
        
//         // Sprawdź IP whitelist - TYMCZASOWO WYŁĄCZONE
//         /*
//         if (!isIPAllowed(clientIP)) {
//             request->send(403, "text/plain", "Access Denied");
//             return;
//         }
//         */
        
//         if (!checkRateLimit(clientIP)) {
//             request->send(429, "text/plain", "Too Many Requests");
//             return;
//         }
        
//         String password = "";
//         if (request->hasParam("password", true)) {
//             password = request->getParam("password", true)->value();
//         }
        
//         if (password == admin_password) {
//             Serial.printf("[AUTH] ✅ Pomyślne logowanie z IP %s\n", clientIP.toString().c_str());
//             resetFailedAttempts(clientIP);
            
//             // Utwórz sesję
//             String sessionToken = createSession(clientIP);
            
//             // Utworzenie kompletnej strony dashboard
//             String completePage = String(dashboardPage) + String(dashboardJS) + String(dashboardPageEnd);
            
//             // Ustaw cookie z sesją używając prawdziwego czasu Unix
//             String cookieHeader;
//             if (isNTPSynced()) {
//                 // Użyj Unix timestamp + SESSION_TIMEOUT
//                 time_t expireTime = time(nullptr) + (SESSION_TIMEOUT / 1000);
//                 struct tm* expireTimeInfo = gmtime(&expireTime);
                
//                 char expireString[64];
//                 strftime(expireString, sizeof(expireString), "%a, %d %b %Y %H:%M:%S GMT", expireTimeInfo);
                
//                 cookieHeader = "session=" + sessionToken + "; Path=/; Expires=" + String(expireString) + "; SameSite=Lax";
//                 Serial.printf("[AUTH] Cookie z NTP: expires %s\n", expireString);
//             } else {
//                 // Fallback: session cookie (bez expiration)
//                 cookieHeader = "session=" + sessionToken + "; Path=/; SameSite=Lax";
//                 Serial.println("[AUTH] Cookie fallback: session cookie (brak NTP)");
//             }
            
//             AsyncWebServerResponse* response = request->beginResponse(200, "text/html", completePage);
//             response->addHeader("Set-Cookie", cookieHeader);
//             request->send(response);
//         } else {
//             Serial.printf("[AUTH] ❌ Błędne hasło z IP %s\n", clientIP.toString().c_str());
//             recordFailedLogin(clientIP);
            
//             request->send(401, "text/html", loginPage);
//         }
//     });
    
//     // ================= ENDPOINT WYLOGOWANIA =================
//     server.on("/logout", HTTP_POST, [](AsyncWebServerRequest* request) {
//         IPAddress clientIP = request->client()->remoteIP();
//         Serial.printf("[AUTH] Wylogowanie IP %s\n", clientIP.toString().c_str());
        
//         if (request->hasHeader("Cookie")) {
//             String cookies = request->getHeader("Cookie")->value();
            
//             int sessionStart = cookies.indexOf("session=");
//             if (sessionStart != -1) {
//                 sessionStart += 8;
//                 int sessionEnd = cookies.indexOf(";", sessionStart);
//                 if (sessionEnd == -1) sessionEnd = cookies.length();
                
//                 String sessionToken = cookies.substring(sessionStart, sessionEnd);
//                 removeSession(sessionToken);
//             }
//         }
        
//         AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "Logged out");
        
//         // Wyczyść cookie używając prawidłowego formatu
//         if (isNTPSynced()) {
//             time_t pastTime = time(nullptr) - 86400; // 24 godziny temu
//             struct tm* pastTimeInfo = gmtime(&pastTime);
//             char expireString[64];
//             strftime(expireString, sizeof(expireString), "%a, %d %b %Y %H:%M:%S GMT", pastTimeInfo);
//             response->addHeader("Set-Cookie", "session=; Path=/; Expires=" + String(expireString) + "; SameSite=Lax");
//         } else {
//             response->addHeader("Set-Cookie", "session=; Path=/; Max-Age=0; SameSite=Lax");
//         }
        
//         request->send(response);
//     });
    
//     // ================= API - STAN LED =================
//     server.on("/api/led/status", HTTP_GET, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         String json = "{\"state\":" + String(ledState ? "true" : "false") + "}";
//         request->send(200, "application/json", json);
//     });
    
//     // ================= API - PRZEŁĄCZENIE LED =================
//     server.on("/api/led/toggle", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         // Toggle stan LED
//         setLedState(!ledState);
        
//         String json = "{\"state\":" + String(ledState ? "true" : "false") + "}";
//         request->send(200, "application/json", json);
//     });
    
//     // *** ROZSZERZENIE: Dodaj tutaj nowe endpointy API dla różnych urządzeń ***
    
//     // ============= API - PRZEKAŹNIKI =============
//     /*
//     server.on("/api/relay/1/toggle", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         setRelayState(1, !relay1State);
//         String json = "{\"state\":" + String(relay1State ? "true" : "false") + "}";
//         request->send(200, "application/json", json);
//     });
    
//     server.on("/api/relay/2/toggle", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         setRelayState(2, !relay2State);
//         String json = "{\"state\":" + String(relay2State ? "true" : "false") + "}";
//         request->send(200, "application/json", json);
//     });
    
//     server.on("/api/relay/3/toggle", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         setRelayState(3, !relay3State);
//         String json = "{\"state\":" + String(relay3State ? "true" : "false") + "}";
//         request->send(200, "application/json", json);
//     });
//     */
    
//     // ============= API - SERWOMECHANIZMY =============
//     /*
//     server.on("/api/servo/1/set", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         if (request->hasParam("position", true)) {
//             int position = request->getParam("position", true)->value().toInt();
//             setServoPosition(1, position);
//         }
        
//         String json = "{\"position\":" + String(servo1Position) + "}";
//         request->send(200, "application/json", json);
//     });
    
//     server.on("/api/servo/2/set", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         if (request->hasParam("position", true)) {
//             int position = request->getParam("position", true)->value().toInt();
//             setServoPosition(2, position);
//         }
        
//         String json = "{\"position\":" + String(servo2Position) + "}";
//         request->send(200, "application/json", json);
//     });
//     */
    
//     // ============= API - PWM/DIMMER =============
//     /*
//     server.on("/api/ledstrip/set", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         if (request->hasParam("brightness", true)) {
//             int brightness = request->getParam("brightness", true)->value().toInt();
//             setLedStripBrightness(brightness);
//         }
        
//         String json = "{\"brightness\":" + String(ledStripBrightness) + "}";
//         request->send(200, "application/json", json);
//     });
    
//     server.on("/api/fan/set", HTTP_POST, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         if (request->hasParam("speed", true)) {
//             int speed = request->getParam("speed", true)->value().toInt();
//             setFanSpeed(speed);
//         }
        
//         String json = "{\"speed\":" + String(fanSpeed) + "}";
//         request->send(200, "application/json", json);
//     });
//     */
    
//     // ============= API - ODCZYT SENSORÓW =============
//     /*
//     server.on("/api/sensors/read", HTTP_GET, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         readAllSensors();
        
//         String json = "{";
//         json += "\"temperature\":" + String(temperature, 1) + ",";
//         json += "\"lightLevel\":" + String(lightLevel) + ",";
//         json += "\"moistureLevel\":" + String(moistureLevel) + ",";
//         json += "\"motionDetected\":" + String(motionDetected ? "true" : "false") + ",";
//         json += "\"doorOpen\":" + String(doorOpen ? "true" : "false");
//         json += "}";
        
//         request->send(200, "application/json", json);
//     });
//     */
    
//     // ============= API - HARMONOGRAMY =============
//     /*
//     server.on("/api/schedules/list", HTTP_GET, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         String json = "[";
//         for (int i = 0; i < 10; i++) {
//             if (schedules[i].enabled) {
//                 if (json != "[") json += ",";
//                 json += "{";
//                 json += "\"id\":" + String(i) + ",";
//                 json += "\"hour\":" + String(schedules[i].hour) + ",";
//                 json += "\"minute\":" + String(schedules[i].minute) + ",";
//                 json += "\"action\":\"" + schedules[i].action + "\"";
//                 json += "}";
//             }
//         }
//         json += "]";
        
//         request->send(200, "application/json", json);
//     });
//     */
    
//     // ================= API - INFORMACJE O SYSTEMIE =================
//     server.on("/api/system/info", HTTP_GET, [](AsyncWebServerRequest* request) {
//         if (!checkAuthentication(request)) {
//             request->send(401, "text/plain", "Unauthorized");
//             return;
//         }
        
//         String json = "{";
//         json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
//         json += "\"uptime\":\"" + String(millis()) + "\",";
//         json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
//         json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
        
//         // Dodaj informacje o czasie
//         if (isNTPSynced()) {
//             time_t now = time(nullptr);
//             struct tm timeinfo;
//             if (getLocalTime(&timeinfo)) {
//                 char timeString[64];
//                 strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
//                 json += "\"currentTime\":\"" + String(timeString) + "\",";
//                 json += "\"timezone\":\"" + String(timeinfo.tm_isdst ? "CEST (letni)" : "CET (zimowy)") + "\",";
//                 json += "\"ntpSynced\":true";
//             }
//         } else {
//             json += "\"currentTime\":\"Nie zsynchronizowany\",";
//             json += "\"timezone\":\"Brak\",";
//             json += "\"ntpSynced\":false";
//         }
        
//         json += "}";
        
//         request->send(200, "application/json", json);
//     });
    
//     // ================= 404 HANDLER =================
//     server.onNotFound([](AsyncWebServerRequest* request) {
//         String message = "<!DOCTYPE html>";
//         message += "<html><head><title>404 Not Found</title>";
//         message += "<style>body{font-family:Arial;text-align:center;padding:50px;}";
//         message += ".error{color:#e74c3c;}.back{margin-top:20px;}";
//         message += "a{color:#3498db;text-decoration:none;}</style></head>";
//         message += "<body><h1 class='error'>404 - Not Found</h1>";
//         message += "<p>Żądana strona nie została znaleziona.</p>";
//         message += "<div class='back'><a href='/'>← Powrót do strony głównej</a></div>";
//         message += "</body></html>";
        
//         request->send(404, "text/html", message);
//     });
    
//     // ================= ROZPOCZĘCIE SERWERA HTTP =================
//     server.begin();
//     Serial.println("[SERVER] ✅ Serwer HTTP uruchomiony na porcie 80");
//     Serial.printf("[SERVER] Dostęp: http://%s\n", WiFi.localIP().toString().c_str());
    
//     // Wyświetl dozwolone IP (jeśli włączone)
//     /*
//     Serial.println("[SECURITY] Dozwolone adresy IP:");
//     for (int i = 0; i < allowedIPsCount; i++) {
//         Serial.printf("  - %s\n", allowedIPs[i].toString().c_str());
//     }
//     */
// }

// // ================= SETUP =================
// void setup() {
//     Serial.begin(115200);
//     delay(2000);
    
//     // Inicjalizuj generator liczb losowych dla tokenów sesji
//     randomSeed(analogRead(0));
    
//     Serial.println("\n" + String("=").substring(0, 50));
//     Serial.println("🚀 ESP32 HTTP Server - Secure LAN Version");
//     Serial.println("📅 Wersja: 2.1 - " + String(__DATE__) + " " + String(__TIME__));
//     Serial.println(String("=").substring(0, 50));
    
//     // Konfiguracja LED (dla ESP32C6 użyj pin 15)
//     pinMode(LED_PIN, OUTPUT);
//     setLedState(false);
//     Serial.printf("[SETUP] Pin LED: %d\n", LED_PIN);
    
//     // *** ROZSZERZENIE: Dodaj tutaj inicjalizację nowych pinów i urządzeń ***
//     /*
//     // Inicjalizacja przekaźników
//     pinMode(RELAY_1_PIN, OUTPUT);
//     pinMode(RELAY_2_PIN, OUTPUT);
//     pinMode(RELAY_3_PIN, OUTPUT);
//     digitalWrite(RELAY_1_PIN, LOW);
//     digitalWrite(RELAY_2_PIN, LOW);
//     digitalWrite(RELAY_3_PIN, LOW);
//     Serial.println("[SETUP] Przekaźniki zainicjalizowane");
    
//     // Inicjalizacja serwomechanizmów
//     servo1.attach(SERVO_1_PIN);
//     servo2.attach(SERVO_2_PIN);
//     servo1.write(90);
//     servo2.write(90);
//     Serial.println("[SETUP] Serwomechanizmy zainicjalizowane");
    
//     // Inicjalizacja sensorów
//     pinMode(PIR_SENSOR_PIN, INPUT);
//     pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
//     pinMode(BUTTON_PIN, INPUT_PULLUP);
//     Serial.println("[SETUP] Sensory zainicjalizowane");
    
//     // Inicjalizacja PWM
//     pinMode(LED_STRIP_PIN, OUTPUT);
//     pinMode(FAN_SPEED_PIN, OUTPUT);
//     analogWrite(LED_STRIP_PIN, 0);
//     analogWrite(FAN_SPEED_PIN, 0);
//     Serial.println("[SETUP] PWM zainicjalizowane");
//     */
    
//     // Inicjalizacja WiFi
//     setupWiFi();
    
//     // Konfiguracja serwera
//     setupServer();
    
//     Serial.println("\n[SYSTEM] ✅ Inicjalizacja zakończona pomyślnie!");
//     Serial.println("[SYSTEM] 🔐 Hasło administratora: " + String(admin_password));
//     Serial.println("[SYSTEM] 🛡️ Zabezpieczenia: Sessions + Rate limiting");
//     Serial.printf("[SYSTEM] 🕒 Timeout sesji: %lu minut\n", SESSION_TIMEOUT / 60000);
//     Serial.println(String("=").substring(0, 50) + "\n");
// }

// // ================= LOOP =================
// void loop() {
//     // Sprawdź połączenie WiFi
//     if (WiFi.status() != WL_CONNECTED) {
//         Serial.println("[WIFI] ❌ Utracono połączenie WiFi, próba ponownego połączenia...");
//         setupWiFi();
//     }
    
//     // Monitoring systemu co 30 sekund
//     static unsigned long lastSystemCheck = 0;
//     if (millis() - lastSystemCheck > 30000) {
//         lastSystemCheck = millis();
        
//         // Policz aktywne sesje
//         int activeSessions = 0;
//         unsigned long now = millis();
//         for (int i = 0; i < sessionCount; i++) {
//             if (sessions[i].isValid && (now - sessions[i].lastActivity) < SESSION_TIMEOUT) {
//                 activeSessions++;
//             }
//         }
        
//         Serial.printf("[SYSTEM] Uptime: %lu min, Pamięć: %d bytes, Sesje: %d\n", 
//                      millis() / 60000, ESP.getFreeHeap(), activeSessions);
//     }
    
//     // *** ROZSZERZENIE: Dodaj tutaj obsługę automatyki i harmonogramów ***
//     /*
//     // Sprawdzaj harmonogramy
//     checkSchedules();
    
//     // Odczytuj sensory co 10 sekund
//     static unsigned long lastSensorRead = 0;
//     if (millis() - lastSensorRead > 10000) {
//         lastSensorRead = millis();
//         readAllSensors();
        
//         // Przykład automatyki: włącz światło gdy ciemno i wykryty ruch
//         if (lightLevel < 100 && motionDetected && !relay1State) {
//             setRelayState(1, true);
//             Serial.println("[AUTO] Włączono światło - wykryty ruch w ciemności");
//         }
        
//         // Automatyczne wyłączenie światła po 5 minutach bez ruchu
//         static unsigned long lastMotion = 0;
//         if (motionDetected) {
//             lastMotion = millis();
//         } else if (relay1State && (millis() - lastMotion > 300000)) {
//             setRelayState(1, false);
//             Serial.println("[AUTO] Wyłączono światło - brak ruchu przez 5 minut");
//         }
//     }
//     */
    
//     // Czyszczenie pamięci co 5 minut
//     static unsigned long lastCleanup = 0;
//     if (millis() - lastCleanup > 300000) {
//         lastCleanup = millis();
//         unsigned long now = millis();
//         int cleanedClients = 0;
//         int cleanedSessions = 0;
        
//         // Czyszczenie starych klientów
//         for (int i = 0; i < clientCount; i++) {
//             if (clients[i].blockedUntil == 0 && 
//                 (now - clients[i].lastRequest) > 3600000) {
//                 if (i < clientCount - 1) {
//                     clients[i] = clients[clientCount - 1];
//                     i--;
//                 }
//                 clientCount--;
//                 cleanedClients++;
//             }
//         }
        
//         // Czyszczenie wygasłych sesji
//         for (int i = 0; i < sessionCount; i++) {
//             if (!sessions[i].isValid || 
//                 (now - sessions[i].lastActivity) > SESSION_TIMEOUT) {
//                 if (sessions[i].isValid) {
//                     sessions[i].isValid = false;
//                     cleanedSessions++;
//                 }
//             }
//         }
        
//         if (cleanedClients > 0 || cleanedSessions > 0) {
//             Serial.printf("[SYSTEM] Wyczyszczono: %d klientów, %d sesji\n", 
//                          cleanedClients, cleanedSessions);
//         }
//     }
    
//     delay(100);
// }