#ifndef API_H
#define API_H

#include <ESPAsyncWebServer.h>

// Authentication endpoints
void handleLogin(AsyncWebServerRequest* request);
void handleLogout(AsyncWebServerRequest* request);

// Status endpoints
void handleStatus(AsyncWebServerRequest* request);

// Pump control endpoints
void handleManualPumpNormal(AsyncWebServerRequest* request);
void handleManualPump1Min(AsyncWebServerRequest* request);

// Settings endpoints
void handleGetSettings(AsyncWebServerRequest* request);
void handleUpdateSettings(AsyncWebServerRequest* request);

#endif