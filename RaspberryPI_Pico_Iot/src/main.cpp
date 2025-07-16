/**
 * Raspberry Pi Pico2 IoT Water System - Main Application
 * 
 * System dolewania wody z komunikacją UART do WebServer ESP32
 * Funkcje: czujnik poziomu wody, sterowanie pompy, RTC DS3231M, 
 *          logowanie na SD, komunikacja UART JSON+CRC16
 * 
 * Version: 1.0-Pico2 - Raspberry Pi Pico2 Implementation
 * Compatible with: WebServer ESP32 v2.1
 */

// ================= IMPORTY MODUŁÓW =================
#include "config.h"
#include "logic/water_system.h"
#include "hardware/water_sensor.h"
#include "hardware/pump_controller.h"
#include "hardware/rtc_ds3231.h"
#include "hardware/sd_logger.h"
#include "config/settings.h"
#include "communication/uart_protocol.h"
#include <SPI.h>  // Dla karty SD

// ================= ZMIENNE GLOBALNE =================
unsigned long lastStatusPrint = 0;
unsigned long lastSystemMaintenance = 0;
bool systemFullyInitialized = false;

// ================= DEKLARACJE FUNKCJI =================
void printSystemSummary();
void printSystemStatus();
void performSystemMaintenance();
void performStartupTests();
void printFullSystemDebug();
void handleCriticalError(const String& error);

// ================= SETUP =================
void setup() {
    // Initialize USB serial for debugging (Pico2)
    Serial.begin(SERIAL_DEBUG_SPEED);
    while (!Serial && millis() < 5000) {
        delay(10);  // Wait for USB serial connection or timeout after 5s
    }
    delay(1000);
    
    Serial.println("\n" + String("=").substring(0, 50));
    Serial.println("🚀 Raspberry Pi Pico2 IoT Water System - Starting");
    Serial.println("📅 Version: " + String(VERSION) + " - " + String(__DATE__) + " " + String(__TIME__));
    Serial.println("🔗 Compatible with WebServer ESP32 v2.1");
    Serial.println(String("=").substring(0, 50));
    Serial.println();
    
    // ================= INICJALIZACJA SPRZĘTU =================
    Serial.println("[PICO2-INIT] === INICJALIZACJA SPRZĘTU ===");
    
    // 1. Inicjalizuj RTC (czas jest krytyczny)
    initializeRTC();
    
    // 2. Inicjalizuj kartę SD (logowanie)
    initializeSDLogger();
    
    // 3. Inicjalizuj EEPROM i ustawienia
    initializeSettings();
    
    // 4. Inicjalizuj sprzęt pompy i czujników
    initializePumpController();
    initializeWaterSensor();
    
    Serial.println("[PICO2-INIT] Sprzęt zainicjalizowany");
    Serial.println();
    
    // ================= INICJALIZACJA KOMUNIKACJI =================
    Serial.println("[PICO2-INIT] === INICJALIZACJA KOMUNIKACJI ===");
    
    // 5. Inicjalizuj protokół UART
    initializeUARTProtocol();
    
    Serial.println("[PICO2-INIT] Komunikacja zainicjalizowana");
    Serial.println();
    
    // ================= INICJALIZACJA SYSTEMU LOGIKI =================
    Serial.println("[PICO2-INIT] === INICJALIZACJA SYSTEMU ===");
    
    // 6. Inicjalizuj główny system dolewania wody
    initializeWaterSystem();
    
    systemFullyInitialized = true;
    
    // ================= PODSUMOWANIE INICJALIZACJI =================
    Serial.println("[PICO2-READY] ✅ SYSTEM GOTOWY DO PRACY");
    Serial.println();
    printSystemSummary();
    
    // Test funkcjonalności (opcjonalnie)
    #if ENABLE_DEBUG_LOGS
    performStartupTests();
    #endif
    
    Serial.println("[PICO2-MAIN-LOOP] Rozpoczynam główną pętlę systemu");
    Serial.println(String("=").substring(0, 50) + "\n");
}

// ================= LOOP =================
void loop() {
    if (!systemFullyInitialized) {
        delay(1000);
        return;
    }
    
    // ================= GŁÓWNA PĘTLA SYSTEMU =================
    
    // 1. Przetwarzaj komunikację UART z WebServerem
    processUARTProtocol();
    
    // 2. Przetwarzaj logikę systemu dolewania wody
    processWaterSystem();
    
    // 3. Konserwacja systemu (co 5 minut)
    if (millis() - lastSystemMaintenance > 300000) {  // 5 minut
        performSystemMaintenance();
        lastSystemMaintenance = millis();
    }
    
    // 4. Wydruk statusu (co 30 sekund)
    if (millis() - lastStatusPrint > 30000) {  // 30 sekund
        printSystemStatus();
        lastStatusPrint = millis();
    }
    
    // 5. Krótkie opóźnienie dla stabilności
    delay(MAIN_LOOP_DELAY);
}

// ================= FUNKCJE POMOCNICZE =================

/**
 * Wydrukuj podsumowanie systemu po inicjalizacji
 */
void printSystemSummary() {
    Serial.println("[PICO2-SUMMARY] === PODSUMOWANIE SYSTEMU ===");
    
    // Status sprzętu
    Serial.printf("[PICO2-SUMMARY] RTC DS3231M: %s\n", isRTCWorking() ? "✅ Działa" : "❌ Błąd");
    Serial.printf("[PICO2-SUMMARY] Karta SD: %s\n", isSDWorking() ? "✅ Działa" : "❌ Błąd");
    Serial.printf("[PICO2-SUMMARY] Czujnik wody: %s\n", isWaterSensorWorking() ? "✅ Działa" : "❌ Błąd");
    
    // Konfiguracja pompy
    PumpConfig config = getPumpConfig();
    Serial.printf("[PICO2-SUMMARY] Konfiguracja pompy: %dml w %ds\n", config.volumeML, config.pumpTimeSeconds);
    
    // Status komunikacji
    Serial.printf("[PICO2-SUMMARY] UART: Pin TX=GP%d, RX=GP%d, Baud=%d\n", UART_TX_PIN, UART_RX_PIN, UART_BAUD_RATE);
    
    // Aktualny czas
    Serial.printf("[PICO2-SUMMARY] Aktualny czas: %s\n", getCurrentTimeString().c_str());
    
    // Stan systemu
    WaterSystemStatus status = getWaterSystemStatus();
    Serial.printf("[PICO2-SUMMARY] Poziom wody: %s\n", 
                  status.waterLevel == WATER_LEVEL_LOW ? "NISKI" : "OK");
    Serial.printf("[PICO2-SUMMARY] Łączne zdarzenia: %lu\n", status.totalEvents);
    
    Serial.println("[PICO2-SUMMARY] ============================");
    Serial.println();
}

/**
 * Wydrukuj bieżący status systemu
 */
void printSystemStatus() {
    WaterSystemStatus status = getWaterSystemStatus();
    bool webServerConn = isWebServerConnected();
    
    // Krótki status w jednej linii
    Serial.printf("[PICO2-STATUS] Water:%s Pump:%s WebServer:%s Time:%s Events:%lu\n",
                  status.waterLevel == WATER_LEVEL_LOW ? "LOW" : "OK",
                  status.pumpActive ? "ON" : "OFF",
                  webServerConn ? "CONN" : "DISC",
                  getCurrentTimeOnlyString().c_str(),
                  status.totalEvents);
    
    // Dodatkowe info jeśli są problemy
    if (!isRTCWorking()) {
        Serial.println("[PICO2-STATUS-WARNING] ⚠️ RTC nie działa prawidłowo");
    }
    if (!isSDWorking()) {
        Serial.println("[PICO2-STATUS-WARNING] ⚠️ Karta SD niedostępna");
    }
    if (!webServerConn) {
        Serial.println("[PICO2-STATUS-WARNING] ⚠️ Brak połączenia z WebServerem");
    }
}

/**
 * Konserwacja systemu
 */
void performSystemMaintenance() {
    Serial.println("[PICO2-MAINTENANCE] Rozpoczynam konserwację systemu");
    
    // 1. Sprawdź stan pamięci (Pico2 ma więcej RAM)
    uint32_t freeHeap = rp2040.getFreeHeap();
    Serial.printf("[PICO2-MAINTENANCE] Wolna pamięć: %u bajtów\n", freeHeap);
    
    if (freeHeap < 50000) {  // Wyższy próg dla Pico2 (520KB RAM)
        Serial.println("[PICO2-MAINTENANCE] ⚠️ Niska ilość wolnej pamięci!");
    }
    
    // 2. Sprawdź uptime
    unsigned long uptimeMinutes = millis() / 60000;
    Serial.printf("[PICO2-MAINTENANCE] Uptime: %lu minut\n", uptimeMinutes);
    
    // 3. Sprawdź temperaturę RTC
    if (isRTCWorking()) {
        float rtcTemp = getRTCTemperature();
        Serial.printf("[PICO2-MAINTENANCE] Temperatura RTC: %.1f°C\n", rtcTemp);
    }
    
    // 4. Sprawdź status logów
    String logInfo = getLogFileInfo();
    Serial.println("[PICO2-MAINTENANCE] Status logów: " + logInfo);
    
    // 5. Czyszczenie starych danych (jeśli potrzeba)
    // TODO: W przyszłości można dodać rotację logów
    
    Serial.println("[PICO2-MAINTENANCE] Konserwacja zakończona");
    Serial.println();
}

/**
 * Testy startowe systemu (debug)
 */
void performStartupTests() {
    Serial.println("[PICO2-TEST] === TESTY STARTOWE ===");
    
    // Test 1: Test czujnika wody
    Serial.println("[PICO2-TEST] Test czujnika poziomu wody:");
    debugWaterSensor();
    
    // Test 2: Test RTC
    Serial.println("[PICO2-TEST] Test RTC DS3231M:");
    debugRTC();
    
    // Test 3: Test karty SD
    Serial.println("[PICO2-TEST] Test karty SD:");
    debugSDLogger();
    
    // Test 4: Test ustawień
    Serial.println("[PICO2-TEST] Test ustawień EEPROM:");
    debugSettings();
    
    // Test 5: Test protokołu UART
    Serial.println("[PICO2-TEST] Test protokołu UART:");
    debugUARTProtocol();
    
    // Test 6: Krótki test pompy (TYLKO JEŚLI BEZPIECZNE)
    /*
    Serial.println("[PICO2-TEST] Test pompy (2 sekundy):");
    if (testPump()) {
        Serial.println("[PICO2-TEST] ✅ Test pompy zakończony sukcesem");
    } else {
        Serial.println("[PICO2-TEST] ❌ Test pompy nieudany");
    }
    */
    
    Serial.println("[PICO2-TEST] === TESTY ZAKOŃCZONE ===");
    Serial.println();
}

/**
 * Debug - wydrukuj pełny status wszystkich systemów
 */
void printFullSystemDebug() {
    Serial.println("\n[PICO2-DEBUG] === PEŁNY DEBUG SYSTEMU ===");
    
    debugWaterSensor();
    debugPumpController();
    debugRTC();
    debugSDLogger();
    debugSettings();
    debugUARTProtocol();
    
    Serial.println("[PICO2-DEBUG] === DEBUG ZAKOŃCZONY ===\n");
}

/**
 * Obsługa błędów krytycznych
 */
void handleCriticalError(const String& error) {
    Serial.println("[PICO2-CRITICAL-ERROR] ⚠️⚠️⚠️ BŁĄD KRYTYCZNY ⚠️⚠️⚠️");
    Serial.println("[PICO2-CRITICAL-ERROR] " + error);
    Serial.println("[PICO2-CRITICAL-ERROR] System przechodzi w tryb awaryjny");
    
    // Wyłącz pompę dla bezpieczeństwa
    stopPump();
    
    // Migaj LED lub inny sygnał błędu
    while (true) {
        Serial.println("[PICO2-EMERGENCY] System w trybie awaryjnym - " + error);
        delay(5000);
    }
}












/**
 * PICO2 HARDWARE SCANNER
 * 
 * Test I2C i SPI połączeń
 * Zastąp tym src/main.cpp i upload
 */

// #include <Arduino.h>
// #include <Wire.h>
// #include <SPI.h>
// #include <SD.h>

// // Piny zgodnie z config
// #define RTC_SDA_PIN 4   // GP4
// #define RTC_SCL_PIN 5   // GP5
// #define SD_CS_PIN   17  // GP17
// #define SD_MOSI_PIN 19  // GP19
// #define SD_MISO_PIN 16  // GP16
// #define SD_CLK_PIN  18  // GP18
// #define LED  25 

// // ================= DEKLARACJE FUNKCJI =================
// void testI2C();
// void testSPI();
// void testGPIO();

// void setup() {
//     Serial.begin(115200);
//     pinMode(LED, OUTPUT);
//     while (!Serial && millis() < 3000) delay(10);
    
//     Serial.println("\n" + String("=").substring(0, 50));
//     Serial.println("🔍 PICO2 HARDWARE SCANNER");
//     Serial.println("Testing I2C (RTC) and SPI (SD) connections");
//     Serial.println(String("=").substring(0, 50));
//     Serial.println();
    
//     testI2C();
//     Serial.println();
//     testSPI();
//     Serial.println();
//     testGPIO();
//     Serial.println();
    
//     Serial.println("🎯 HARDWARE SCAN COMPLETE");
//     Serial.println("Send 'i' for I2C scan, 's' for SPI test, 'g' for GPIO test");
// }

// void loop() {
//     if (Serial.available()) {
//         char cmd = Serial.read();
//         Serial.println();
        
//         switch (cmd) {
//             case 'i':
//             case 'I':
//                 testI2C();
//                 break;
//             case 's':
//             case 'S':
//                 testSPI();
//                 break;
//             case 'g':
//             case 'G':
//                 testGPIO();
//                 break;
//             case 'h':
//             case 'H':
//                 Serial.println("Commands: i=I2C scan, s=SPI test, g=GPIO test, h=help");
//                 break;
//         }
//         Serial.println();
//     }

//     digitalWrite(LED, HIGH);
//     delay(100);
//     digitalWrite(LED, LOW);
//     delay(100);


// }

// void testI2C() {
//     Serial.println("🔍 === I2C SCAN TEST ===");
//     Serial.printf("I2C pins: SDA=GP%d, SCL=GP%d\n", RTC_SDA_PIN, RTC_SCL_PIN);
    
//     // Konfiguruj I2C
//     Wire.setSDA(RTC_SDA_PIN);
//     Wire.setSCL(RTC_SCL_PIN);
//     Wire.begin();
    
//     Serial.println("Scanning I2C addresses 0x01-0x7F...");
    
//     int devices = 0;
//     for (byte addr = 1; addr < 127; addr++) {
//         Wire.beginTransmission(addr);
//         byte error = Wire.endTransmission();
        
//         if (error == 0) {
//             Serial.printf("✅ I2C device found at address 0x%02X", addr);
            
//             // Identify known devices
//             if (addr == 0x68) {
//                 Serial.print(" (DS3231 RTC)");
//             } else if (addr == 0x50) {
//                 Serial.print(" (EEPROM)");
//             } else if (addr == 0x3C || addr == 0x3D) {
//                 Serial.print(" (OLED Display)");
//             }
//             Serial.println();
//             devices++;
//         }
//         else if (error == 4) {
//             Serial.printf("❌ Unknown error at address 0x%02X\n", addr);
//         }
//     }
    
//     if (devices == 0) {
//         Serial.println("❌ NO I2C DEVICES FOUND!");
//         Serial.println("   Check connections:");
//         Serial.printf("   - VCC+ → Pico2 3V3\n");
//         Serial.printf("   - GND- → Pico2 GND\n");
//         Serial.printf("   - SDA → Pico2 GP%d\n", RTC_SDA_PIN);
//         Serial.printf("   - SCL → Pico2 GP%d\n", RTC_SCL_PIN);
//         Serial.println("   - Pull-up resistors (if needed)");
//         Serial.println("   - DS3231M should appear at 0x68");
//     } else {
//         Serial.printf("✅ Found %d I2C device(s)\n", devices);
//     }
// }

// void testSPI() {
//     Serial.println("💾 === SPI/SD CARD TEST ===");
//     Serial.printf("SPI pins: MISO=GP%d, MOSI=GP%d, CLK=GP%d, CS=GP%d\n", 
//                   SD_MISO_PIN, SD_MOSI_PIN, SD_CLK_PIN, SD_CS_PIN);
    
//     // Konfiguruj SPI
//     SPI.setRX(SD_MISO_PIN);
//     SPI.setTX(SD_MOSI_PIN);
//     SPI.setSCK(SD_CLK_PIN);
    
//     Serial.println("Configuring CS pin...");
//     pinMode(SD_CS_PIN, OUTPUT);
//     digitalWrite(SD_CS_PIN, HIGH);
    
//     Serial.println("Attempting SD card initialization...");
    
//     if (SD.begin(SD_CS_PIN)) {
//         Serial.println("✅ SD CARD INITIALIZED SUCCESSFULLY!");
        
//         // Test basic operations
//         Serial.println("Testing file operations...");
        
//         File testFile = SD.open("/test.txt", FILE_WRITE);
//         if (testFile) {
//             testFile.println("Pico2 hardware test");
//             testFile.printf("Timestamp: %lu\n", millis());
//             testFile.close();
//             Serial.println("✅ File write test OK");
            
//             // Read test
//             testFile = SD.open("/test.txt");
//             if (testFile) {
//                 Serial.println("✅ File read test:");
//                 while (testFile.available()) {
//                     Serial.print("   ");
//                     Serial.print(testFile.readStringUntil('\n'));
//                 }
//                 testFile.close();
                
//                 // Cleanup
//                 SD.remove("/test.txt");
//                 Serial.println("✅ File cleanup OK");
//             }
//         } else {
//             Serial.println("❌ File write test FAILED");
//         }
        
//     } else {
//         Serial.println("❌ SD CARD INITIALIZATION FAILED!");
//         Serial.println("   Check:");
//         Serial.println("   - SD card inserted");
//         Serial.println("   - SD card formatted as FAT32");
//         Serial.println("   - SD card ≤ 32GB");
//         Serial.println("   - Connections:");
//         Serial.printf("     3V  → Pico2 3V3\n");
//         Serial.printf("     GND → Pico2 GND\n");
//         Serial.printf("     CLK → Pico2 GP%d\n", SD_CLK_PIN);
//         Serial.printf("     DO  → Pico2 GP%d (MISO)\n", SD_MISO_PIN);
//         Serial.printf("     DI  → Pico2 GP%d (MOSI)\n", SD_MOSI_PIN);
//         Serial.printf("     CS  → Pico2 GP%d\n", SD_CS_PIN);
//     }
// }

// void testGPIO() {
//     Serial.println("⚡ === GPIO TEST ===");
    
//     // Test water sensor pin
//     pinMode(15, INPUT_PULLUP);  // GP15 - water sensor
//     bool waterSensor = digitalRead(15);
//     Serial.printf("Water sensor (GP15): %s (raw: %s)\n", 
//                   waterSensor ? "WATER OK" : "WATER LOW",
//                   waterSensor ? "HIGH" : "LOW");
    
//     // Test pump relay pin  
//     pinMode(14, OUTPUT);  // GP14 - pump relay
//     Serial.println("Testing pump relay (GP14)...");
    
//     digitalWrite(14, LOW);
//     Serial.println("  Pump relay: OFF (LOW)");
//     delay(500);
    
//     digitalWrite(14, HIGH);
//     Serial.println("  Pump relay: ON (HIGH)");
//     delay(500);
    
//     digitalWrite(14, LOW);
//     Serial.println("  Pump relay: OFF (LOW) - test complete");
    
//     // Test built-in LED if available
//     #ifdef LED_BUILTIN
//     pinMode(LED_BUILTIN, OUTPUT);
//     Serial.println("Testing built-in LED...");
//     for (int i = 0; i < 3; i++) {
//         digitalWrite(LED_BUILTIN, HIGH);
//         delay(200);
//         digitalWrite(LED_BUILTIN, LOW);
//         delay(200);
//     }
//     Serial.println("LED test complete");
//     #endif
    
//     Serial.println("✅ GPIO test complete");
// }
