#ifndef RTC_DS3231_H
#define RTC_DS3231_H

#include <Arduino.h>

// Podstawowe funkcje RTC
void initializeRTC();
String getRTCTimestamp();
unsigned long getUnixTimestamp();
void setRTCTime(unsigned long unix_timestamp);
bool isRTCValid();

// Dodatkowe funkcje diagnostyczne i użytkowe
float getRTCTemperature();
bool isRTCWorking();
String getCurrentDateString();
String getCurrentTimeOnlyString();
void debugRTC();

// Zmienne globalne (extern)
extern bool rtcInitialized;
extern unsigned long lastRTCRead;

#endif