#include "rtc_controller.h"
#include "../core/logging.h"
#include <Wire.h>
#include <RTClib.h>



RTC_DS3231 rtc;
bool rtcInitialized = false;

void initializeRTC() {
    LOG_INFO("Starting RTC DS3231 initialization with RTClib...");
    
    // Inicjalizacja I2C z domyślnymi pinami ESP32-C3
    Wire.begin();
    
    // Sprawdzenie czy moduł DS3231 jest podłączony
    if (!rtc.begin()) {
        LOG_ERROR("Cannot find DS3231 RTC module!");
        LOG_ERROR("Check connections:");
        LOG_ERROR("DS3231 VCC -> ESP32 3.3V");
        LOG_ERROR("DS3231 GND -> ESP32 GND");
        LOG_ERROR("DS3231 SDA -> ESP32 GPIO5 (default SDA)");
        LOG_ERROR("DS3231 SCL -> ESP32 GPIO6 (default SCL)");
        rtcInitialized = false;
        return;
    }
    
    rtcInitialized = true;
    
    // Sprawdzenie czy RTC stracił zasilanie
    if (rtc.lostPower()) {
        LOG_WARNING("RTC lost power, setting time from compile time!");
        // Ustaw czas na moment kompilacji programu
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        LOG_INFO("RTC time set from compile time");
    }
    
    LOG_INFO("RTC DS3231 initialized successfully");
    LOG_INFO("Current time: %s", getCurrentTimestamp().c_str());
}

String getCurrentTimestamp() {
    if (!rtcInitialized) {
        return "RTC_ERROR";
    }
    
    DateTime now = rtc.now();
    
    char timeBuffer[20];
    snprintf(timeBuffer, sizeof(timeBuffer), "%04d-%02d-%02d %02d:%02d:%02d",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
    
    return String(timeBuffer);
}

bool isRTCWorking() {
    if (!rtcInitialized) {
        return false;
    }
    
    DateTime now = rtc.now();
    return (now.year() >= 2020);  // Sprawdź czy czas ma sens
}

unsigned long getUnixTimestamp() {
    if (!rtcInitialized) {
        return 1609459200; // Default: 2021-01-01
    }
    
    DateTime now = rtc.now();
    return now.unixtime();
}
