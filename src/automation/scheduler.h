#ifndef AUTOMATION_SCHEDULER_H
#define AUTOMATION_SCHEDULER_H

#include <Arduino.h>
#include "../config.h"

// *** ROZSZERZENIE: Ten plik służy jako przykład rozszerzenia o automatykę ***

// ================= STRUKTURY AUTOMATYKI =================
struct Schedule {
    int id;
    int hour;
    int minute;
    String action;
    String parameters;
    bool enabled;
    bool recurring;
    unsigned long lastExecution;
};

struct AutomationRule {
    int id;
    String condition;
    String action;
    String parameters;
    bool enabled;
    unsigned long lastTrigger;
    unsigned long cooldown;  // Czas między kolejnymi uruchomieniami
};

// ================= STAŁE =================
const int MAX_SCHEDULES = 20;
const int MAX_AUTOMATION_RULES = 15;
const unsigned long DEFAULT_COOLDOWN = 30000; // 30 sekund

// ================= FUNKCJE AUTOMATYKI =================
void initializeScheduler();
void processSchedules();
void processAutomationRules();

// Schedule Management
int addSchedule(int hour, int minute, const String& action, const String& parameters = "");
void removeSchedule(int id);
void enableSchedule(int id, bool enabled);
Schedule* getSchedule(int id);
int getScheduleCount();

// Rule Management
int addAutomationRule(const String& condition, const String& action, const String& parameters = "");
void removeAutomationRule(int id);
void enableAutomationRule(int id, bool enabled);
AutomationRule* getAutomationRule(int id);
int getAutomationRuleCount();

// Execution
void executeScheduledAction(const String& action, const String& parameters);
void executeAutomationAction(const String& action, const String& parameters);
bool evaluateCondition(const String& condition);

// ================= ZMIENNE GLOBALNE =================
extern Schedule schedules[];
extern AutomationRule automationRules[];
extern int scheduleCount;
extern int automationRuleCount;
extern unsigned long lastScheduleCheck;
extern unsigned long lastRuleCheck;

// ================= PRZYKŁADY UŻYCIA =================
/*

// Dodawanie harmonogramów:
addSchedule(7, 0, "relay1_on");           // Włącz światło o 7:00
addSchedule(22, 30, "relay1_off");        // Wyłącz światło o 22:30
addSchedule(6, 0, "led_on", "brightness:128"); // Włącz LED z jasnością 128

// Dodawanie reguł automatyki:
addAutomationRule("motion_detected", "relay1_on");     // Światło przy ruchu
addAutomationRule("door_open", "led_on");              // LED przy otwartych drzwiach
addAutomationRule("temp_high", "fan_on", "speed:200"); // Wentylator przy wysokiej temperaturze

// Warunki (do implementacji w evaluateCondition):
// - motion_detected: Wykryty ruch (PIR)
// - door_open: Otwarte drzwi (magnetic sensor)
// - light_low: Niski poziom światła (< 100)
// - temp_high: Wysoka temperatura (> 25°C)
// - temp_low: Niska temperatura (< 20°C)
// - time_range:19:00-07:00: Czas w przedziale (noc)
// - humidity_high: Wysoka wilgotność (> 70%)

// Akcje (do implementacji w executeScheduledAction/executeAutomationAction):
// - led_on / led_off: Sterowanie LED
// - relay1_on / relay1_off: Sterowanie przekaźnikiem 1
// - relay2_on / relay2_off: Sterowanie przekaźnikiem 2
// - relay3_on / relay3_off: Sterowanie przekaźnikiem 3
// - servo1_set: Ustawienie pozycji servo 1 (parameters: "position:90")
// - servo2_set: Ustawienie pozycji servo 2 (parameters: "position:180")
// - ledstrip_set: Ustawienie jasności taśmy LED (parameters: "brightness:255")
// - fan_set: Ustawienie prędkości wentylatora (parameters: "speed:200")
// - alarm_trigger: Uruchomienie alarmu (parameters: "type:intrusion")

*/

#endif