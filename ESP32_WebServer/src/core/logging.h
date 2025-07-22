#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>

enum LogLevel {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_DEBUG = 3
};

void initializeLogging();
void setLogLevel(LogLevel level);

#define LOG_ERROR(format, ...) logMessage(LOG_LEVEL_ERROR, "[ERROR] " format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) logMessage(LOG_LEVEL_WARNING, "[WARN] " format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) logMessage(LOG_LEVEL_INFO, "[INFO] " format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) logMessage(LOG_LEVEL_DEBUG, "[DEBUG] " format, ##__VA_ARGS__)

void logMessage(LogLevel level, const char* format, ...);

#endif