#ifndef SECURITY_SESSIONS_H
#define SECURITY_SESSIONS_H

#include <Arduino.h>
#include <IPAddress.h>
#include "../config.h"

// ================= STRUKTURY SESJI =================
struct SessionInfo {
    String token;
    IPAddress ip;
    unsigned long createdAt;
    unsigned long lastActivity;
    bool isValid;
};

// ================= FUNKCJE SESJI =================
void initializeSessions();
String createSession(IPAddress ip);
SessionInfo* findSessionByToken(const String& token);
void removeSession(const String& token);
void cleanupExpiredSessions();
int getActiveSessionCount();
String generateSessionToken();

// ================= ZMIENNE GLOBALNE =================
extern SessionInfo sessions[];
extern int sessionCount;
extern unsigned long lastSessionCleanup;

#endif