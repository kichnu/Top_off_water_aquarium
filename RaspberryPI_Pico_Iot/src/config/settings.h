#ifndef CONFIG_SETTINGS_H
#define CONFIG_SETTINGS_H

#include <Arduino.h>

// ================= STRUKTURY KONFIGURACJI =================
struct PumpConfig {
    int volumeML;           // Objętość jednego podania [ml]
    int pumpTimeSeconds;    // Czas pracy pompy [s]
};

// ================= STAŁE USTAWIEŃ =================
#define SETTINGS_MAGIC_NUMBER   0xA5B4  // Magic number dla walidacji EEPROM

// ================= FUNKCJE GŁÓWNE =================
void initializeSettings();
void loadSystemSettings();

// ================= KONFIGURACJA POMPY =================
void loadPumpConfig();
void savePumpConfig();
void setDefaultPumpConfig();
PumpConfig getPumpConfig();
bool setPumpConfig(int volumeML, int pumpTimeSeconds);

// ================= EVENT ID MANAGEMENT =================
void initializeEventIdIfNeeded();
unsigned long getNextEventId();
void incrementEventId();
void resetEventId();

// ================= FUNKCJE POMOCNICZE EEPROM =================
void writeUint16ToEEPROM(int address, uint16_t value);
uint16_t readUint16FromEEPROM(int address);
void writeUint32ToEEPROM(int address, uint32_t value);
uint32_t readUint32FromEEPROM(int address);

// ================= DIAGNOSTYKA =================
void clearEEPROM();
void debugSettings();

// ================= ZMIENNE GLOBALNE =================
extern PumpConfig currentPumpConfig;
extern bool settingsInitialized;

#endif