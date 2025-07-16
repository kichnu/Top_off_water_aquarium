#ifndef HARDWARE_PUMP_CONTROLLER_H
#define HARDWARE_PUMP_CONTROLLER_H

#include <Arduino.h>

// ================= FUNKCJE STEROWANIA POMPY =================
void initializePumpController();
bool runPump(int durationSeconds);
void stopPump();

// ================= MONITORING POMPY =================
bool isPumpRunning();
unsigned long getPumpRunTime();
unsigned long getLastPumpStartTime();
unsigned long getLastPumpEndTime();
int getLastPumpDuration();

// ================= DIAGNOSTYKA =================
bool testPump();
String getPumpStatusString();
void debugPumpController();

// ================= ZMIENNE GLOBALNE =================
extern bool pumpRunning;
extern unsigned long pumpStartTime;
extern unsigned long pumpEndTime;
extern int lastPumpDuration;

#endif