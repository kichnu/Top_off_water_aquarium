#ifndef HARDWARE_RTC_DS3231_H
#define HARDWARE_RTC_DS3231_H

#include <Arduino.h>
#include "DFRobot_DS3231M.h"

// ================= STRUKTURY CZASU =================
struct TimeInfo {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    float temperature;
    bool valid;
};

// ================= FUNKCJE RTC =================
void initializeRTC();
String getCurrentTimeString();
TimeInfo getCurrentTime();
unsigned long getUnixTimestamp();

// ================= FORMATOWANIE CZASU =================
String formatTimeInfo(TimeInfo timeInfo);
String formatDateTimeForFilename(TimeInfo timeInfo);
String getCurrentDateString();
String getCurrentTimeOnlyString();

// ================= DIAGNOSTYKA =================
bool isRTCWorking();
float getRTCTemperature();
void debugRTC();

// ================= ZMIENNE GLOBALNE =================
extern DFRobot_DS3231M rtc;
extern bool rtcInitialized;
extern unsigned long lastRTCRead;

#endif