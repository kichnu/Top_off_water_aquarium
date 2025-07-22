#include "rtc_controller.h"
#include "../config/hardware_pins.h"
#include "../core/logging.h"
#include <DFRobot_DS3231M.h>
#include <Wire.h>

DFRobot_DS3231M rtc;
bool rtcInitialized = false;

void initRTC() {
    Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN);
    
    int attempts = 0;
    while(!rtc.begin() && attempts < 10) {
        LOG_WARNING("RTC initialization attempt %d", attempts + 1);
        delay(1000);
        attempts++;
    }
    
    if (attempts >= 10) {
        LOG_ERROR("RTC initialization failed");
        rtcInitialized = false;
        return;
    }
    
    rtcInitialized = true;
    
    if (rtc.lostPower()) {
        LOG_WARNING("RTC lost power - setting time from compile time");
        setRTCFromCompileTime();
    }
    
    LOG_INFO("RTC DS3231M initialized successfully");
    LOG_INFO("Current time: %s", getCurrentTimestamp().c_str());
}

void setRTCFromCompileTime() {
    if (!rtcInitialized) {
        LOG_ERROR("Cannot set RTC time - RTC not initialized");
        return;
    }
    
    // Makra kompilacji - automatyczny czas!
    const char* compileDate = __DATE__;  // "Jan 23 2025"
    const char* compileTime = __TIME__;  // "14:23:45"
    
    LOG_INFO("Setting RTC from compile time: %s %s", compileDate, compileTime);
    
    // Parsuj datę (format: "Jan 23 2025")
    char monthStr[4];
    int day, year;
    sscanf(compileDate, "%s %d %d", monthStr, &day, &year);
    
    // Konwertuj miesiąc z tekstu na numer
    int month = 1;
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++) {
        if (strncmp(monthStr, months[i], 3) == 0) {
            month = i + 1;
            break;
        }
    }
    
    // Parsuj czas (format: "14:23:45")
    int hour, minute, second;
    sscanf(compileTime, "%d:%d:%d", &hour, &minute, &second);
    
    // Ustaw czas w RTC
    rtc.setYear(year);
    rtc.setMonth(month);
    rtc.setDate(day);
    rtc.setHour(hour, e24hours);
    rtc.setMinute(minute);
    rtc.setSecond(second);
    
    LOG_INFO("RTC time set to: %04d-%02d-%02d %02d:%02d:%02d", 
             year, month, day, hour, minute, second);
}

String getCurrentTimestamp() {
    if (!rtcInitialized) {
        return "RTC_ERROR";
    }
    
    rtc.getNowTime();
    char timeBuffer[20];
    snprintf(timeBuffer, sizeof(timeBuffer), "%04d-%02d-%02d %02d:%02d:%02d",
             rtc.year(), rtc.month(), rtc.day(),
             rtc.hour(), rtc.minute(), rtc.second());
    
    return String(timeBuffer);
}

bool isRTCWorking() {
    return rtcInitialized && !rtc.lostPower();
}

unsigned long getUnixTimestamp() {
    if (!rtcInitialized) {
        return 1609459200; // Default: 2021-01-01
    }
    
    rtc.getNowTime();
    struct tm timeinfo;
    timeinfo.tm_year = rtc.year() - 1900;
    timeinfo.tm_mon = rtc.month() - 1;
    timeinfo.tm_mday = rtc.day();
    timeinfo.tm_hour = rtc.hour();
    timeinfo.tm_min = rtc.minute();
    timeinfo.tm_sec = rtc.second();
    timeinfo.tm_isdst = -1;
    
    time_t timestamp = mktime(&timeinfo);
    return (unsigned long)timestamp;
}