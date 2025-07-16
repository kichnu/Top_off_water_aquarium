#include "water_system.h"
#include "../hardware/water_sensor.h"
#include "../hardware/pump_controller.h"
#include "../hardware/sd_logger.h"
#include "../hardware/rtc_ds3231.h"
#include "../config/settings.h"
#include "../config.h"

// ================= ZMIENNE GLOBALNE =================
WaterSystemStatus systemStatus;
unsigned long lastSensorCheck = 0;
bool sensorFirstCheck = false;
unsigned long sensorFirstCheckTime = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja systemu dolewania wody
 */
void initializeWaterSystem() {
    Serial.println("[WATER-SYSTEM-INIT] Inicjalizacja systemu dolewania wody");
    
    // Wyzeruj status
    systemStatus.waterLevel = WATER_LEVEL_UNKNOWN;
    systemStatus.pumpActive = false;
    systemStatus.lastPumpTime = "";
    systemStatus.eventsToday = 0;
    systemStatus.totalEvents = 0;
    systemStatus.systemReady = false;
    
    // Inicjalizuj komponenty
    initializeWaterSensor();
    initializePumpController();
    initializeSDLogger();
    initializeRTC();
    loadSystemSettings();
    
    // Sprawdź początkowy stan czujnika
    systemStatus.waterLevel = readWaterLevel();
    systemStatus.systemReady = true;
    
    Serial.println("[WATER-SYSTEM-INIT] System gotowy do pracy");
    Serial.printf("[WATER-SYSTEM-STATUS] Poziom wody: %s\n", 
                  systemStatus.waterLevel == WATER_LEVEL_LOW ? "NISKI" : 
                  systemStatus.waterLevel == WATER_LEVEL_OK ? "OK" : "NIEZNANY");
    Serial.println();
}

/**
 * 🚀 GŁÓWNA FUNKCJA DO PÓŹNIEJSZEGO ROZWOJU
 * 
 * Sprawdza czy pompa może zostać uruchomiona (automatycznie przez czujnik)
 * 
 * Możliwe przyszłe implementacje:
 * - Limit dziennej objętości wody
 * - Harmonogram czasowy (np. nie pompuj w nocy 22:00-06:00)  
 * - Zabezpieczenie przed zbyt częstym pompowaniem (min. przerwa)
 * - Status innych systemów (temperatura, ciśnienie, awarie)
 * - Tryb serwisowy/wakacyjny (całkowite wyłączenie)
 * - Sezonowe ustawienia (więcej wody latem)
 * - Integracja z czujnikami pogody
 * - Monitoring zużycia energii pompy
 * - Detekcja awarii pompy lub czujników
 * 
 * @return true = pompa może zostać uruchomiona automatycznie
 * @return false = pompa zablokowana (powód w logach)
 */
bool check_permission() {
    // ================= WERSJA 1.0: ZAWSZE POZWÓL =================
    // Dla pierwszej implementacji - brak ograniczeń
    
    #if ENABLE_DEBUG_LOGS
    Serial.println("[WATER-PERMISSION-CHECK] Sprawdzanie uprawnień pompy");
    Serial.println("[WATER-PERMISSION-RESULT] POZWOLENIE UDZIELONE (wersja 1.0)");
    Serial.println();
    #endif
    
    return true;
    
    // ================= TODO: PRZYSZŁE IMPLEMENTACJE =================
    
    /*
    // Przykład przyszłej implementacji:
    
    // 1. Sprawdź limit dzienny
    if (systemStatus.eventsToday >= getDailyLimit()) {
        Serial.println("[WATER-PERMISSION-DENIED] Przekroczony limit dzienny");
        return false;
    }
    
    // 2. Sprawdź harmonogram czasowy  
    String currentTime = getCurrentTime();
    if (isNightTime(currentTime)) {
        Serial.println("[WATER-PERMISSION-DENIED] Tryb nocny aktywny");
        return false;
    }
    
    // 3. Sprawdź minimalną przerwę między pompowaniami
    if (getTimeSinceLastPump() < getMinimumBreakTime()) {
        Serial.println("[WATER-PERMISSION-DENIED] Za krótka przerwa od ostatniego pompowania");
        return false;
    }
    
    // 4. Sprawdź tryb serwisowy
    if (isMaintenanceMode()) {
        Serial.println("[WATER-PERMISSION-DENIED] Tryb serwisowy aktywny");
        return false;
    }
    
    // 5. Sprawdź status innych systemów
    if (!areOtherSystemsOK()) {
        Serial.println("[WATER-PERMISSION-DENIED] Problemy z innymi systemami");
        return false;
    }
    
    return true;
    */
}

/**
 * Główna pętla systemu dolewania wody
 */
void processWaterSystem() {
    if (!systemStatus.systemReady) {
        return;
    }
    
    // Sprawdź czujnik poziomu wody z debouncingiem
    checkWaterSensorWithDebouncing();
    
    // Sprawdź status pompy
    updatePumpStatus();
}

/**
 * Sprawdź czujnik wody z prostym debouncingiem przez delay
 */
void checkWaterSensorWithDebouncing() {
    WaterLevel currentLevel = readWaterLevel();
    
    // Jeśli poziom się zmienił na LOW (niski poziom wody)
    if (currentLevel == WATER_LEVEL_LOW && systemStatus.waterLevel != WATER_LEVEL_LOW) {
        
        Serial.println("[WATER-SENSOR-DEBOUNCE] Wykryto niski poziom wody - rozpoczynam debouncing");
        
        // Proste debouncing przez delay (OK dla rzadkich zdarzeń)
        delay(DEBOUNCE_DELAY_MS);
        
        // Sprawdź ponownie po delay
        WaterLevel confirmedLevel = readWaterLevel();
        
        if (confirmedLevel == WATER_LEVEL_LOW) {
            Serial.println("[WATER-SENSOR-CONFIRMED] Niski poziom wody potwierdzony po debouncing");
            
            // Zaktualizuj status
            systemStatus.waterLevel = WATER_LEVEL_LOW;
            
            // Sprawdź uprawnienia i uruchom pompę
            if (check_permission()) {
                triggerAutomaticPump();
            } else {
                Serial.println("[WATER-SYSTEM-BLOCKED] Pompa zablokowana przez check_permission()");
                Serial.println();
            }
        } else {
            Serial.println("[WATER-SENSOR-FALSE-ALARM] False alarm - poziom wody OK po debouncing");
            Serial.println();
        }
    } else if (currentLevel == WATER_LEVEL_OK && systemStatus.waterLevel != WATER_LEVEL_OK) {
        // Poziom wody wrócił do normy
        systemStatus.waterLevel = WATER_LEVEL_OK;
        Serial.println("[WATER-SENSOR-STATUS] Poziom wody powrócił do normy");
        Serial.println();
    }
}

/**
 * Uruchom pompę automatycznie (przez czujnik)
 */
void triggerAutomaticPump() {
    Serial.println("[WATER-PUMP-AUTO] Uruchamianie pompy automatycznie");
    
    PumpConfig config = getPumpConfig();
    String timestamp = getCurrentTimeString();
    
    // Uruchom pompę
    bool pumpResult = runPump(config.pumpTimeSeconds);
    
    if (pumpResult) {
        // Aktualizuj status systemu
        systemStatus.lastPumpTime = timestamp;
        systemStatus.eventsToday++;
        systemStatus.totalEvents++;
        systemStatus.pumpActive = false; // Pompa zakończyła pracę
        
        // Zapisz zdarzenie do loga
        logPumpEvent("AUTO", config.volumeML, timestamp);
        
        Serial.printf("[WATER-PUMP-AUTO-SUCCESS] Pompa zakończona - %dml w %ds\n", 
                     config.volumeML, config.pumpTimeSeconds);
        Serial.println();
    } else {
        Serial.println("[WATER-PUMP-AUTO-ERROR] Błąd podczas pracy pompy");
        Serial.println();
    }
}

/**
 * Uruchom pompę ręcznie (z WebServera)
 */
bool triggerManualPump() {
    Serial.println("[WATER-PUMP-MANUAL] Uruchamianie pompy ręcznie z WebServera");
    
    PumpConfig config = getPumpConfig();
    String timestamp = getCurrentTimeString();
    
    // Ręczne uruchomienie - POMIJA check_permission()
    bool pumpResult = runPump(config.pumpTimeSeconds);
    
    if (pumpResult) {
        // Aktualizuj status systemu
        systemStatus.lastPumpTime = timestamp;
        systemStatus.eventsToday++;
        systemStatus.totalEvents++;
        systemStatus.pumpActive = false;
        
        // Zapisz zdarzenie do loga
        logPumpEvent("MANUAL", config.volumeML, timestamp);
        
        Serial.printf("[WATER-PUMP-MANUAL-SUCCESS] Pompa ręczna zakończona - %dml w %ds\n", 
                     config.volumeML, config.pumpTimeSeconds);
        Serial.println();
        
        return true;
    } else {
        Serial.println("[WATER-PUMP-MANUAL-ERROR] Błąd podczas ręcznej pracy pompy");
        Serial.println();
        return false;
    }
}

/**
 * Aktualizuj status pompy
 */
void updatePumpStatus() {
    systemStatus.pumpActive = isPumpRunning();
}

/**
 * Pobierz status systemu
 */
WaterSystemStatus getWaterSystemStatus() {
    // Odśwież dane przed zwróceniem
    systemStatus.waterLevel = readWaterLevel();
    systemStatus.pumpActive = isPumpRunning();
    
    return systemStatus;
}