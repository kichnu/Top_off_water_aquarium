#ifndef NTP_H
#define NTP_H

#include <Arduino.h>

void initializeNTP();
void updateNTP();
bool isNTPSynced();
String getNTPStatus();

#endif