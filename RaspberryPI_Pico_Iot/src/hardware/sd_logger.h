#ifndef HARDWARE_SD_LOGGER_H
#define HARDWARE_SD_LOGGER_H

#include <Arduino.h>

// ================= FUNKCJE LOGOWANIA =================
void initializeSDLogger();
bool logPumpEvent(const String& type, int volumeML, const String& timestamp);

// ================= ZARZĄDZANIE PLIKAMI =================
bool createNewLogFile();
bool appendEventToFile(const String& eventJson);
unsigned long countExistingEvents();

// ================= ODCZYT LOGÓW =================
String getRecentEvents(int count = 10);
String getLogFileInfo();

// ================= DIAGNOSTYKA =================
void displaySDInfo();
bool isSDWorking();
String getLastLogError();
void debugSDLogger();

// ================= ZMIENNE GLOBALNE =================
extern bool sdInitialized;
extern unsigned long totalLoggedEvents;
extern String lastLogError;

#endif