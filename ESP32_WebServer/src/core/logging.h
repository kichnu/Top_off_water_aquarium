#ifndef CORE_LOGGING_H
#define CORE_LOGGING_H

#include <Arduino.h>
#include <IPAddress.h>

// ================= POZIOMY LOGOWANIA =================
enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3
};

// ================= FUNKCJE LOGOWANIA =================
void initializeLogging();
void logMessage(LogLevel level, const char* module, const char* message);
void logSystemEvent(const char* event);
void logSecurityEvent(const char* event, IPAddress ip);
void logHardwareEvent(const char* device, const char* action);

// ================= MAKRA POMOCNICZE =================
#define LOG_DEBUG_MSG(module, msg) logMessage(LOG_DEBUG, module, msg)
#define LOG_INFO_MSG(module, msg) logMessage(LOG_INFO, module, msg)
#define LOG_WARNING_MSG(module, msg) logMessage(LOG_WARNING, module, msg)
#define LOG_ERROR_MSG(module, msg) logMessage(LOG_ERROR, module, msg)

// ================= ZMIENNE GLOBALNE =================
extern LogLevel currentLogLevel;
extern bool loggingEnabled;

#endif