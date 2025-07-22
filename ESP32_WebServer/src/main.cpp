#include <Arduino.h>
#include <WiFi.h>
#include "config/hardware_pins.h"
#include "config/settings.h"
#include "hardware/water_sensor.h"
#include "hardware/pump_controller.h"
#include "hardware/rtc_ds3231.h"
#include "network/wifi.h"
#include "network/ntp.h"
#include "network/vps_logger.h"
#include "security/auth.h"
#include "security/sessions.h"
#include "security/ratelimit.h"
#include "web/server.h"
#include "core/logging.h"
#include "core/system.h"


#define LED_BUILTIN 2

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_BUILTIN, OUTPUT);
    
    LOG_INFO("=== ESP32-C6 Unified Water System Starting ===");
    
    // Initialize core systems
    initializeLogging();
    initializeSettings();
    initializeSystem();
    
    // Initialize hardware
    initializeWaterSensors();
    initializePumpController();
    initializeRTC();
    
    // Debug RTC status - simplified version
    Serial.println("[SYSTEM] RTC initialized");
    Serial.printf("[SYSTEM] Current time: %s\n", getRTCTimestamp().c_str());
    // Serial.printf("[SYSTEM] RTC Temperature: %.2f°C\n", getRTCTemperature()); // Commented out temporarily
    
    // Initialize security
    initializeAuth();
    initializeSessions();
    initializeRateLimit();
    
    // Initialize network
    initializeWiFi();
    initializeNTP();
    initializeVPSLogger();
    
    // Initialize web server
    initializeWebServer();
    
    LOG_INFO("=== System initialization complete ===");
    LOG_INFO("IP Address: %s", WiFi.localIP().toString().c_str());
}

void loop() {
    // Core system tasks
    processDualWaterSensors();
    updatePumpController();
    updateSessions();
    updateRateLimit();
    updateSystem();
    
    // Network tasks
    updateNTP();
    
    // Debug RTC every 10 minutes - simplified
    static unsigned long lastRTCDebug = 0;
    if (millis() - lastRTCDebug > 600000) { // 10 minutes
        Serial.printf("[SYSTEM] RTC Status - Time: %s\n", getRTCTimestamp().c_str());
        lastRTCDebug = millis();
    }
    Serial.printf("sdujfhgbvijkuaefghbuiejrak");
    digitalWrite(LED_BUILTIN, HIGH);
    
    
    delay(100);
    
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}