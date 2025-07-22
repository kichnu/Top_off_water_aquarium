#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <ESPAsyncWebServer.h>

void handleDashboard(AsyncWebServerRequest* request);
void handleLoginPage(AsyncWebServerRequest* request);
void handleSettingsPage(AsyncWebServerRequest* request);

String getDashboardHTML();
String getLoginHTML();
String getSettingsHTML();

#endif



