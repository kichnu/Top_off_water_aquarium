#include "settings.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "../core/logging.h"
#include "hardware_pins.h"

#define EEPROM_SIZE 1024
#define SETTINGS_MAGIC 0xABCD1234

struct SettingsData {
    uint32_t magic;
    char wifi_ssid[64];
    char wifi_password[64];
    char admin_password_hash[65];  // SHA256 hex string + null terminator
    uint16_t pump_time_seconds;
    uint16_t volume_ml_per_second;
    uint32_t daily_limit_ml;
    uint16_t min_interval_seconds;
    bool auto_pump_enabled;
    uint32_t checksum;
};

SettingsData currentSettings;

uint32_t calculateChecksum(const SettingsData* data) {
    uint32_t checksum = 0;
    const uint8_t* bytes = (const uint8_t*)data;
    for (size_t i = 0; i < sizeof(SettingsData) - sizeof(uint32_t); i++) {
        checksum += bytes[i];
    }
    return checksum;
}

void initializeSettings() {
    EEPROM.begin(EEPROM_SIZE);
    
    EEPROM.get(0, currentSettings);
    
    if (currentSettings.magic != SETTINGS_MAGIC || 
        currentSettings.checksum != calculateChecksum(&currentSettings)) {
        
        LOG_INFO("Initializing default settings");
        loadDefaultSettings();
        saveSettings();
    } else {
        LOG_INFO("Settings loaded from EEPROM");
    }
}

void loadDefaultSettings() {
    currentSettings.magic = SETTINGS_MAGIC;
    strcpy(currentSettings.wifi_ssid, "YourWiFiNetwork");
    strcpy(currentSettings.wifi_password, "YourWiFiPassword");
    strcpy(currentSettings.admin_password_hash, "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"); // "password"
    currentSettings.pump_time_seconds = PUMP_DEFAULT_TIME;
    currentSettings.volume_ml_per_second = 100;  // Assume 100ml/second flow rate
    currentSettings.daily_limit_ml = MAX_DAILY_VOLUME_ML;
    currentSettings.min_interval_seconds = MIN_PUMP_INTERVAL;
    currentSettings.auto_pump_enabled = true;
    currentSettings.checksum = calculateChecksum(&currentSettings);
}

void saveSettings() {
    currentSettings.checksum = calculateChecksum(&currentSettings);
    EEPROM.put(0, currentSettings);
    EEPROM.commit();
    LOG_INFO("Settings saved to EEPROM");
}

// Getters
String getWiFiSSID() { return String(currentSettings.wifi_ssid); }
String getWiFiPassword() { return String(currentSettings.wifi_password); }
String getAdminPasswordHash() { return String(currentSettings.admin_password_hash); }

PumpConfig getPumpConfig() {
    PumpConfig config;
    config.pumpTimeSeconds = currentSettings.pump_time_seconds;
    config.volumeML = currentSettings.pump_time_seconds * currentSettings.volume_ml_per_second;
    config.autoPumpEnabled = currentSettings.auto_pump_enabled;
    config.dailyLimitML = currentSettings.daily_limit_ml;
    config.minIntervalSeconds = currentSettings.min_interval_seconds;
    return config;
}

// Setters
void setWiFiCredentials(const String& ssid, const String& password) {
    strncpy(currentSettings.wifi_ssid, ssid.c_str(), 63);
    strncpy(currentSettings.wifi_password, password.c_str(), 63);
    currentSettings.wifi_ssid[63] = '\0';
    currentSettings.wifi_password[63] = '\0';
    saveSettings();
}

void setAdminPasswordHash(const String& hash) {
    strncpy(currentSettings.admin_password_hash, hash.c_str(), 64);
    currentSettings.admin_password_hash[64] = '\0';
    saveSettings();
}

void setPumpConfig(const PumpConfig& config) {
    currentSettings.pump_time_seconds = config.pumpTimeSeconds;
    currentSettings.volume_ml_per_second = config.volumeML / config.pumpTimeSeconds;
    currentSettings.auto_pump_enabled = config.autoPumpEnabled;
    currentSettings.daily_limit_ml = config.dailyLimitML;
    currentSettings.min_interval_seconds = config.minIntervalSeconds;
    saveSettings();
}