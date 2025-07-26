#include <Arduino.h>
#include "config/config.h"
#include "core/logging.h"
#include "hardware/rtc_controller.h"
#include "hardware/water_sensors.h"
#include "hardware/pump_controller.h"
#include "network/wifi_manager.h"
#include "network/vps_logger.h"
#include "security/auth_manager.h"
#include "security/session_manager.h"
#include "security/rate_limiter.h"
#include "web/web_server.h"



#define STATUS_LED_PIN 2

// Global pump settings


void setup() {
    // Initialize core systems

    delay(6000);
    
    
    
    
    initLogging();
    
    LOG_INFO("=== ESP32-C3 Water System Starting ===");
    LOG_INFO("Device ID: %s", DEVICE_ID);
    
    // Initialize hardware
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, HIGH); 
    initWaterSensors();
    initPumpController();
    
    // initRTC();
    initializeRTC();
    
    // Initialize security
    initAuthManager();
    initSessionManager();
    initRateLimiter();
    
    // Initialize network
    initWiFi();
    initVPSLogger();
    
    // Initialize web server
    initWebServer();
    
    digitalWrite(STATUS_LED_PIN, LOW);
    
    LOG_INFO("=== System initialization complete ===");
    if (isWiFiConnected()) {
        LOG_INFO("Dashboard: http://%s", getLocalIP().toString().c_str());
    }
    // LOG_INFO("Current time: %s", getCurrentTimestamp().c_str());
}

void loop() {


    static unsigned long lastUpdate = 0;
    unsigned long now = millis();
    
    // Update water sensors every loop
    updateWaterSensors();
    
    // Update other systems every 100ms
    if (now - lastUpdate >= 100) {
        updatePumpController();
        updateSessionManager();
        updateRateLimiter();
        updateWiFi();
        
        // Check for auto pump trigger
        if (currentPumpSettings.autoModeEnabled && shouldActivatePump() && !isPumpActive()) {
            LOG_INFO("Auto pump triggered - water level low");
            triggerPump(currentPumpSettings.normalCycleSeconds, "AUTO_PUMP");
        }
        
        lastUpdate = now;
    }
    
    // Status LED heartbeat (slow blink when OK, fast when issues)
    static unsigned long lastBlink = 0;
    unsigned long blinkInterval = (isWiFiConnected() && isRTCWorking()) ? 2000 : 500;
    
    if (now - lastBlink >= blinkInterval) {
        digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
        lastBlink = now;
    }
    
    delay(100); // Small delay to prevent watchdog issues
}