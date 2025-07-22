#include "logging.h"
#include <stdarg.h>

static LogLevel current_log_level = LOG_LEVEL_INFO;

void initializeLogging() {
    Serial.begin(115200);
    current_log_level = LOG_LEVEL_INFO;
    LOG_INFO("Logging system initialized");
}

void setLogLevel(LogLevel level) {
    current_log_level = level;
}

void logMessage(LogLevel level, const char* format, ...) {
    if (level > current_log_level) {
        return;
    }
    
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    unsigned long timestamp = millis();
    Serial.printf("[%lu] %s\n", timestamp, buffer);
}