#ifndef WEB_API_H
#define WEB_API_H

#include <ESPAsyncWebServer.h>
#include "../config.h"

// ================= API SETUP =================
void setupAPIRoutes();

// ================= LED API =================
void handleLEDStatus(AsyncWebServerRequest* request);
void handleLEDToggle(AsyncWebServerRequest* request);

// ================= STATUS API =================
void handleBasicStatus(AsyncWebServerRequest* request);

// ================= POMOCNICZE =================
void sendJSONError(AsyncWebServerRequest* request, const String& error);
void sendJSONSuccess(AsyncWebServerRequest* request, const String& data);
void sendJSONWithIoTStatus(AsyncWebServerRequest* request, const String& data);

#endif