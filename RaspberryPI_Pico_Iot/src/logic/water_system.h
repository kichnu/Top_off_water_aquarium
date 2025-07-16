#ifndef LOGIC_WATER_SYSTEM_H
#define LOGIC_WATER_SYSTEM_H

#include <Arduino.h>

// ================= ENUMS I STRUKTURY =================

enum WaterLevel {
    WATER_LEVEL_UNKNOWN = 0,
    WATER_LEVEL_OK = 1,
    WATER_LEVEL_LOW = 2
};

struct WaterSystemStatus {
    WaterLevel waterLevel;
    bool pumpActive;
    String lastPumpTime;
    int eventsToday;
    unsigned long totalEvents;
    bool systemReady;
};

// ================= GŁÓWNE FUNKCJE SYSTEMU =================
void initializeWaterSystem();
void processWaterSystem();
WaterSystemStatus getWaterSystemStatus();

// ================= STEROWANIE POMPY =================
bool triggerManualPump();
void triggerAutomaticPump();
void updatePumpStatus();

// ================= LOGIKA UPRAWNIEŃ =================
bool check_permission();

// ================= CZUJNIK WODY =================
void checkWaterSensorWithDebouncing();

// ================= ZMIENNE GLOBALNE =================
extern WaterSystemStatus systemStatus;

#endif