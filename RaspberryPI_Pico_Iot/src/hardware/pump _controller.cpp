#include "pump_controller.h"
#include "../config.h"

// ================= ZMIENNE GLOBALNE =================
bool pumpRunning = false;
unsigned long pumpStartTime = 0;
unsigned long pumpEndTime = 0;
int lastPumpDuration = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja kontrolera pompy
 */
void initializePumpController() {
    pinMode(PUMP_RELAY_PIN, OUTPUT);
    digitalWrite(PUMP_RELAY_PIN, LOW);  // Pompa wyłączona
    
    pumpRunning = false;
    pumpStartTime = 0;
    pumpEndTime = 0;
    lastPumpDuration = 0;
    
    Serial.println("[PUMP-CONTROLLER-INIT] Inicjalizacja kontrolera pompy");
    Serial.printf("[PUMP-CONTROLLER-CONFIG] Pin przekaźnika: %d\n", PUMP_RELAY_PIN);
    Serial.println("[PUMP-CONTROLLER-LOGIC] HIGH = pompa ON, LOW = pompa OFF");
    Serial.println("[PUMP-CONTROLLER-STATUS] Pompa wyłączona (stan początkowy)");
    Serial.println();
}

/**
 * Uruchom pompę na określony czas (blokujące)
 */
bool runPump(int durationSeconds) {
    if (pumpRunning) {
        Serial.println("[PUMP-CONTROLLER-ERROR] Pompa już działa - nie można uruchomić ponownie");
        return false;
    }
    
    if (durationSeconds <= 0) {
        Serial.println("[PUMP-CONTROLLER-ERROR] Nieprawidłowy czas pracy pompy");
        return false;
    }
    
    Serial.printf("[PUMP-CONTROLLER-START] Uruchamianie pompy na %d sekund\n", durationSeconds);
    
    // Uruchom pompę
    pumpRunning = true;
    pumpStartTime = millis();
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    
    Serial.println("[PUMP-CONTROLLER-ACTIVE] Pompa WŁĄCZONA");
    
    // Oczekaj określony czas (blokujące - OK dla tego zastosowania)
    delay(durationSeconds * 1000);
    
    // Wyłącz pompę
    digitalWrite(PUMP_RELAY_PIN, LOW);
    pumpEndTime = millis();
    pumpRunning = false;
    lastPumpDuration = durationSeconds;
    
    Serial.println("[PUMP-CONTROLLER-STOP] Pompa WYŁĄCZONA");
    Serial.printf("[PUMP-CONTROLLER-COMPLETED] Czas pracy: %d sekund\n", durationSeconds);
    Serial.println();
    
    return true;
}

/**
 * Natychmiast zatrzymaj pompę (funkcja bezpieczeństwa)
 */
void stopPump() {
    if (pumpRunning) {
        digitalWrite(PUMP_RELAY_PIN, LOW);
        pumpEndTime = millis();
        pumpRunning = false;
        
        int actualDuration = (pumpEndTime - pumpStartTime) / 1000;
        lastPumpDuration = actualDuration;
        
        Serial.println("[PUMP-CONTROLLER-EMERGENCY-STOP] Pompa zatrzymana awaryjnie");
        Serial.printf("[PUMP-CONTROLLER-ACTUAL-TIME] Rzeczywisty czas pracy: %d sekund\n", actualDuration);
        Serial.println();
    }
}

/**
 * Sprawdź czy pompa obecnie działa
 */
bool isPumpRunning() {
    return pumpRunning;
}

/**
 * Pobierz czas od uruchomienia pompy
 */
unsigned long getPumpRunTime() {
    if (pumpRunning) {
        return (millis() - pumpStartTime) / 1000;  // w sekundach
    } else {
        return 0;
    }
}

/**
 * Pobierz czas ostatniego uruchomienia pompy
 */
unsigned long getLastPumpStartTime() {
    return pumpStartTime;
}

/**
 * Pobierz czas ostatniego zatrzymania pompy
 */
unsigned long getLastPumpEndTime() {
    return pumpEndTime;
}

/**
 * Pobierz czas trwania ostatniego cyklu pompy
 */
int getLastPumpDuration() {
    return lastPumpDuration;
}

/**
 * Test pompy - krótkie uruchomienie
 */
bool testPump() {
    Serial.println("[PUMP-CONTROLLER-TEST] Rozpoczynam test pompy (2 sekundy)");
    
    bool result = runPump(2);  // Test na 2 sekundy
    
    if (result) {
        Serial.println("[PUMP-CONTROLLER-TEST] Test pompy ZAKOŃCZONY SUKCESEM");
    } else {
        Serial.println("[PUMP-CONTROLLER-TEST] Test pompy NIEUDANY");
    }
    
    return result;
}

/**
 * Status pompy jako string
 */
String getPumpStatusString() {
    if (pumpRunning) {
        unsigned long runTime = getPumpRunTime();
        return "RUNNING (" + String(runTime) + "s)";
    } else {
        return "STOPPED";
    }
}

/**
 * Debug - wyświetl pełny status pompy
 */
void debugPumpController() {
    bool pinState = digitalRead(PUMP_RELAY_PIN);
    
    Serial.println("[PUMP-CONTROLLER-DEBUG] === STATUS POMPY ===");
    Serial.printf("[PUMP-CONTROLLER-DEBUG] Pin %d stan: %s\n", 
                  PUMP_RELAY_PIN, pinState ? "HIGH (ON)" : "LOW (OFF)");
    Serial.printf("[PUMP-CONTROLLER-DEBUG] Status wewnętrzny: %s\n", 
                  pumpRunning ? "DZIAŁA" : "ZATRZYMANA");
    Serial.printf("[PUMP-CONTROLLER-DEBUG] Obecny czas pracy: %lu sekund\n", 
                  getPumpRunTime());
    Serial.printf("[PUMP-CONTROLLER-DEBUG] Ostatni czas pracy: %d sekund\n", 
                  lastPumpDuration);
    
    if (pumpStartTime > 0) {
        Serial.printf("[PUMP-CONTROLLER-DEBUG] Ostatnie uruchomienie: %lu ms temu\n", 
                      millis() - pumpStartTime);
    }
    if (pumpEndTime > 0) {
        Serial.printf("[PUMP-CONTROLLER-DEBUG] Ostatnie zatrzymanie: %lu ms temu\n", 
                      millis() - pumpEndTime);
    }
    
    Serial.println("[PUMP-CONTROLLER-DEBUG] ====================");
    Serial.println();
}