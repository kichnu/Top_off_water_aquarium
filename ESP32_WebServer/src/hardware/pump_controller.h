#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>

void initializePumpController();
void updatePumpController();
bool triggerAutomaticPump();
bool triggerManualPump(uint16_t duration_seconds, const String& action_type);
bool isPumpRunning();
uint32_t getPumpRemainingTime();

#endif