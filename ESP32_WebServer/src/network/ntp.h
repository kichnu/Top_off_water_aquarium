#ifndef NETWORK_NTP_H
#define NETWORK_NTP_H

#include <Arduino.h>
#include "time.h"
#include "../config.h"

// ================= STRUKTURY =================
struct TimeInfo {
    bool synced;
    String localTime;
    String utcTime;
    String timezone;
    bool isDST;
};

// ================= FUNKCJE NTP =================
void initializeNTP();
bool syncNTPTime();
bool isNTPSynced();
TimeInfo getCurrentTime();
String getFormattedTime(bool useUTC = false);
time_t getUnixTimestamp();

// ================= ZMIENNE GLOBALNE =================
extern bool ntpSynced;
extern unsigned long lastNTPSync;

#endif