#ifndef WEB_API_H
#define WEB_API_H

#include <ESPAsyncWebServer.h>
#include "../config.h"

// ================= API SETUP =================
void setupAPIRoutes();

// ================= LED API =================
void handleLEDStatus(AsyncWebServerRequest* request);
void handleLEDToggle(AsyncWebServerRequest* request);

// ================= SYSTEM API =================
void handleSystemInfo(AsyncWebServerRequest* request);

// ================= UART API (NOWE) =================
void handleUARTStatus(AsyncWebServerRequest* request);

// *** ROZSZERZENIE: Hardware API (odkomentuj według potrzeb) ***
// ============= PRZEKAŹNIKI API =============
/*
void handleRelay1Toggle(AsyncWebServerRequest* request);
void handleRelay2Toggle(AsyncWebServerRequest* request);
void handleRelay3Toggle(AsyncWebServerRequest* request);
*/

// ============= SERWOMECHANIZMY API =============
/*
void handleServo1Set(AsyncWebServerRequest* request);
void handleServo2Set(AsyncWebServerRequest* request);
*/

// ============= PWM/DIMMER API =============
/*
void handleLEDStripSet(AsyncWebServerRequest* request);
void handleFanSet(AsyncWebServerRequest* request);
*/

// ============= SENSORY API =================
/*
void handleSensorsRead(AsyncWebServerRequest* request);
*/

// ============= HARMONOGRAMY API =============
/*
void handleSchedulesList(AsyncWebServerRequest* request);
void handleScheduleAdd(AsyncWebServerRequest* request);
void handleScheduleRemove(AsyncWebServerRequest* request);
*/

// ================= POMOCNICZE =================
String createJSONResponse(const String& data);
void sendJSONError(AsyncWebServerRequest* request, const String& error);
void sendJSONSuccess(AsyncWebServerRequest* request, const String& data);
void sendJSONWithIoTStatus(AsyncWebServerRequest* request, const String& data); // NOWE

#endif