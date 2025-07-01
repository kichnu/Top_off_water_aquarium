#ifndef HARDWARE_ACTUATORS_H
#define HARDWARE_ACTUATORS_H

#include <Arduino.h>
#include "../config.h"

// ================= CACHE STANÓW URZĄDZEŃ =================
// WebServer cache'uje stany urządzeń na podstawie odpowiedzi z IoT
extern bool ledState;

// *** ROZSZERZENIE: Dodaj tutaj zmienne cache dla nowych urządzeń ***
// ============= CACHE STANÓW PRZEKAŹNIKÓW =============
// extern bool relay1State;  // Cache stanu przekaźnika 1
// extern bool relay2State;  // Cache stanu przekaźnika 2
// extern bool relay3State;  // Cache stanu przekaźnika 3

// ============= CACHE POZYCJI SERWOMECHANIZMÓW =============
// extern int servo1Position;   // Cache pozycji servo 1 (0-180°)
// extern int servo2Position;   // Cache pozycji servo 2 (0-180°)

// ============= CACHE PWM/DIMMER =============
// extern int ledStripBrightness;  // Cache jasności taśmy LED (0-255)
// extern int fanSpeed;           // Cache prędkości wentylatora (0-255)

// ============= CACHE DANYCH SENSORÓW =============
// extern float temperature;      // Cache temperatury
// extern int lightLevel;         // Cache poziomu światła
// extern int moistureLevel;      // Cache poziomu wilgotności
// extern bool motionDetected;    // Cache wykrytego ruchu
// extern bool doorOpen;          // Cache stanu drzwi

// ================= FUNKCJE STEROWANIA =================
void initializeActuators();

// LED Control (przez UART)
void setLEDState(bool state);
bool getLEDState();

// *** ROZSZERZENIE: Dodaj tutaj funkcje sterowania różnymi urządzeniami ***

// ============= FUNKCJE STEROWANIA PRZEKAŹNIKAMI (przez UART) =============
/*
void setRelayState(int relayNumber, bool state);
bool getRelayState(int relayNumber);
*/

// ============= FUNKCJE STEROWANIA SERWOMECHANIZMAMI (przez UART) =============
/*
void setServoPosition(int servoNumber, int position);
int getServoPosition(int servoNumber);
*/

// ============= FUNKCJE PWM/DIMMER (przez UART) =============
/*
void setLEDStripBrightness(int brightness);
void setFanSpeed(int speed);
int getLEDStripBrightness();
int getFanSpeed();
*/

// ============= FUNKCJE ODCZYTU SENSORÓW (przez UART) =============
/*
void requestSensorData();
void updateSensorCache(const String& sensorData);
float getTemperature();
int getLightLevel();
int getMoistureLevel();
bool getMotionDetected();
bool getDoorOpen();
*/

// ================= SYNCHRONIZACJA STANU =================
void requestStateSync();
void processSyncResponse(const String& result);
void updateStateFromResponse(const String& action, const String& result);

// ================= MONITORING =================
String getActuatorStatus();
void updateActuatorStates();

#endif