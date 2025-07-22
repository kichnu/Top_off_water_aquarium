#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

struct PumpConfig {
    uint16_t pumpTimeSeconds;
    uint16_t volumeML;
    bool autoPumpEnabled;
    uint32_t dailyLimitML;
    uint16_t minIntervalSeconds;
};

void initializeSettings();
void loadDefaultSettings();
void saveSettings();

// Getters
String getWiFiSSID();
String getWiFiPassword();
String getAdminPasswordHash();
PumpConfig getPumpConfig();

// Setters
void setWiFiCredentials(const String& ssid, const String& password);
void setAdminPasswordHash(const String& hash);
void setPumpConfig(const PumpConfig& config);

#endif