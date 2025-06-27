#ifndef SECURITY_AUTH_H
#define SECURITY_AUTH_H

#include <ESPAsyncWebServer.h>
#include "../config.h"

// ================= FUNKCJE AUTORYZACJI =================
bool checkAuthentication(AsyncWebServerRequest* request);
bool verifyPassword(const String& password);
bool isIPAllowed(IPAddress ip);

// ================= OBSŁUGA LOGOWANIA =================
bool processLogin(AsyncWebServerRequest* request);
void processLogout(AsyncWebServerRequest* request);

// ================= ZMIENNE GLOBALNE =================
extern bool authenticationEnabled;
extern unsigned long lastAuthCheck;

#endif