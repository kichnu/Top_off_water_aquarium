#include "logging.h"
#include <stdarg.h>


void initLogging() {
    Serial.begin(115200);
    delay(1000);
    LOG_INFO("Logging system initialized");
}

void logInfo(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.printf("[%lu] %s\n", millis(), buffer);
}

void logWarning(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.printf("[%lu] %s\n", millis(), buffer);
}

void logError(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.printf("[%lu] %s\n", millis(), buffer);
}