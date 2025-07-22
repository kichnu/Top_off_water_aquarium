#include "rtc_ds3231.h"
#include "../config/hardware_pins.h"
#include "../core/logging.h"
#include <DFRobot_DS3231M.h>
#include <Wire.h>

// ================= ZMIENNE GLOBALNE =================
DFRobot_DS3231M rtc;
bool rtcInitialized = false;
unsigned long lastRTCRead = 0;

// ================= IMPLEMENTACJE =================

void initializeRTC() {
    Serial.println("[RTC-INIT] Inicjalizacja RTC DS3231M (DFRobot)");
    
    // Konfiguruj I2C dla ESP32-C6
    Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN);
    
    Serial.printf("[RTC-CONFIG] I2C SDA: GPIO%d, SCL: GPIO%d\n", RTC_SDA_PIN, RTC_SCL_PIN);
    
    // Inicjalizuj RTC - czekaj aż się zainicjalizuje
    int initAttempts = 0;
    while(rtc.begin() != true && initAttempts < 10) {
        Serial.println("[RTC-INIT] Próba inicjalizacji RTC...");
        delay(1000);
        initAttempts++;
    }
    
    if (initAttempts >= 10) {
        Serial.println("[RTC-ERROR] Nie można zainicjalizować RTC DS3231M!");
        Serial.println("[RTC-ERROR] Sprawdź połączenia I2C i zasilanie RTC");
        rtcInitialized = false;
        return;
    }
    
    rtcInitialized = true;
    lastRTCRead = millis();
    
    // Sprawdź czy RTC stracił zasilanie
    if (rtc.lostPower()) {
        Serial.println("[RTC-WARNING] RTC stracił zasilanie - czas może być nieprawidłowy");
        Serial.println("[RTC-WARNING] Ustaw czas w RTC przed użyciem systemu");
    }
    
    // Wyświetl aktualny czas
    rtc.getNowTime();
    Serial.println("[RTC-SUCCESS] RTC DS3231M zainicjalizowany pomyślnie");
    Serial.printf("[RTC-TIME] Aktualny czas: %04d-%02d-%02d %02d:%02d:%02d\n",
                  rtc.year(), rtc.month(), rtc.day(),
                  rtc.hour(), rtc.minute(), rtc.second());
    Serial.println();
    
    LOG_INFO("RTC DS3231M initialized successfully");
}

String getRTCTimestamp() {
    if (!rtcInitialized) {
        return "RTC-ERROR";
    }
    
    rtc.getNowTime();
    lastRTCRead = millis();
    
    char timeBuffer[20];
    snprintf(timeBuffer, sizeof(timeBuffer), "%04d-%02d-%02d %02d:%02d:%02d",
             rtc.year(), rtc.month(), rtc.day(),
             rtc.hour(), rtc.minute(), rtc.second());
    
    return String(timeBuffer);
}

unsigned long getUnixTimestamp() {
    if (!rtcInitialized) {
        // Fallback to system time if RTC not available
        time_t now = time(nullptr);
        if (now > 1609459200) { // After 2021-01-01
            return (unsigned long)now;
        }
        return 1609459200; // Default to 2021-01-01
    }
    
    rtc.getNowTime();
    
    // Convert RTC time to Unix timestamp
    struct tm timeinfo;
    timeinfo.tm_year = rtc.year() - 1900;  // tm_year is years since 1900
    timeinfo.tm_mon = rtc.month() - 1;     // tm_mon is 0-11
    timeinfo.tm_mday = rtc.day();
    timeinfo.tm_hour = rtc.hour();
    timeinfo.tm_min = rtc.minute();
    timeinfo.tm_sec = rtc.second();
    timeinfo.tm_isdst = -1;  // Let mktime determine DST
    
    time_t timestamp = mktime(&timeinfo);
    return (unsigned long)timestamp;
}

// void setRTCTime(unsigned long unix_timestamp) {
//     if (!rtcInitialized) {
//         LOG_WARNING("RTC not available, cannot set time");
//         return;
//     }
    
//     time_t time_val = unix_timestamp;
//     struct tm* timeinfo = localtime(&time_val);
    
//     Set time using DFRobot methods
//     rtc.setYear(timeinfo->tm_year + 1900);
//     rtc.setMonth(timeinfo->tm_mon + 1);
//     rtc.setDate(timeinfo->tm_mday);
//     rtc.setHour(timeinfo->tm_hour);
//     rtc.setMinute(timeinfo->tm_min);
//     rtc.setSecond(timeinfo->tm_sec);
    
//     LOG_INFO("RTC time updated from Unix timestamp");
// }

bool isRTCValid() {
    if (!rtcInitialized) {
        return false;
    }
    
    return !rtc.lostPower();
}
