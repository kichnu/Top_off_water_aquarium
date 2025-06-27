/**
 * ESP32 Smart Home - Główny plik Arduino
 * 
 * Modularny system sterowania domem inteligentnym z interfejsem web
 * Funkcje: LED, przekaźniki, serwomechanizmy, sensory, harmonogramy
 * Bezpieczeństwo: Rate limiting, sesje, IP whitelist
 * Komunikacja: HTTP, WiFi, NTP
 * 
 * Autor: ESP32 Smart Home Project
 * Wersja: 2.1 Modular
 */


// ================= IMPORTY MODUŁÓW =================
#include "config.h"
#include "core/system.h"
#include "core/logging.h"
#include "network/wifi.h"
#include "network/ntp.h"
#include "security/auth.h"
#include "security/sessions.h"
#include "security/ratelimit.h"
#include "web/server.h"
#include "hardware/actuators.h"

// *** ROZSZERZENIE: Dodaj tutaj importy dla nowych modułów ***
// #include "src/hardware/sensors.h"
// #include "src/automation/scheduler.h"

// ================= SETUP =================
void setup() {
    // Inicjalizacja core systemów
    initializeLogging();
    initializeSystem();
    
    // Inicjalizacja hardware
    initializeActuators();
    
    // *** ROZSZERZENIE: Dodaj tutaj inicjalizację nowych modułów hardware ***
    // initializeSensors();
    
    // Inicjalizacja network
    initializeWiFi();
    // NTP zostanie zainicjalizowane automatycznie po połączeniu WiFi
    
    // Inicjalizacja security
    initializeSessions();
    initializeRateLimit();
    
    // Inicjalizacja web services
    initializeWebServer();
    startWebServer();
    
    // *** ROZSZERZENIE: Dodaj tutaj inicjalizację automatyki ***
    // initializeScheduler();
    // initializeAutomationRules();
    
    Serial.println("\n[SYSTEM] ✅ Inicjalizacja zakończona pomyślnie!");
    Serial.println("[SYSTEM] 🔐 Hasło administratora: " + String(ADMIN_PASSWORD));
    Serial.println("[SYSTEM] 🛡️ Zabezpieczenia: Sessions + Rate limiting");
    Serial.printf("[SYSTEM] 🕒 Timeout sesji: %lu minut\n", SESSION_TIMEOUT / 60000);
    Serial.println(String("=").substring(0, 50) + "\n");
}

// ================= LOOP =================
void loop() {
    // Sprawdź połączenie sieciowe
    checkWiFiConnection();
    
    // Konserwacja systemów
    systemMaintenance();
    
    // *** ROZSZERZENIE: Dodaj tutaj obsługę automatyki i harmonogramów ***
    /*
    // Sprawdzaj harmonogramy
    processSchedules();
    
    // Obsługa reguł automatyki
    processAutomationRules();
    
    // Odczytuj sensory co 10 sekund
    static unsigned long lastSensorRead = 0;
    if (millis() - lastSensorRead > 10000) {
        lastSensorRead = millis();
        readAllSensors();
        
        // Przykład automatyki: włącz światło gdy ciemno i wykryty ruch
        if (lightLevel < 100 && motionDetected && !getRelayState(1)) {
            setRelayState(1, true);
            LOG_INFO_MSG("AUTO", "Włączono światło - wykryty ruch w ciemności");
        }
        
        // Automatyczne wyłączenie światła po 5 minutach bez ruchu
        static unsigned long lastMotion = 0;
        if (motionDetected) {
            lastMotion = millis();
        } else if (getRelayState(1) && (millis() - lastMotion > 300000)) {
            setRelayState(1, false);
            LOG_INFO_MSG("AUTO", "Wyłączono światło - brak ruchu przez 5 minut");
        }
    }
    */
    
    // Czyszczenie pamięci co 5 minut
    static unsigned long lastCleanup = 0;
    if (millis() - lastCleanup > 300000) {
        lastCleanup = millis();
        
        cleanupExpiredSessions();
        cleanupOldClients();
        
        // Pokaż aktywne sesje dla debugowania
        int activeSessions = getActiveSessionCount();
        if (activeSessions > 0) {
            Serial.printf("[SYSTEM] Aktywne sesje: %d\n", activeSessions);
        }
    }
    
    delay(100);
}

// ================= DODATKOWE FUNKCJE POMOCNICZE =================

/**
 * Funkcja wywoływana przy watchdog reset lub crash
 */
void onSystemCrash() {
    Serial.println("[SYSTEM] ❌ Wykryto crash systemu!");
    Serial.println("[SYSTEM] Restart za 5 sekund...");
    delay(5000);
    ESP.restart();
}

/**
 * Funkcja wywoływana przy niskim poziomie pamięci
 */
void onLowMemory() {
    Serial.println("[SYSTEM] ⚠️ Niski poziom pamięci!");
    
    // Wyczyść stare sesje i klientów
    cleanupExpiredSessions();
    cleanupOldClients();
    
    // Wyświetl status pamięci
    Serial.printf("[SYSTEM] Wolna pamięć po czyszczeniu: %d bytes\n", ESP.getFreeHeap());
}

// *** ROZSZERZENIE: Dodaj tutaj dodatkowe funkcje pomocnicze ***
/*
// Funkcja wykonywana przy harmonogramie
void executeScheduledAction(String action) {
    if (action == "led_on") setLEDState(true);
    else if (action == "led_off") setLEDState(false);
    else if (action == "relay1_on") setRelayState(1, true);
    else if (action == "relay1_off") setRelayState(1, false);
    else if (action == "relay2_on") setRelayState(2, true);
    else if (action == "relay2_off") setRelayState(2, false);
    else if (action == "relay3_on") setRelayState(3, true);
    else if (action == "relay3_off") setRelayState(3, false);
    // Dodaj więcej akcji według potrzeb...
    
    LOG_INFO_MSG("SCHEDULE", ("Wykonano akcję: " + action).c_str());
}

// Funkcja sprawdzająca warunki automatyki
bool checkAutomationCondition(String condition) {
    if (condition == "motion_detected") return motionDetected;
    else if (condition == "door_open") return doorOpen;
    else if (condition == "light_low") return lightLevel < 100;
    else if (condition == "temp_high") return temperature > 25.0;
    else if (condition == "temp_low") return temperature < 20.0;
    // Dodaj więcej warunków według potrzeb...
    
    return false;
}

// Funkcja obsługi sygnałów alarmowych
void handleAlarmCondition(String alarm) {
    if (alarm == "fire") {
        // Włącz wszystkie światła, wyłącz ogrzewanie
        setRelayState(1, true); // Światło
        setRelayState(2, false); // Ogrzewanie
        LOG_ERROR_MSG("ALARM", "Wykryto pożar!");
    }
    else if (alarm == "intrusion") {
        // Włącz światła, rozpocznij nagrywanie
        setRelayState(1, true);
        LOG_ERROR_MSG("ALARM", "Wykryto włamanie!");
    }
}
*/