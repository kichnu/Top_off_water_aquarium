#ifndef HARDWARE_ACTUATORS_H
#define HARDWARE_ACTUATORS_H

#include <Arduino.h>
#include "../config.h"

// ================= STANY URZĄDZEŃ =================
extern bool ledState;

// *** ROZSZERZENIE: Dodaj tutaj zmienne stanu dla nowych urządzeń ***
// ============= STANY PRZEKAŹNIKÓW =============
// extern bool relay1State;  // Stan przekaźnika 1
// extern bool relay2State;  // Stan przekaźnika 2
// extern bool relay3State;  // Stan przekaźnika 3

// ============= POZYCJE SERWOMECHANIZMÓW =============
// extern int servo1Position;   // Pozycja servo 1 (0-180°)
// extern int servo2Position;   // Pozycja servo 2 (0-180°)

// ============= PWM/DIMMER =============
// extern int ledStripBrightness;  // Jasność taśmy LED (0-255)
// extern int fanSpeed;           // Prędkość wentylatora (0-255)

// ================= FUNKCJE STEROWANIA =================
void initializeActuators();

// LED Control
void setLEDState(bool state);
bool getLEDState();

// *** ROZSZERZENIE: Dodaj tutaj funkcje sterowania różnymi urządzeniami ***

// ============= FUNKCJE STEROWANIA PRZEKAŹNIKAMI =============
/*
void setRelayState(int relayNumber, bool state);
bool getRelayState(int relayNumber);
*/

// ============= FUNKCJE STEROWANIA SERWOMECHANIZMAMI =============
/*
void setServoPosition(int servoNumber, int position);
int getServoPosition(int servoNumber);
*/

// ============= FUNKCJE PWM/DIMMER =============
/*
void setLEDStripBrightness(int brightness);
void setFanSpeed(int speed);
int getLEDStripBrightness();
int getFanSpeed();
*/

// ================= MONITORING =================
String getActuatorStatus();
void updateActuatorStates();

#endif