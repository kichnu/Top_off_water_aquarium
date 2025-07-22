#include "pump_controller.h"
#include "../config/hardware_pins.h"
#include "../config/settings.h"
#include "../network/vps_logger.h"
#include "../hardware/rtc_ds3231.h"
#include "../core/logging.h"

static bool pump_running = false;
static unsigned long pump_start_time = 0;
static unsigned long pump_duration = 0;
static String current_action_type = "";

void initializePumpController() {
    pinMode(PUMP_RELAY_PIN, OUTPUT);
    digitalWrite(PUMP_RELAY_PIN, LOW);  // Pump OFF
    
    pump_running = false;
    pump_start_time = 0;
    pump_duration = 0;
    
    LOG_INFO("Pump controller initialized");
}

void updatePumpController() {
    if (pump_running && (millis() - pump_start_time >= pump_duration)) {
        // Stop pump
        digitalWrite(PUMP_RELAY_PIN, LOW);
        pump_running = false;
        
        PumpConfig config = getPumpConfig();
        uint16_t volume_ml = (pump_duration / 1000) * (config.volumeML / config.pumpTimeSeconds);
        
        LOG_INFO("Pump stopped after %lu seconds, estimated volume: %d ml", 
                 pump_duration / 1000, volume_ml);
        
        // Log to VPS
        String timestamp = getRTCTimestamp();
        logToVPS(current_action_type, volume_ml, timestamp);
        
        current_action_type = "";
    }
}

bool triggerAutomaticPump() {
    if (pump_running) {
        LOG_WARNING("Pump already running, ignoring auto trigger");
        return false;
    }
    
    PumpConfig config = getPumpConfig();
    if (!config.autoPumpEnabled) {
        LOG_INFO("Auto pump disabled in settings");
        return false;
    }
    
    return triggerManualPump(config.pumpTimeSeconds, "AUTO_PUMP");
}

bool triggerManualPump(uint16_t duration_seconds, const String& action_type) {
    if (pump_running) {
        LOG_WARNING("Pump already running, ignoring manual trigger");
        return false;
    }
    
    // Start pump
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    pump_running = true;
    pump_start_time = millis();
    pump_duration = duration_seconds * 1000UL;
    current_action_type = action_type;
    
    LOG_INFO("Pump started: %s for %d seconds", action_type.c_str(), duration_seconds);
    
    return true;
}

bool isPumpRunning() {
    return pump_running;
}

uint32_t getPumpRemainingTime() {
    if (!pump_running) return 0;
    
    unsigned long elapsed = millis() - pump_start_time;
    if (elapsed >= pump_duration) return 0;
    
    return (pump_duration - elapsed) / 1000;
}