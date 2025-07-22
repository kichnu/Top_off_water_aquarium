#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void initializeWebServer();
bool checkAuthentication(AsyncWebServerRequest* request);
void setupRoutes();

extern AsyncWebServer server;

#endif