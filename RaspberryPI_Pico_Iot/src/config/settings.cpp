#include "settings.h"
#include "../config.h"
#include <EEPROM.h>

// ================= ZMIENNE GLOBALNE =================
PumpConfig currentPumpConfig;
bool settingsInitialized = false;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja systemu ustawień (EEPROM)
 */
void initializeSettings() {
    EEPROM.begin(EEPROM_SIZE);
    
    Serial.println("[SETTINGS-INIT] Inicjalizacja systemu ustawień (EEPROM)");
    Serial.printf("[SETTINGS-CONFIG] Rozmiar EEPROM: %d bajtów\n", EEPROM_SIZE);
    
    // Załaduj konfigurację pompy
    loadPumpConfig();
    
    // Sprawdź i zainicjalizuj Event ID jeśli potrzeba
    initializeEventIdIfNeeded();
    
    settingsInitialized = true;
    
    Serial.println("[SETTINGS-SUCCESS] Ustawienia załadowane pomyślnie");
    Serial.printf("[SETTINGS-PUMP] Volume: %dml, Time: %ds\n", 
                  currentPumpConfig.volumeML, currentPumpConfig.pumpTimeSeconds);
    Serial.printf("[SETTINGS-EVENT-ID] Następne ID zdarzenia: %lu\n", getNextEventId());
    Serial.println();
}

/**
 * Załaduj konfigurację pompy z EEPROM
 */
void loadPumpConfig() {
    // Sprawdź czy EEPROM ma prawidłowe dane (magic number)
    uint16_t magic = readUint16FromEEPROM(PUMP_CONFIG_ADDR);
    
    if (magic == SETTINGS_MAGIC_NUMBER) {
        // Załaduj zapisaną konfigurację
        currentPumpConfig.volumeML = readUint16FromEEPROM(PUMP_CONFIG_ADDR + 2);
        currentPumpConfig.pumpTimeSeconds = readUint16FromEEPROM(PUMP_CONFIG_ADDR + 4);
        
        Serial.println("[SETTINGS-LOAD] Załadowano konfigurację pompy z EEPROM");
        
        // Walidacja - sprawdź czy wartości są sensowne
        if (currentPumpConfig.volumeML < 1 || currentPumpConfig.volumeML > 1000 ||
            currentPumpConfig.pumpTimeSeconds < 1 || currentPumpConfig.pumpTimeSeconds > 120) {
            
            Serial.println("[SETTINGS-WARNING] Nieprawidłowe wartości w EEPROM - używam domyślnych");
            setDefaultPumpConfig();
            savePumpConfig();
        }
    } else {
        // Pierwsza inicjalizacja - ustaw domyślne wartości
        Serial.println("[SETTINGS-FIRST-INIT] Pierwsza inicjalizacja - ustawiam domyślne wartości");
        setDefaultPumpConfig();
        savePumpConfig();
    }
}

/**
 * Zapisz konfigurację pompy do EEPROM
 */
void savePumpConfig() {
    // Zapisz magic number
    writeUint16ToEEPROM(PUMP_CONFIG_ADDR, SETTINGS_MAGIC_NUMBER);
    
    // Zapisz konfigurację
    writeUint16ToEEPROM(PUMP_CONFIG_ADDR + 2, currentPumpConfig.volumeML);
    writeUint16ToEEPROM(PUMP_CONFIG_ADDR + 4, currentPumpConfig.pumpTimeSeconds);
    
    EEPROM.commit();
    
    Serial.printf("[SETTINGS-SAVE] Zapisano konfigurację pompy: %dml, %ds\n", 
                  currentPumpConfig.volumeML, currentPumpConfig.pumpTimeSeconds);
}

/**
 * Ustaw domyślną konfigurację pompy
 */
void setDefaultPumpConfig() {
    currentPumpConfig.volumeML = DEFAULT_VOLUME_ML;
    currentPumpConfig.pumpTimeSeconds = DEFAULT_PUMP_TIME_S;
    
    Serial.printf("[SETTINGS-DEFAULT] Ustawiono domyślną konfigurację: %dml, %ds\n", 
                  currentPumpConfig.volumeML, currentPumpConfig.pumpTimeSeconds);
}

/**
 * Pobierz aktualną konfigurację pompy
 */
PumpConfig getPumpConfig() {
    return currentPumpConfig;
}

/**
 * Ustaw nową konfigurację pompy
 */
bool setPumpConfig(int volumeML, int pumpTimeSeconds) {
    // Walidacja parametrów
    if (volumeML < 1 || volumeML > 1000) {
        Serial.printf("[SETTINGS-ERROR] Nieprawidłowa objętość: %dml (dozwolone: 1-1000ml)\n", volumeML);
        return false;
    }
    
    if (pumpTimeSeconds < 1 || pumpTimeSeconds > 120) {
        Serial.printf("[SETTINGS-ERROR] Nieprawidłowy czas: %ds (dozwolony: 1-120s)\n", pumpTimeSeconds);
        return false;
    }
    
    // Zapisz nową konfigurację
    currentPumpConfig.volumeML = volumeML;
    currentPumpConfig.pumpTimeSeconds = pumpTimeSeconds;
    
    savePumpConfig();
    
    Serial.printf("[SETTINGS-UPDATE] Zaktualizowano konfigurację pompy: %dml, %ds\n", 
                  volumeML, pumpTimeSeconds);
    
    return true;
}

/**
 * Załaduj ustawienia systemowe
 */
void loadSystemSettings() {
    if (!settingsInitialized) {
        initializeSettings();
    }
    
    Serial.println("[SETTINGS-SYSTEM] Ustawienia systemowe załadowane");
}

// ================= FUNKCJE EVENT ID =================

/**
 * Inicjalizuj Event ID jeśli potrzeba
 */
void initializeEventIdIfNeeded() {
    unsigned long eventId = readUint32FromEEPROM(EVENT_ID_ADDR);
    
    // Jeśli EEPROM jest czyste (0xFFFFFFFF) lub błędne (0), ustaw na 1
    if (eventId == 0xFFFFFFFF || eventId == 0) {
        eventId = 1;
        writeUint32ToEEPROM(EVENT_ID_ADDR, eventId);
        EEPROM.commit();
        
        Serial.println("[SETTINGS-EVENT-ID] Zainicjalizowano Event ID na 1");
    } else {
        Serial.printf("[SETTINGS-EVENT-ID] Załadowano Event ID: %lu\n", eventId);
    }
}

/**
 * Pobierz następne ID zdarzenia
 */
unsigned long getNextEventId() {
    return readUint32FromEEPROM(EVENT_ID_ADDR);
}

/**
 * Zwiększ licznik ID zdarzenia
 */
void incrementEventId() {
    unsigned long currentId = readUint32FromEEPROM(EVENT_ID_ADDR);
    currentId++;
    writeUint32ToEEPROM(EVENT_ID_ADDR, currentId);
    EEPROM.commit();
    
    #if ENABLE_DEBUG_LOGS
    Serial.printf("[SETTINGS-EVENT-ID] Zwiększono Event ID do: %lu\n", currentId);
    #endif
}

/**
 * Reset Event ID (użyj ostrożnie!)
 */
void resetEventId() {
    writeUint32ToEEPROM(EVENT_ID_ADDR, 1);
    EEPROM.commit();
    
    Serial.println("[SETTINGS-EVENT-ID-RESET] Event ID zresetowane do 1");
}

// ================= FUNKCJE POMOCNICZE EEPROM =================

/**
 * Zapisz uint16 do EEPROM
 */
void writeUint16ToEEPROM(int address, uint16_t value) {
    EEPROM.write(address, value & 0xFF);
    EEPROM.write(address + 1, (value >> 8) & 0xFF);
}

/**
 * Odczytaj uint16 z EEPROM
 */
uint16_t readUint16FromEEPROM(int address) {
    uint16_t value = EEPROM.read(address);
    value |= (EEPROM.read(address + 1) << 8);
    return value;
}

/**
 * Zapisz uint32 do EEPROM
 */
void writeUint32ToEEPROM(int address, uint32_t value) {
    EEPROM.write(address, value & 0xFF);
    EEPROM.write(address + 1, (value >> 8) & 0xFF);
    EEPROM.write(address + 2, (value >> 16) & 0xFF);
    EEPROM.write(address + 3, (value >> 24) & 0xFF);
}

/**
 * Odczytaj uint32 z EEPROM
 */
uint32_t readUint32FromEEPROM(int address) {
    uint32_t value = EEPROM.read(address);
    value |= (EEPROM.read(address + 1) << 8);
    value |= (EEPROM.read(address + 2) << 16);
    value |= (EEPROM.read(address + 3) << 24);
    return value;
}

/**
 * Wyczyść całą EEPROM (użyj ostrożnie!)
 */
void clearEEPROM() {
    Serial.println("[SETTINGS-CLEAR] UWAGA: Czyszczenie całej EEPROM!");
    
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, 0xFF);
    }
    
    EEPROM.commit();
    
    Serial.println("[SETTINGS-CLEAR] EEPROM wyczyszczona - restart wymagany");
}

/**
 * Debug - wyświetl status ustawień
 */
void debugSettings() {
    Serial.println("[SETTINGS-DEBUG] === STATUS USTAWIEŃ ===");
    Serial.printf("[SETTINGS-DEBUG] Zainicjalizowane: %s\n", settingsInitialized ? "TAK" : "NIE");
    Serial.printf("[SETTINGS-DEBUG] Magic number: 0x%04X\n", readUint16FromEEPROM(PUMP_CONFIG_ADDR));
    Serial.printf("[SETTINGS-DEBUG] Volume ML: %d\n", currentPumpConfig.volumeML);
    Serial.printf("[SETTINGS-DEBUG] Pump Time: %d sekund\n", currentPumpConfig.pumpTimeSeconds);
    Serial.printf("[SETTINGS-DEBUG] Event ID: %lu\n", getNextEventId());
    Serial.println("[SETTINGS-DEBUG] ======================");
    Serial.println();
}