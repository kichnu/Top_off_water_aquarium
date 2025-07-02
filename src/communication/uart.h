#ifndef COMMUNICATION_UART_H
#define COMMUNICATION_UART_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "../config.h"

// ================= STRUKTURY KOMUNIKACJI =================
struct UARTCommand {
    uint32_t id;
    String type;
    String action;
    String params;
    String crc;
    unsigned long timestamp;
    int retryCount;
    bool pending;
};

struct UARTResponse {
    uint32_t id;
    String type;
    String status;
    String result;
    String sensors;
    String crc;
    bool valid;
};

// ================= STAŁE =================
const int MAX_PENDING_COMMANDS = 10;
const unsigned long COMMAND_TIMEOUT = 2000;  // 2 sekundy
const int MAX_RETRIES = 3;
const unsigned long HEARTBEAT_INTERVAL = 60000; // 1 minuta
const int UART_BUFFER_SIZE = 512;

// ================= FUNKCJE UART =================
void initializeUART();
bool sendCommand(const String& action, const String& params);
bool sendHeartbeat();
void processUARTData();
UARTResponse getLastResponse(uint32_t commandId);
bool isIoTConnected();
void checkCommandTimeouts();
uint16_t calculateCRC16(const String& data);
bool validateCRC(const String& jsonStr, const String& expectedCrc);

// ================= LED CONTROL FUNCTIONS (Moved from actuators) =================
void setLEDState(bool state);
bool getLEDState();
void requestStateSync();
void processSyncResponse(const String& result);
void updateStateFromResponse(const String& action, const String& result);

// ================= HELPER FUNCTIONS =================
String createCommandJSON(const String& action, const String& params);
bool parseResponseJSON(const String& jsonStr, UARTResponse& response);
uint32_t generateCommandId();
void cleanupOldCommands();

// ================= STATUS FUNCTIONS =================
int getPendingCommandsCount();
unsigned long getLastHeartbeat();
String getConnectionStatus();
String getLastError();

// ================= ZMIENNE GLOBALNE =================
extern UARTCommand pendingCommands[MAX_PENDING_COMMANDS];
extern int pendingCommandsCount;
extern unsigned long lastHeartbeat;
extern bool iotConnected;
extern String lastError;
extern String uartBuffer;
extern bool ledState; // LED state cache (moved from actuators)

#endif