#ifndef SECURITY_RATELIMIT_H
#define SECURITY_RATELIMIT_H

#include <IPAddress.h>
#include "../config.h"

// ================= STRUKTURY RATE LIMITING =================
struct ClientInfo {
    IPAddress ip;
    unsigned long lastRequest;
    unsigned long blockedUntil;
    int failedAttempts;
    int requestCount;
    unsigned long requestWindow;
};

// ================= FUNKCJE RATE LIMITING =================
void initializeRateLimit();
bool checkRateLimit(IPAddress ip);
void recordFailedLogin(IPAddress ip);
void resetFailedAttempts(IPAddress ip);
ClientInfo* findOrCreateClient(IPAddress ip);
void cleanupOldClients();

// ================= ZMIENNE GLOBALNE =================
extern ClientInfo clients[];
extern int clientCount;
extern unsigned long lastClientCleanup;

#endif