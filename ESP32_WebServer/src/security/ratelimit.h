#ifndef RATELIMIT_H
#define RATELIMIT_H

#include <Arduino.h>
#include <WiFi.h>

void initializeRateLimit();
void updateRateLimit();
bool isRateLimited(IPAddress ip);
void recordRequest(IPAddress ip);
void recordFailedAttempt(IPAddress ip);
bool isIPBlocked(IPAddress ip);

#endif