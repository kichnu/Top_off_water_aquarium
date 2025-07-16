#ifndef COMMUNICATION_UART_PROTOCOL_H
#define COMMUNICATION_UART_PROTOCOL_H

#include <Arduino.h>
#include <ArduinoJson.h>

// ================= FUNKCJE GŁÓWNE =================
void initializeUARTProtocol();
void processUARTProtocol();

// ================= KOMUNIKACJA =================
void sendUARTResponse(uint32_t commandId, const String& status, const String& result = "");
void sendPong();

// ================= PRZETWARZANIE KOMEND =================
void processCommand(const String& jsonCommand);
void processCommandType(uint32_t commandId, JsonDocument& doc);

// ================= OBSŁUGA KONKRETNYCH KOMEND =================
void handleManualPump(uint32_t commandId, JsonDocument& doc);
void handleSetPumpConfig(uint32_t commandId, JsonDocument& doc);
void handleGetWaterStatus(uint32_t commandId, JsonDocument& doc);
void handleGetRecentLogs(uint32_t commandId, JsonDocument& doc);
void handleGetSystemStatus(uint32_t commandId, JsonDocument& doc);

// ================= PROTOKÓŁ JSON + CRC16 =================
String createResponseJSON(uint32_t commandId, const String& status, const String& result = "");
uint16_t calculateCRC16(const String& data);
bool validateCRC(const String& jsonStr, const String& expectedCrc);

// ================= STATUS I DIAGNOSTYKA =================
bool isWebServerConnected();
String getLastUARTError();
void debugUARTProtocol();

// ================= ZMIENNE GLOBALNE =================
extern String uartBuffer;
extern unsigned long lastHeartbeat;
extern bool webServerConnected;
extern String lastUARTError;

#endif