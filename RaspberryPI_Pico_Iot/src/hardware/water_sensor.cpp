#include "water_sensor.h"
#include "../config.h"

// ================= ZMIENNE GLOBALNE =================
WaterLevel lastWaterLevel = WATER_LEVEL_UNKNOWN;
unsigned long lastSensorRead = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja czujnika poziomu wody
 */
void initializeWaterSensor() {
    pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
    
    Serial.println("[WATER-SENSOR-INIT] Inicjalizacja czujnika poziomu wody");
    Serial.printf("[WATER-SENSOR-CONFIG] Pin: %d (INPUT_PULLUP)\n", WATER_SENSOR_PIN);
    Serial.println("[WATER-SENSOR-LOGIC] LOW = niski poziom wody, HIGH = poziom OK");
    
    // Początkowy odczyt
    lastWaterLevel = readWaterLevel();
    lastSensorRead = millis();
    
    Serial.printf("[WATER-SENSOR-INITIAL] Początkowy stan: %s\n", 
                  lastWaterLevel == WATER_LEVEL_LOW ? "NISKI" : 
                  lastWaterLevel == WATER_LEVEL_OK ? "OK" : "NIEZNANY");
    Serial.println();
}

/**
 * Odczytaj aktualny poziom wody z czujnika
 */
WaterLevel readWaterLevel() {
    // Odczytaj stan pinu (INPUT_PULLUP: LOW = kontakt zamknięty)
    bool sensorState = digitalRead(WATER_SENSOR_PIN);
    
    // LOW = niski poziom wody (kontakton aktywny)
    // HIGH = poziom OK (kontakton nieaktywny)
    if (sensorState == LOW) {
        return WATER_LEVEL_LOW;
    } else {
        return WATER_LEVEL_OK;
    }
}

/**
 * Sprawdź czy poziom wody się zmienił
 */
bool hasWaterLevelChanged() {
    WaterLevel currentLevel = readWaterLevel();
    lastSensorRead = millis();
    
    if (currentLevel != lastWaterLevel) {
        
        #if ENABLE_DEBUG_LOGS
        Serial.printf("[WATER-SENSOR-CHANGE] Zmiana poziomu: %s -> %s\n",
                      lastWaterLevel == WATER_LEVEL_LOW ? "NISKI" : 
                      lastWaterLevel == WATER_LEVEL_OK ? "OK" : "NIEZNANY",
                      currentLevel == WATER_LEVEL_LOW ? "NISKI" : 
                      currentLevel == WATER_LEVEL_OK ? "OK" : "NIEZNANY");
        #endif
        
        lastWaterLevel = currentLevel;
        return true;
    }
    
    return false;
}

/**
 * Pobierz ostatni odczytany poziom wody
 */
WaterLevel getLastWaterLevel() {
    return lastWaterLevel;
}

/**
 * Pobierz czas ostatniego odczytu czujnika
 */
unsigned long getLastSensorReadTime() {
    return lastSensorRead;
}

/**
 * Sprawdź czy czujnik działa poprawnie
 */
bool isWaterSensorWorking() {
    // Prosta diagnostyka - sprawdź czy czujnik odpowiada
    WaterLevel reading1 = readWaterLevel();
    delay(10);
    WaterLevel reading2 = readWaterLevel();
    
    // Jeśli dwa odczyty w odstępie 10ms są identyczne, 
    // prawdopodobnie czujnik działa
    return (reading1 == reading2);
}

/**
 * Pobierz status czujnika jako string
 */
String getWaterLevelString(WaterLevel level) {
    switch (level) {
        case WATER_LEVEL_LOW:
            return "LOW";
        case WATER_LEVEL_OK:
            return "OK";
        case WATER_LEVEL_UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

/**
 * Debug - wyświetl stan czujnika
 */
void debugWaterSensor() {
    WaterLevel current = readWaterLevel();
    bool pinState = digitalRead(WATER_SENSOR_PIN);
    
    Serial.println("[WATER-SENSOR-DEBUG] === STATUS CZUJNIKA ===");
    Serial.printf("[WATER-SENSOR-DEBUG] Pin %d stan: %s\n", 
                  WATER_SENSOR_PIN, pinState ? "HIGH" : "LOW");
    Serial.printf("[WATER-SENSOR-DEBUG] Poziom wody: %s\n", 
                  getWaterLevelString(current).c_str());
    Serial.printf("[WATER-SENSOR-DEBUG] Ostatni odczyt: %lu ms temu\n", 
                  millis() - lastSensorRead);
    Serial.printf("[WATER-SENSOR-DEBUG] Czujnik sprawny: %s\n", 
                  isWaterSensorWorking() ? "TAK" : "NIE");
    Serial.println("[WATER-SENSOR-DEBUG] ========================");
    Serial.println();
}