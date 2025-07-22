#ifndef RTC_CONTROLLER_H
#define RTC_CONTROLLER_H

#include <Arduino.h>

void initRTC();
String getCurrentTimestamp();
bool isRTCWorking();
unsigned long getUnixTimestamp();
void setRTCFromCompileTime();  // NOWA FUNKCJA

#endif