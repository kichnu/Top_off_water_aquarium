#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include "../config.h"

// ================= FUNKCJE SERWERA =================
void initializeWebServer();
void startWebServer();
void stopWebServer();
void setupRoutes();

// ================= OBSŁUGA ŻĄDAŃ =================
void handleNotFound(AsyncWebServerRequest* request);
void handleRoot(AsyncWebServerRequest* request);
void handleLogin(AsyncWebServerRequest* request);
void handleLogout(AsyncWebServerRequest* request);

// ================= ZMIENNE GLOBALNE =================
extern AsyncWebServer server;
extern bool serverRunning;
extern unsigned long serverStartTime;

#endif