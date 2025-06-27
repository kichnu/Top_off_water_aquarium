#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <Arduino.h>
#include "../config.h"

// ================= STRUKTURY SYSTEMOWE =================
struct SystemInfo {
    unsigned long uptime;
    uint32_t freeHeap;
    int wifiRSSI;
    bool ntpSynced;
    String currentTime;
    String timezone;
};

// ================= FUNKCJE SYSTEMOWE =================
void initializeSystem();
void systemMaintenance();
SystemInfo getSystemInfo();
void restartSystem();
void checkMemoryUsage();

// ================= ZMIENNE GLOBALNE =================
extern unsigned long systemStartTime;
extern bool systemInitialized;

#endif