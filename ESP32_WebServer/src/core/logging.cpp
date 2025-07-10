#include "logging.h"

// ================= ZMIENNE GLOBALNE =================
LogLevel currentLogLevel = LOG_INFO;
bool loggingEnabled = true;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja systemu logowania
 */
void initializeLogging() {
    Serial.begin(115200);
    delay(2000);
    loggingEnabled = true;
    currentLogLevel = LOG_INFO;
}

/**
 * Główna funkcja logowania
 */
void logMessage(LogLevel level, const char* module, const char* message) {
    if (!loggingEnabled || level < currentLogLevel) {
        return;
    }
    
    const char* levelStr;
    switch (level) {
        case LOG_DEBUG:   levelStr = "DEBUG"; break;
        case LOG_INFO:    levelStr = "INFO";  break;
        case LOG_WARNING: levelStr = "WARN";  break;
        case LOG_ERROR:   levelStr = "ERROR"; break;
        default:          levelStr = "UNKNOWN"; break;
    }
    
    Serial.printf("[%s] [%s] %s\n", levelStr, module, message);
}

/**
 * Logowanie wydarzeń systemowych
 */
void logSystemEvent(const char* event) {
    logMessage(LOG_INFO, "SYSTEM", event);
}

/**
 * Logowanie wydarzeń związanych z bezpieczeństwem
 */
void logSecurityEvent(const char* event, IPAddress ip) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s - IP: %s", event, ip.toString().c_str());
    logMessage(LOG_WARNING, "SECURITY", buffer);
}

/**
 * Logowanie wydarzeń sprzętowych
 */
void logHardwareEvent(const char* device, const char* action) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s: %s", device, action);
    logMessage(LOG_INFO, "HARDWARE", buffer);
}