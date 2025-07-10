// /**
//  * RASPBERRY PI PICO2 WATER SYSTEM - KOD TESTOWY (NAPRAWIONY)
//  * 
//  * Uproszczony kod do testowania wszystkich komponentów
//  * Użyj PRZED uruchomieniem głównej aplikacji
//  * 
//  * Test sequence:
//  * 1. RTC DS3231M (DFRobot)
//  * 2. SD Card (SPI)
//  * 3. Water sensor (GPIO)
//  * 4. Pump relay (GPIO)
//  * 5. UART communication
//  */

// #include <Arduino.h>
// #include <Wire.h>
// #include <SPI.h>
// #include <SD.h>
// #include "DFRobot_DS3231M.h"

// // ================= PINY PICO2 =================
// #define WATER_SENSOR_PIN    15  // GP15 - Float switch
// #define PUMP_RELAY_PIN      14  // GP14 - Relay control
// #define SD_CS_PIN           17  // GP17 - SD CS
// #define SD_MOSI_PIN         19  // GP19 - SD MOSI
// #define SD_MISO_PIN         16  // GP16 - SD MISO  
// #define SD_CLK_PIN          18  // GP18 - SD CLK
// #define RTC_SDA_PIN         4   // GP4 - RTC SDA
// #define RTC_SCL_PIN         5   // GP5 - RTC SCL
// #define UART_TX_PIN         8   // GP8 - UART TX
// #define UART_RX_PIN         9   // GP9 - UART RX

// // ================= OBJEKTY =================
// DFRobot_DS3231M rtc;

// // ================= ZMIENNE TESTOWE =================
// bool rtcWorking = false;
// bool sdWorking = false;
// bool sensorWorking = false;
// bool pumpWorking = false;
// bool uartWorking = false;

// // ================= DEKLARACJE FUNKCJI =================
// void testRTC();
// void testSDCard();
// void testWaterSensor();
// void testPumpRelay();
// void testUART();
// void printTestSummary();
// void printQuickStatus();
// void handleCommand(char cmd);

// // ================= SETUP I LOOP =================
// void setup() {
//     // USB Serial
//     Serial.begin(115200);
//     while (!Serial && millis() < 5000) {
//         delay(10);
//     }
//     delay(1000);
    
//     Serial.println("\n" + String("=").substring(0, 60));
//     Serial.println("🧪 RASPBERRY PI PICO2 WATER SYSTEM - TEST MODE");
//     Serial.println("📋 Testing all components before main application");
//     Serial.println(String("=").substring(0, 60));
//     Serial.println();
    
//     // Test wszystkich komponentów
//     testRTC();
//     testSDCard();
//     testWaterSensor();
//     testPumpRelay();
//     testUART();
    
//     // Podsumowanie testów
//     printTestSummary();
    
//     Serial.println("\n🔄 Starting continuous monitoring...");
//     Serial.println("Send 'p' to test pump, 'r' to read RTC, 's' for sensor");
// }

// void loop() {
//     // Continuous monitoring
//     static unsigned long lastStatus = 0;
//     if (millis() - lastStatus > 5000) {
//         lastStatus = millis();
//         printQuickStatus();
//     }
    
//     // Interactive commands
//     if (Serial.available()) {
//         char cmd = Serial.read();
//         handleCommand(cmd);
//     }
    
//     delay(100);
// }

// // ================= TESTY KOMPONENTÓW =================
// /**
//  * Test RTC DS3231M
//  */
// void testRTC() {
//     Serial.println("🕒 TEST 1: RTC DS3231M (DFRobot)");
//     Serial.println("----------------------------------------");
    
//     // Konfiguracja I2C
//     Wire.setSDA(RTC_SDA_PIN);
//     Wire.setSCL(RTC_SCL_PIN);
//     Wire.begin();
    
//     Serial.printf("I2C pins: SDA=GP%d, SCL=GP%d\n", RTC_SDA_PIN, RTC_SCL_PIN);
    
//     // Inicjalizacja RTC
//     int attempts = 0;
//     while(rtc.begin() != true && attempts < 5) {
//         Serial.printf("RTC init attempt %d/5...\n", attempts + 1);
//         delay(1000);
//         attempts++;
//     }
    
//     if (attempts >= 5) {
//         Serial.println("❌ RTC DS3231M initialization FAILED");
//         Serial.println("   Check I2C connections and power");
//         rtcWorking = false;
//     } else {
//         Serial.println("✅ RTC DS3231M initialized successfully");
        
//         // Test odczytu czasu
//         rtc.getNowTime();
//         Serial.printf("📅 Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
//                       rtc.year(), rtc.month(), rtc.day(),
//                       rtc.hour(), rtc.minute(), rtc.second());
        
//         // Test temperatury
//         float temp = rtc.getTemperatureC();
//         Serial.printf("🌡️  RTC temperature: %.2f°C\n", temp);
        
//         // Sprawdź czy czas się zmienia
//         int sec1 = rtc.second();
//         delay(1100);
//         rtc.getNowTime();
//         int sec2 = rtc.second();
        
//         if (sec1 != sec2) {
//             Serial.println("✅ RTC time is counting");
//             rtcWorking = true;
//         } else {
//             Serial.println("❌ RTC time not changing - check crystal");
//             rtcWorking = false;
//         }
        
//         if (rtc.lostPower()) {
//             Serial.println("⚠️  RTC lost power - time may be incorrect");
//         }
//     }
    
//     Serial.println();
// }

// /**
//  * Test karty SD - uproszczona wersja
//  */
// void testSDCard() {
//     Serial.println("💾 TEST 2: MicroSD Card (SPI)");
//     Serial.println("----------------------------------------");
    
//     // Konfiguracja SPI
//     SPI.setRX(SD_MISO_PIN);
//     SPI.setTX(SD_MOSI_PIN);
//     SPI.setSCK(SD_CLK_PIN);
    
//     Serial.printf("SPI pins: MISO=GP%d, MOSI=GP%d, CLK=GP%d, CS=GP%d\n",
//                   SD_MISO_PIN, SD_MOSI_PIN, SD_CLK_PIN, SD_CS_PIN);
    
//     // Inicjalizacja SD
//     if (!SD.begin(SD_CS_PIN)) {
//         Serial.println("❌ SD Card initialization FAILED");
//         Serial.println("   Check SD card, connections, and formatting (FAT32)");
//         sdWorking = false;
//     } else {
//         Serial.println("✅ SD Card initialized successfully");
        
//         // Podstawowe info o karcie (bez niedostępnych metod)
//         Serial.println("📊 SD Card detected and accessible");
        
//         // Test zapisu
//         File testFile = SD.open("/test.txt", FILE_WRITE);
//         if (testFile) {
//             testFile.println("Pico2 Water System Test");
//             testFile.println("Timestamp: " + String(millis()));
//             testFile.close();
            
//             // Test odczytu
//             testFile = SD.open("/test.txt");
//             if (testFile) {
//                 Serial.println("✅ SD write/read test successful");
//                 while (testFile.available()) {
//                     String line = testFile.readStringUntil('\n');
//                     line.trim(); // Remove whitespace
//                     if (line.length() > 0) {
//                         Serial.println("   File content: " + line);
//                     }
//                 }
//                 testFile.close();
                
//                 // Usuń plik testowy
//                 SD.remove("/test.txt");
//                 sdWorking = true;
//             } else {
//                 Serial.println("❌ SD read test FAILED");
//                 sdWorking = false;
//             }
//         } else {
//             Serial.println("❌ SD write test FAILED");
//             sdWorking = false;
//         }
//     }
    
//     Serial.println();
// }

// /**
//  * Test czujnika poziomu wody
//  */
// void testWaterSensor() {
//     Serial.println("💧 TEST 3: Water Level Sensor (Float Switch)");
//     Serial.println("----------------------------------------");
    
//     pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
    
//     Serial.printf("Sensor pin: GP%d (INPUT_PULLUP)\n", WATER_SENSOR_PIN);
//     Serial.println("Logic: LOW = water level low, HIGH = water level OK");
    
//     // Test odczytu
//     bool state1 = digitalRead(WATER_SENSOR_PIN);
//     delay(100);
//     bool state2 = digitalRead(WATER_SENSOR_PIN);
//     delay(100);
//     bool state3 = digitalRead(WATER_SENSOR_PIN);
    
//     Serial.printf("Pin readings: %s, %s, %s\n",
//                   state1 ? "HIGH" : "LOW",
//                   state2 ? "HIGH" : "LOW",
//                   state3 ? "HIGH" : "LOW");
    
//     if (state1 == state2 && state2 == state3) {
//         Serial.printf("✅ Sensor stable reading: %s\n", state1 ? "WATER OK" : "WATER LOW");
//         sensorWorking = true;
//     } else {
//         Serial.println("⚠️  Sensor readings unstable - check connections");
//         sensorWorking = false;
//     }
    
//     Serial.println("🔧 Manual test: Try changing float position and check readings");
    
//     Serial.println();
// }

// /**
//  * Test przekaźnika pompy
//  */
// void testPumpRelay() {
//     Serial.println("⚡ TEST 4: Pump Relay Control");
//     Serial.println("----------------------------------------");
    
//     pinMode(PUMP_RELAY_PIN, OUTPUT);
//     digitalWrite(PUMP_RELAY_PIN, LOW);
    
//     Serial.printf("Relay pin: GP%d (OUTPUT)\n", PUMP_RELAY_PIN);
//     Serial.println("Logic: HIGH = pump ON, LOW = pump OFF");
    
//     // Test ON
//     Serial.println("🔄 Testing relay ON (2 seconds)...");
//     digitalWrite(PUMP_RELAY_PIN, HIGH);
//     delay(2000);
    
//     // Test OFF
//     Serial.println("🔄 Testing relay OFF...");
//     digitalWrite(PUMP_RELAY_PIN, LOW);
    
//     Serial.println("✅ Relay control test completed");
//     Serial.println("🔧 Check if relay LED/click responds to commands");
//     Serial.println("⚠️  Connect pump ONLY after confirming relay works");
    
//     pumpWorking = true;  // Assume working if no exceptions
    
//     Serial.println();
// }

// /**
//  * Test komunikacji UART
//  */
// void testUART() {
//     Serial.println("📡 TEST 5: UART Communication");
//     Serial.println("----------------------------------------");
    
//     Serial1.setTX(UART_TX_PIN);
//     Serial1.setRX(UART_RX_PIN);
//     Serial1.begin(1200);
    
//     Serial.printf("UART pins: TX=GP%d, RX=GP%d, Baud=1200\n", UART_TX_PIN, UART_RX_PIN);
//     Serial.println("Purpose: Communication with ESP32-C6 WebServer");
    
//     // Test wysyłania
//     Serial.println("📤 Sending test message...");
//     Serial1.println("{\"test\":\"pico2_ready\",\"timestamp\":" + String(millis()) + "}");
//     Serial1.flush();
    
//     // Test odbierania (timeout 3s)
//     Serial.println("📥 Listening for response (3s timeout)...");
//     unsigned long startTime = millis();
//     String response = "";
    
//     while (millis() - startTime < 3000) {
//         if (Serial1.available()) {
//             response += (char)Serial1.read();
//         }
//         delay(10);
//     }
    
//     if (response.length() > 0) {
//         Serial.println("✅ UART response received:");
//         Serial.println("   " + response);
//         uartWorking = true;
//     } else {
//         Serial.println("⚠️  No UART response (normal if ESP32-C6 not connected)");
//         Serial.println("   Connect ESP32-C6 WebServer for full test");
//         uartWorking = false;  // Not an error, just not connected
//     }
    
//     Serial.println();
// }

// // ================= POMOCNICZE =================
// /**
//  * Podsumowanie testów
//  */
// void printTestSummary() {
//     Serial.println(String("=").substring(0, 60));
//     Serial.println("📊 TEST SUMMARY");
//     Serial.println(String("=").substring(0, 60));
    
//     Serial.printf("🕒 RTC DS3231M:      %s\n", rtcWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("💾 SD Card:          %s\n", sdWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("💧 Water Sensor:     %s\n", sensorWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("⚡ Pump Relay:       %s\n", pumpWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("📡 UART Comm:        %s\n", uartWorking ? "✅ OK" : "⚠️  NO ESP32-C6");
    
//     Serial.println();
    
//     int workingComponents = rtcWorking + sdWorking + sensorWorking + pumpWorking;
    
//     if (workingComponents >= 4) {
//         Serial.println("🎉 SYSTEM READY FOR MAIN APPLICATION!");
//         Serial.println("   You can now upload the main water system code");
//     } else {
//         Serial.println("⚠️  SOME COMPONENTS NEED ATTENTION");
//         Serial.println("   Fix issues before deploying main application");
//     }
    
//     Serial.println(String("=").substring(0, 60));
//     Serial.println();
// }

// /**
//  * Szybki status (continuous monitoring)
//  */
// void printQuickStatus() {
//     // Current time
//     String timeStr = "No RTC";
//     if (rtcWorking) {
//         rtc.getNowTime();
//         char buffer[20];
//         sprintf(buffer, "%02d:%02d:%02d", rtc.hour(), rtc.minute(), rtc.second());
//         timeStr = String(buffer);
//     }
    
//     // Water sensor
//     bool waterLevel = digitalRead(WATER_SENSOR_PIN);
    
//     // Memory
//     uint32_t freeHeap = rp2040.getFreeHeap();
    
//     Serial.printf("[MONITOR] Time:%s Water:%s Memory:%uKB Uptime:%lum\n",
//                   timeStr.c_str(),
//                   waterLevel ? "OK" : "LOW",
//                   freeHeap / 1024,
//                   millis() / 60000);
// }

// /**
//  * Obsługa komend interaktywnych - naprawiona
//  */
// void handleCommand(char cmd) {
//     switch (cmd) {
//         case 'p':
//         case 'P':
//             Serial.println("\n🔄 Manual pump test (3 seconds):");
//             digitalWrite(PUMP_RELAY_PIN, HIGH);
//             Serial.println("   Pump ON");
//             delay(3000);
//             digitalWrite(PUMP_RELAY_PIN, LOW);
//             Serial.println("   Pump OFF");
//             break;
            
//         case 'r':
//         case 'R':
//             if (rtcWorking) {
//                 rtc.getNowTime();
//                 Serial.printf("\n🕒 RTC Reading: %04d-%02d-%02d %02d:%02d:%02d (%.2f°C)\n",
//                               rtc.year(), rtc.month(), rtc.day(),
//                               rtc.hour(), rtc.minute(), rtc.second(),
//                               rtc.getTemperatureC());
//             } else {
//                 Serial.println("\n❌ RTC not working");
//             }
//             break;
            
//         case 's':
//         case 'S': {
//             bool sensorState = digitalRead(WATER_SENSOR_PIN);
//             Serial.printf("\n💧 Water Sensor: %s (%s)\n",
//                           sensorState ? "HIGH" : "LOW",
//                           sensorState ? "Water OK" : "Water LOW");
//             break;
//         }
            
//         case 'd':
//         case 'D':
//             Serial.println("\n🔧 Debug info:");
//             Serial.printf("   Free memory: %u bytes\n", rp2040.getFreeHeap());
//             Serial.printf("   Uptime: %lu seconds\n", millis() / 1000);
//             Serial.printf("   All GPIO states: Sensor=%d, Pump=%d\n",
//                           digitalRead(WATER_SENSOR_PIN),
//                           digitalRead(PUMP_RELAY_PIN));
//             break;
            
//         case 'h':
//         case 'H':
//         case '?':
//             Serial.println("\n📋 Available commands:");
//             Serial.println("   p - Test pump (3 seconds)");
//             Serial.println("   r - Read RTC time");
//             Serial.println("   s - Read water sensor");
//             Serial.println("   d - Debug info");
//             Serial.println("   h - This help");
//             break;
            
//         default:
//             if (cmd != '\n' && cmd != '\r') {
//                 Serial.println("\n❓ Unknown command. Send 'h' for help");
//             }
//             break;
//     }
// }


/**
 * RASPBERRY PI PICO2 WATER SYSTEM - KOD TESTOWY (NAPRAWIONY)
 * 
 * Uproszczony kod do testowania wszystkich komponentów
 * Użyj PRZED uruchomieniem głównej aplikacji
 * 
 * Test sequence:
 * 1. RTC DS3231M (DFRobot)
 * 2. SD Card (SPI)
 * 3. Water sensor (GPIO)
 * 4. Pump relay (GPIO)
 * 5. UART communication
 */

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "DFRobot_DS3231M.h"

// ================= PINY PICO2 =================
#define WATER_SENSOR_PIN    15  // GP15 - Float switch
#define PUMP_RELAY_PIN      14  // GP14 - Relay control
#define SD_CS_PIN           17  // GP17 - SD CS
#define SD_MOSI_PIN         19  // GP19 - SD MOSI
#define SD_MISO_PIN         16  // GP16 - SD MISO  
#define SD_CLK_PIN          18  // GP18 - SD CLK
#define RTC_SDA_PIN         4   // GP4 - RTC SDA
#define RTC_SCL_PIN         5   // GP5 - RTC SCL
#define UART_TX_PIN         8   // GP8 - UART TX
#define UART_RX_PIN         9   // GP9 - UART RX
#define LED_PIN             25  // GP25 - Built-in LED

// ================= OBJEKTY =================
DFRobot_DS3231M rtc;

// ================= ZMIENNE TESTOWE =================
bool rtcWorking = false;
bool sdWorking = false;
bool sensorWorking = false;
bool pumpWorking = false;
bool uartWorking = false;

// ================= DEKLARACJE FUNKCJI =================
void testRTC();
void testSDCard();
void testWaterSensor();
void testPumpRelay();
void testUART();
void printTestSummary();
void printQuickStatus();
void handleCommand(char cmd);
void blinkLED(int times, int delayMs = 200);

// ================= SETUP I LOOP =================
void setup() {
    Serial.begin(115200);
    Serial.println("Start programu");
    // Built-in LED jako wskaźnik statusu
    pinMode(LED_PIN, OUTPUT);
    pinMode(PUMP_RELAY_PIN, OUTPUT);
    // digitalWrite(LED_PIN, HIGH); // LED ON podczas boot
    
    // USB Serial - zwiększony timeout dla stabilności
    // unsigned long serialStart = millis();
    // while (!Serial && (millis() - serialStart < 15000)) {
    //     // Miga LED podczas oczekiwania na Serial
    //     digitalWrite(LED_PIN, (millis() / 300) % 2);
    //     delay(50);
    // }
    
    //Serial gotowy - sygnalizuj 3 szybkimi błyskami
    // blinkLED(3, 100);
    // digitalWrite(LED_PIN, LOW);
    // delay(500);
    
    // Serial.println("\n" + String("=").substring(0, 60));
    // Serial.println("🧪 RASPBERRY PI PICO2 WATER SYSTEM - TEST MODE");
    // Serial.println("📋 Testing all components before main application");
    // Serial.println(String("=").substring(0, 60));
    // Serial.println();
    
    // // Debug info - pomaga z diagnozą
    // Serial.printf("🔧 System info:\n");
    // Serial.printf("   - Free memory: %u bytes\n", rp2040.getFreeHeap());
    // Serial.printf("   - CPU frequency: %u MHz\n", rp2040.f_cpu() / 1000000);
    // Serial.printf("   - Core temperature: %.1f°C\n", analogReadTemp());
    // Serial.printf("   - Boot time: %lu ms\n", millis());
    // Serial.println();
    
    // // Test wszystkich komponentów
    // Serial.println("🚀 Starting component tests...\n");
    
    // testRTC();
    // testSDCard();
    // testWaterSensor();
    // testPumpRelay();
    // testUART();
    
    // // Podsumowanie testów
    // printTestSummary();
    
    // // Sygnalizuj koniec testów
    // if (rtcWorking && sdWorking && sensorWorking && pumpWorking) {
    //     blinkLED(5, 150); // 5 błysków = wszystko OK
    // } else {
    //     blinkLED(10, 100); // 10 szybkich = problemy
    // }
    
    // Serial.println("\n🔄 Starting continuous monitoring...");
    // Serial.println("📡 Send commands: 'p'=pump, 'r'=RTC, 's'=sensor, 'h'=help");
    // Serial.println();
}

void loop() {



     while (true) {
        // Miga LED podczas oczekiwania na Serial
         digitalWrite(LED_PIN, HIGH);
         digitalWrite(PUMP_RELAY_PIN, HIGH);
         Serial.println("LED ON");
          delay(150);
         digitalWrite(LED_PIN, LOW);
         digitalWrite(PUMP_RELAY_PIN, LOW);
         Serial.println("LED OFF");
         delay(150);
    }
}


//     // LED heartbeat - puls co 2 sekundy
//     static unsigned long lastHeartbeat = 0;
//     if (millis() - lastHeartbeat > 2000) {
//         lastHeartbeat = millis();
//         digitalWrite(LED_PIN, HIGH);
//         delay(50);
//         digitalWrite(LED_PIN, LOW);
//     }
    
//     // Status monitoring co 10 sekund
//     static unsigned long lastStatus = 0;
//     if (millis() - lastStatus > 10000) {
//         lastStatus = millis();
//         printQuickStatus();
//     }
    
//     // Interactive commands
//     if (Serial.available()) {
//         char cmd = Serial.read();
//         handleCommand(cmd);
//     }
    
//     delay(100);
// }

// // ================= TESTY KOMPONENTÓW =================
// /**
//  * Test RTC DS3231M
//  */
// void testRTC() {
//     Serial.println("🕒 TEST 1: RTC DS3231M (DFRobot)");
//     Serial.println("----------------------------------------");
    
//     // Konfiguracja I2C
//     Wire.setSDA(RTC_SDA_PIN);
//     Wire.setSCL(RTC_SCL_PIN);
//     Wire.begin();
    
//     Serial.printf("I2C pins: SDA=GP%d, SCL=GP%d\n", RTC_SDA_PIN, RTC_SCL_PIN);
    
//     // Skanowanie I2C bus dla diagnostyki
//     Serial.print("I2C scan: ");
//     for (byte i = 8; i < 120; i++) {
//         Wire.beginTransmission(i);
//         if (Wire.endTransmission() == 0) {
//             Serial.printf("0x%02X ", i);
//         }
//     }
//     Serial.println();
    
//     // Inicjalizacja RTC
//     int attempts = 0;
//     while(rtc.begin() != true && attempts < 5) {
//         Serial.printf("RTC init attempt %d/5...\n", attempts + 1);
//         delay(1000);
//         attempts++;
//     }
    
//     if (attempts >= 5) {
//         Serial.println("❌ RTC DS3231M initialization FAILED");
//         Serial.println("   Check I2C connections and power");
//         Serial.println("   Expected I2C address: 0x68");
//         rtcWorking = false;
//     } else {
//         Serial.println("✅ RTC DS3231M initialized successfully");
        
//         // Test odczytu czasu
//         rtc.getNowTime();
//         Serial.printf("📅 Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
//                       rtc.year(), rtc.month(), rtc.day(),
//                       rtc.hour(), rtc.minute(), rtc.second());
        
//         // Test temperatury
//         float temp = rtc.getTemperatureC();
//         Serial.printf("🌡️  RTC temperature: %.2f°C\n", temp);
        
//         // Sprawdź czy czas się zmienia
//         int sec1 = rtc.second();
//         delay(1100);
//         rtc.getNowTime();
//         int sec2 = rtc.second();
        
//         if (sec1 != sec2) {
//             Serial.println("✅ RTC time is counting");
//             rtcWorking = true;
//         } else {
//             Serial.println("❌ RTC time not changing - check crystal");
//             rtcWorking = false;
//         }
        
//         if (rtc.lostPower()) {
//             Serial.println("⚠️  RTC lost power - time may be incorrect");
//         }
//     }
    
//     Serial.println();
// }

// /**
//  * Test karty SD - uproszczona wersja
//  */
// void testSDCard() {
//     Serial.println("💾 TEST 2: MicroSD Card (SPI)");
//     Serial.println("----------------------------------------");
    
//     // Konfiguracja SPI
//     SPI.setRX(SD_MISO_PIN);
//     SPI.setTX(SD_MOSI_PIN);
//     SPI.setSCK(SD_CLK_PIN);
    
//     Serial.printf("SPI pins: MISO=GP%d, MOSI=GP%d, CLK=GP%d, CS=GP%d\n",
//                   SD_MISO_PIN, SD_MOSI_PIN, SD_CLK_PIN, SD_CS_PIN);
    
//     // Test CS pin
//     pinMode(SD_CS_PIN, OUTPUT);
//     digitalWrite(SD_CS_PIN, HIGH);
//     delay(100);
    
//     // Inicjalizacja SD z retry
//     bool sdInitialized = false;
//     for (int attempt = 1; attempt <= 3; attempt++) {
//         Serial.printf("SD init attempt %d/3...\n", attempt);
//         if (SD.begin(SD_CS_PIN)) {
//             sdInitialized = true;
//             break;
//         }
//         delay(1000);
//     }
    
//     if (!sdInitialized) {
//         Serial.println("❌ SD Card initialization FAILED");
//         Serial.println("   Check: SD card inserted, formatted FAT32, connections");
//         Serial.println("   Try: different SD card, check power supply");
//         sdWorking = false;
//     } else {
//         Serial.println("✅ SD Card initialized successfully");
//         Serial.println("📊 SD Card detected and accessible");
        
//         // Test zapisu/odczytu
//         String testData = "Pico2 Water System Test - " + String(millis());
//         File testFile = SD.open("/pico2_test.txt", FILE_WRITE);
//         if (testFile) {
//             testFile.println(testData);
//             testFile.close();
            
//             // Test odczytu
//             testFile = SD.open("/pico2_test.txt", FILE_READ);
//             if (testFile) {
//                 String readData = testFile.readStringUntil('\n');
//                 testFile.close();
//                 readData.trim();
                
//                 if (readData == testData) {
//                     Serial.println("✅ SD write/read test successful");
//                     sdWorking = true;
//                 } else {
//                     Serial.println("❌ SD data mismatch");
//                     Serial.println("   Written: " + testData);
//                     Serial.println("   Read: " + readData);
//                     sdWorking = false;
//                 }
                
//                 // Cleanup
//                 SD.remove("/pico2_test.txt");
//             } else {
//                 Serial.println("❌ SD read test FAILED");
//                 sdWorking = false;
//             }
//         } else {
//             Serial.println("❌ SD write test FAILED");
//             sdWorking = false;
//         }
//     }
    
//     Serial.println();
// }

// /**
//  * Test czujnika poziomu wody
//  */
// void testWaterSensor() {
//     Serial.println("💧 TEST 3: Water Level Sensor (Float Switch)");
//     Serial.println("----------------------------------------");
    
//     pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
//     delay(100); // Stabilizacja
    
//     Serial.printf("Sensor pin: GP%d (INPUT_PULLUP)\n", WATER_SENSOR_PIN);
//     Serial.println("Logic: LOW = water level low, HIGH = water level OK");
    
//     // Test stabilności przez 1 sekundę
//     bool readings[10];
//     Serial.print("Stability test (1s): ");
//     for (int i = 0; i < 10; i++) {
//         readings[i] = digitalRead(WATER_SENSOR_PIN);
//         Serial.print(readings[i] ? "H" : "L");
//         delay(100);
//     }
//     Serial.println();
    
//     // Sprawdź stabilność
//     bool stable = true;
//     bool firstReading = readings[0];
//     for (int i = 1; i < 10; i++) {
//         if (readings[i] != firstReading) {
//             stable = false;
//             break;
//         }
//     }
    
//     if (stable) {
//         Serial.printf("✅ Sensor stable reading: %s\n", firstReading ? "WATER OK" : "WATER LOW");
//         sensorWorking = true;
//     } else {
//         Serial.println("⚠️  Sensor readings unstable - check connections or sensor");
//         sensorWorking = false;
//     }
    
//     Serial.println("🔧 Manual test: Change float position and use 's' command");
    
//     Serial.println();
// }

// /**
//  * Test przekaźnika pompy
//  */
// void testPumpRelay() {
//     Serial.println("⚡ TEST 4: Pump Relay Control");
//     Serial.println("----------------------------------------");
    
//     pinMode(PUMP_RELAY_PIN, OUTPUT);
//     digitalWrite(PUMP_RELAY_PIN, LOW);
//     delay(100);
    
//     Serial.printf("Relay pin: GP%d (OUTPUT)\n", PUMP_RELAY_PIN);
//     Serial.println("Logic: HIGH = pump ON, LOW = pump OFF");
    
//     // Test OFF state
//     Serial.println("🔄 Testing relay OFF state...");
//     digitalWrite(PUMP_RELAY_PIN, LOW);
//     delay(500);
//     bool offState = digitalRead(PUMP_RELAY_PIN);
    
//     // Test ON state  
//     Serial.println("🔄 Testing relay ON (2 seconds)...");
//     digitalWrite(PUMP_RELAY_PIN, HIGH);
//     delay(500);
//     bool onState = digitalRead(PUMP_RELAY_PIN);
//     delay(1500); // Total 2 seconds ON
    
//     // Test OFF again
//     Serial.println("🔄 Testing relay OFF...");
//     digitalWrite(PUMP_RELAY_PIN, LOW);
//     delay(500);
//     bool finalState = digitalRead(PUMP_RELAY_PIN);
    
//     // Verify
//     if (!offState && onState && !finalState) {
//         Serial.println("✅ Relay control test completed successfully");
//         pumpWorking = true;
//     } else {
//         Serial.printf("❌ Relay test issue: OFF=%d, ON=%d, FINAL=%d\n", 
//                      offState, onState, finalState);
//         pumpWorking = false;
//     }
    
//     Serial.println("🔧 Check if relay LED/click responds to commands");
//     Serial.println("⚠️  Connect pump ONLY after confirming relay works safely");
    
//     Serial.println();
// }

// /**
//  * Test komunikacji UART
//  */
// void testUART() {
//     Serial.println("📡 TEST 5: UART Communication");
//     Serial.println("----------------------------------------");
    
//     Serial1.setTX(UART_TX_PIN);
//     Serial1.setRX(UART_RX_PIN);
//     Serial1.begin(1200);
//     delay(100);
    
//     Serial.printf("UART pins: TX=GP%d, RX=GP%d, Baud=1200\n", UART_TX_PIN, UART_RX_PIN);
//     Serial.println("Purpose: Communication with ESP32-C6 WebServer");
//     Serial.println("Expected: Crossed connection (Pico TX -> ESP32 RX)");
    
//     // Clear any existing data
//     while (Serial1.available()) {
//         Serial1.read();
//     }
    
//     // Test wysyłania
//     String testMessage = "{\"test\":\"pico2_ready\",\"timestamp\":" + String(millis()) + "}";
//     Serial.println("📤 Sending test message:");
//     Serial.println("   " + testMessage);
//     Serial1.println(testMessage);
//     Serial1.flush();
    
//     // Test odbierania z dłuższym timeout
//     Serial.println("📥 Listening for response (5s timeout)...");
//     unsigned long startTime = millis();
//     String response = "";
    
//     while (millis() - startTime < 5000) {
//         if (Serial1.available()) {
//             char c = Serial1.read();
//             response += c;
//             if (c == '\n') break; // Complete line received
//         }
//         delay(10);
//     }
    
//     if (response.length() > 0) {
//         Serial.println("✅ UART response received:");
//         Serial.println("   " + response.substring(0, min(100, (int)response.length())));
//         uartWorking = true;
//     } else {
//         Serial.println("⚠️  No UART response (normal if ESP32-C6 not connected)");
//         Serial.println("   This is not an error - just no device connected");
//         uartWorking = false;  // Not an error, just not connected
//     }
    
//     Serial.println();
// }

// // ================= POMOCNICZE =================
// /**
//  * Podsumowanie testów
//  */
// void printTestSummary() {
//     Serial.println(String("=").substring(0, 60));
//     Serial.println("📊 TEST SUMMARY");
//     Serial.println(String("=").substring(0, 60));
    
//     Serial.printf("🕒 RTC DS3231M:      %s\n", rtcWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("💾 SD Card:          %s\n", sdWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("💧 Water Sensor:     %s\n", sensorWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("⚡ Pump Relay:       %s\n", pumpWorking ? "✅ OK" : "❌ FAIL");
//     Serial.printf("📡 UART Comm:        %s\n", uartWorking ? "✅ OK" : "⚠️  NO ESP32-C6");
    
//     Serial.println();
    
//     int criticalComponents = rtcWorking + sdWorking + sensorWorking + pumpWorking;
    
//     if (criticalComponents >= 4) {
//         Serial.println("🎉 SYSTEM READY FOR MAIN APPLICATION!");
//         Serial.println("   All critical components working");
//         Serial.println("   You can now upload the main water system code");
//     } else {
//         Serial.println("⚠️  SOME CRITICAL COMPONENTS NEED ATTENTION");
//         Serial.println("   Fix hardware issues before deploying main application");
//         Serial.println("   Required: RTC + SD + Sensor + Pump Relay");
//     }
    
//     Serial.println(String("=").substring(0, 60));
//     Serial.println();
// }

// /**
//  * Szybki status (continuous monitoring)
//  */
// void printQuickStatus() {
//     // Current time
//     String timeStr = "No RTC";
//     if (rtcWorking) {
//         rtc.getNowTime();
//         char buffer[20];
//         sprintf(buffer, "%02d:%02d:%02d", rtc.hour(), rtc.minute(), rtc.second());
//         timeStr = String(buffer);
//     }
    
//     // Water sensor
//     bool waterLevel = digitalRead(WATER_SENSOR_PIN);
    
//     // Memory and uptime
//     uint32_t freeHeap = rp2040.getFreeHeap();
//     unsigned long uptimeMin = millis() / 60000;
    
//     Serial.printf("[MONITOR] Time:%s Water:%s Memory:%uKB Uptime:%lum Temp:%.1f°C\n",
//                   timeStr.c_str(),
//                   waterLevel ? "OK" : "LOW",
//                   freeHeap / 1024,
//                   uptimeMin,
//                   analogReadTemp());
// }

// /**
//  * Obsługa komend interaktywnych - naprawiona
//  */
// void handleCommand(char cmd) {
//     switch (cmd) {
//         case 'p':
//         case 'P':
//             Serial.println("\n🔄 Manual pump test (3 seconds):");
//             Serial.println("   ⚠️  ENSURE PUMP IS SAFELY CONNECTED!");
//             digitalWrite(PUMP_RELAY_PIN, HIGH);
//             digitalWrite(LED_PIN, HIGH);
//             Serial.println("   Pump/Relay ON");
//             delay(3000);
//             digitalWrite(PUMP_RELAY_PIN, LOW);
//             digitalWrite(LED_PIN, LOW);
//             Serial.println("   Pump/Relay OFF");
//             break;
            
//         case 'r':
//         case 'R':
//             if (rtcWorking) {
//                 rtc.getNowTime();
//                 Serial.printf("\n🕒 RTC Reading: %04d-%02d-%02d %02d:%02d:%02d (%.2f°C)\n",
//                               rtc.year(), rtc.month(), rtc.day(),
//                               rtc.hour(), rtc.minute(), rtc.second(),
//                               rtc.getTemperatureC());
//             } else {
//                 Serial.println("\n❌ RTC not working - run full test first");
//             }
//             break;
            
//         case 's':
//         case 'S': {
//             // Test sensor several times
//             Serial.println("\n💧 Water Sensor Reading:");
//             for (int i = 0; i < 5; i++) {
//                 bool state = digitalRead(WATER_SENSOR_PIN);
//                 Serial.printf("   Reading %d: %s (%s)\n", i+1,
//                               state ? "HIGH" : "LOW",
//                               state ? "Water OK" : "Water LOW");
//                 delay(200);
//             }
//             break;
//         }
            
//         case 'd':
//         case 'D':
//             Serial.println("\n🔧 Debug info:");
//             Serial.printf("   Free memory: %u bytes (%.1f KB)\n", 
//                          rp2040.getFreeHeap(), rp2040.getFreeHeap()/1024.0);
//             Serial.printf("   Uptime: %lu seconds\n", millis() / 1000);
//             Serial.printf("   Core temp: %.2f°C\n", analogReadTemp());
//             Serial.printf("   GPIO states: Sensor=%d, Pump=%d, LED=%d\n",
//                           digitalRead(WATER_SENSOR_PIN),
//                           digitalRead(PUMP_RELAY_PIN),
//                           digitalRead(LED_PIN));
//             break;
            
//         case 't':
//         case 'T':
//             Serial.println("\n🔄 Re-running all tests...");
//             testRTC();
//             testSDCard();
//             testWaterSensor();
//             testPumpRelay();
//             testUART();
//             printTestSummary();
//             break;
            
//         case 'l':
//         case 'L':
//             blinkLED(5, 200);
//             Serial.println("\n💡 LED blink test completed");
//             break;
            
//         case 'h':
//         case 'H':
//         case '?':
//             Serial.println("\n📋 Available commands:");
//             Serial.println("   p - Test pump (3 seconds) ⚠️ SAFETY!");
//             Serial.println("   r - Read RTC time");
//             Serial.println("   s - Read water sensor (5x)");
//             Serial.println("   d - Debug info");
//             Serial.println("   t - Re-run all tests");
//             Serial.println("   l - LED blink test");
//             Serial.println("   h - This help");
//             break;
            
//         default:
//             if (cmd != '\n' && cmd != '\r') {
//                 Serial.println("\n❓ Unknown command. Send 'h' for help");
//             }
//             break;
//     }
// }

// /**
//  * Helper - miganie LED
//  */
// void blinkLED(int times, int delayMs) {
//     for (int i = 0; i < times; i++) {
//         digitalWrite(LED_PIN, HIGH);
//         delay(delayMs);
//         digitalWrite(LED_PIN, LOW);
//         delay(delayMs);
//     }
// }