#ifndef SESSIONS_H
#define SESSIONS_H

#include <Arduino.h>
#include <WiFi.h>

struct Session {
    String token;
    IPAddress ip;
    unsigned long created_at;
    unsigned long last_activity;
    bool is_valid;
};

void initializeSessions();
void updateSessions();
String createSession(IPAddress ip);
bool validateSession(const String& token, IPAddress ip);
void destroySession(const String& token);
void clearExpiredSessions();

#endif