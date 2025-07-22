#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>

void initLogging();
void logInfo(const char* format, ...);
void logWarning(const char* format, ...);
void logError(const char* format, ...);

#define LOG_INFO(format, ...) logInfo("[INFO] " format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) logWarning("[WARN] " format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) logError("[ERROR] " format, ##__VA_ARGS__)

#endif