#ifndef WEB_HANDLERS_H
#define WEB_HANDLERS_H

#include <ESPAsyncWebServer.h>

void handleDashboard(AsyncWebServerRequest* request);
void handleLoginPage(AsyncWebServerRequest* request);
void handleLogin(AsyncWebServerRequest* request);
void handleLogout(AsyncWebServerRequest* request);
void handleStatus(AsyncWebServerRequest* request);
void handlePumpNormal(AsyncWebServerRequest* request);
void handlePumpExtended(AsyncWebServerRequest* request);
void handlePumpStop(AsyncWebServerRequest* request);
void handlePumpSettings(AsyncWebServerRequest* request);

#endif