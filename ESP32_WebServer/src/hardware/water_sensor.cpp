#include "water_sensor.h"
#include "../config/hardware_pins.h"
#include "../hardware/pump_controller.h"
#include "../core/logging.h"

SensorState sensor1_state = {false, false, 0, false};
SensorState sensor2_state = {false, false, 0, false};

void initializeWaterSensors() {
    pinMode(WATER_SENSOR1_PIN, INPUT_PULLUP);
    pinMode(WATER_SENSOR2_PIN, INPUT_PULLUP);
    
    // Initialize states
    sensor1_state.current_value = (digitalRead(WATER_SENSOR1_PIN) == LOW);
    sensor2_state.current_value = (digitalRead(WATER_SENSOR2_PIN) == LOW);
    sensor1_state.last_stable_value = sensor1_state.current_value;
    sensor2_state.last_stable_value = sensor2_state.current_value;
    sensor1_state.is_stable = true;
    sensor2_state.is_stable = true;
    
    LOG_INFO("Water sensors initialized - Sensor1: %s, Sensor2: %s", 
             sensor1_state.current_value ? "LOW" : "OK",
             sensor2_state.current_value ? "LOW" : "OK");
}

void processDualWaterSensors() {
    unsigned long now = millis();
    
    // Read both sensors (LOW = water needed)
    bool sensor1_raw = (digitalRead(WATER_SENSOR1_PIN) == LOW);
    bool sensor2_raw = (digitalRead(WATER_SENSOR2_PIN) == LOW);
    
    // Individual debouncing for sensor1
    if (sensor1_raw != sensor1_state.current_value) {
        sensor1_state.current_value = sensor1_raw;
        sensor1_state.last_change_time = now;
        sensor1_state.is_stable = false;
        LOG_DEBUG("Sensor1 change detected: %s", sensor1_raw ? "LOW" : "OK");
    } else if (now - sensor1_state.last_change_time > DEBOUNCE_DELAY_MS) {
        if (!sensor1_state.is_stable) {
            sensor1_state.last_stable_value = sensor1_raw;
            sensor1_state.is_stable = true;
            LOG_INFO("Sensor1 stabilized: %s", sensor1_raw ? "LOW" : "OK");
        }
    }
    
    // Individual debouncing for sensor2  
    if (sensor2_raw != sensor2_state.current_value) {
        sensor2_state.current_value = sensor2_raw;
        sensor2_state.last_change_time = now;
        sensor2_state.is_stable = false;
        LOG_DEBUG("Sensor2 change detected: %s", sensor2_raw ? "LOW" : "OK");
    } else if (now - sensor2_state.last_change_time > DEBOUNCE_DELAY_MS) {
        if (!sensor2_state.is_stable) {
            sensor2_state.last_stable_value = sensor2_raw;
            sensor2_state.is_stable = true;
            LOG_INFO("Sensor2 stabilized: %s", sensor2_raw ? "LOW" : "OK");
        }
    }
    
    // Decision making only when both sensors are stable
    if (sensor1_state.is_stable && sensor2_state.is_stable) {
        bool should_pump = (sensor1_state.last_stable_value && sensor2_state.last_stable_value);
        
        if (should_pump && shouldTriggerPump()) {
            LOG_INFO("Auto pump triggered - both sensors LOW");
            triggerAutomaticPump();
        }
    }
}

String getCombinedWaterStatus() {
    if (!sensor1_state.is_stable || !sensor2_state.is_stable) {
        return "CHECKING";
    }
    
    bool water_needed = (sensor1_state.last_stable_value && sensor2_state.last_stable_value);
    return water_needed ? "LOW" : "OK";
}

bool isWaterNeeded() {
    return sensor1_state.is_stable && sensor2_state.is_stable &&
           sensor1_state.last_stable_value && sensor2_state.last_stable_value;
}

bool shouldTriggerPump() {
    // TODO: Future expansion point for check_permission()
    // - Daily volume limits
    // - Time-based restrictions (night mode)
    // - Minimum intervals between pumping
    // - Maintenance mode locks
    // - Seasonal adjustments
    return true; // HARDCODED for initial implementation - EXPAND HERE LATER
}