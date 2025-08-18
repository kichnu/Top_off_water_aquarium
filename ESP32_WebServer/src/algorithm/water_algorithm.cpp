#include "water_algorithm.h"
#include "../core/logging.h"
#include "../hardware/pump_controller.h"
#include "../hardware/water_sensors.h"
// #include <algorithm>
#include "../config/config.h"  // dla currentPumpSettings
#include "../config/hardware_pins.h"

WaterAlgorithm waterAlgorithm;

WaterAlgorithm::WaterAlgorithm() {
    currentState = STATE_IDLE;
    resetCycle();
    dayStartTime = millis();
    dailyVolumeML = 0;
    lastError = ERROR_NONE;
    errorSignalActive = false;
    lastSensor1State = false;
    lastSensor2State = false;
    
    pinMode(ERROR_SIGNAL_PIN, OUTPUT);
    digitalWrite(ERROR_SIGNAL_PIN, LOW);
    pinMode(RESET_PIN, INPUT_PULLUP);
}

void WaterAlgorithm::resetCycle() {
    currentCycle = {};
    currentCycle.timestamp = millis() / 1000; // Unix-like timestamp
    triggerStartTime = 0;
    sensor1TriggerTime = 0;
    sensor2TriggerTime = 0;
    sensor1ReleaseTime = 0;
    sensor2ReleaseTime = 0;
    pumpStartTime = 0;
    waitingForSecondSensor = false;
    pumpAttempts = 0;
}

void WaterAlgorithm::update() {
    // Check for reset button in error state
    if (currentState == STATE_ERROR) {
        if (digitalRead(RESET_PIN) == LOW) {
            LOG_INFO("Reset button pressed, clearing error");
            resetFromError();
        }
        updateErrorSignal();
        return;
    }
    
    // Daily volume reset (every 24h)
    if (millis() - dayStartTime > 86400000UL) { // 24 hours in ms
        dayStartTime = millis();
        dailyVolumeML = 0;
        todayCycles.clear();
        LOG_INFO("Daily volume counter reset");
    }
    
    uint32_t currentTime = millis();
    uint32_t stateElapsed = currentTime - stateStartTime;
    
    switch (currentState) {
        case STATE_IDLE:
            // Waiting for trigger - handled by onSensorStateChange
            break;
            
        case STATE_TRYB_1_WAIT:
            // Waiting for second sensor or timeout
            if (stateElapsed >= TIME_GAP_1_MAX * 1000) {
                // Timeout - proceed with max value
                currentCycle.time_gap_1 = TIME_GAP_1_MAX;
                LOG_INFO("TRYB_1: TIME_GAP_1 timeout, using max: %d", TIME_GAP_1_MAX);
                
                // Evaluate result
                if (sensor_time_match_function(currentCycle.time_gap_1, THRESHOLD_1)) {
                    currentCycle.sensor_results |= PumpCycle::RESULT_GAP1_FAIL;
                }
                
                // Move to delay before pump
                currentState = STATE_TRYB_1_DELAY;
                stateStartTime = currentTime;
                LOG_INFO("TRYB_1: Starting TIME_TO_PUMP delay (%ds)", TIME_TO_PUMP);
            }
            break;
            
        case STATE_TRYB_1_DELAY:
            // Wait TIME_TO_PUMP from original trigger
            if (currentTime - triggerStartTime >= TIME_TO_PUMP * 1000) {
                // Start pump
                LOG_INFO("TRYB_2: Starting pump attempt %d/%d", 
                        pumpAttempts + 1, PUMP_MAX_ATTEMPTS);
                
                pumpStartTime = currentTime;
                pumpAttempts++;
                
                // Calculate pump duration based on volumePerSecond
                uint16_t pumpDuration = (SINGLE_DOSE_VOLUME / currentPumpSettings.volumePerSecond);
                triggerPump(pumpDuration, "AUTO_PUMP");  //"AUTO_PUMP"
                
                currentCycle.pump_duration = pumpDuration / 1000;
                currentState = STATE_TRYB_2_PUMP;
                stateStartTime = currentTime;
            }
            break;
            
        case STATE_TRYB_2_PUMP:
            // Pump is running, wait for completion
            if (!isPumpActive()) {
                // Pump finished, check sensor response
                LOG_INFO("TRYB_2: Pump finished, checking sensors");
                currentState = STATE_TRYB_2_VERIFY;
                stateStartTime = currentTime;
            }
            break;
            
        case STATE_TRYB_2_VERIFY: {
            // Check if sensors deactivated (water level rose)
            bool sensorsOK = !readWaterSensor1() && !readWaterSensor2();
            
            if (sensorsOK) {
                // Success! Calculate water trigger time
                calculateWaterTrigger();
                LOG_INFO("TRYB_2: Sensors deactivated, water_trigger_time: %dms", 
                        currentCycle.water_trigger_time);
                
                // Check if we need to measure TIME_GAP_2
                if (!(currentCycle.sensor_results & PumpCycle::RESULT_GAP1_FAIL)) {
                    currentState = STATE_TRYB_2_WAIT_GAP2;
                    stateStartTime = currentTime;
                    waitingForSecondSensor = true;
                    LOG_INFO("TRYB_2: Waiting for TIME_GAP_2");
                } else {
                    // Skip TIME_GAP_2 measurement
                    currentState = STATE_LOGGING;
                    stateStartTime = currentTime;
                }
            } else if (stateElapsed >= WATER_TRIGGER_MAX_TIME * 1000) {
                // Timeout - sensors didn't respond
                currentCycle.water_trigger_time = WATER_TRIGGER_MAX_TIME;
                
                if (pumpAttempts < PUMP_MAX_ATTEMPTS) {
                    // Try again
                    LOG_WARNING("TRYB_2: Sensors didn't respond, retry %d/%d", 
                               pumpAttempts + 1, PUMP_MAX_ATTEMPTS);
                    currentState = STATE_TRYB_1_DELAY;
                    stateStartTime = currentTime - (TIME_TO_PUMP * 1000); // Immediate retry
                } else {
                    // All attempts failed
                    LOG_ERROR("TRYB_2: All pump attempts failed!");
                    currentCycle.error_code = ERROR_PUMP_FAILURE;
                    startErrorSignal(ERROR_PUMP_FAILURE);
                    currentState = STATE_ERROR;
                }
            }
            break;
        }    
            
        case STATE_TRYB_2_WAIT_GAP2: 
            // Waiting for TIME_GAP_2 measurement
            if (!waitingForSecondSensor || stateElapsed >= TIME_GAP_2_MAX * 1000) {
                if (stateElapsed >= TIME_GAP_2_MAX * 1000) {
                    currentCycle.time_gap_2 = TIME_GAP_2_MAX;
                    LOG_INFO("TRYB_2: TIME_GAP_2 timeout, using max: %d", TIME_GAP_2_MAX);
                }
                
                // Evaluate result
                if (sensor_time_match_function(currentCycle.time_gap_2, THRESHOLD_2)) {
                    currentCycle.sensor_results |= PumpCycle::RESULT_GAP2_FAIL;
                }
                
                currentState = STATE_LOGGING;
                stateStartTime = currentTime;
            }
            break;
            
        case STATE_LOGGING:
            // Log cycle and check daily limit
            logCycleComplete();
            
            // Check daily limit
            if (dailyVolumeML > FILL_WATER_MAX) {
                LOG_ERROR("Daily limit exceeded! %dml > %dml", dailyVolumeML, FILL_WATER_MAX);
                currentCycle.error_code = ERROR_DAILY_LIMIT;
                startErrorSignal(ERROR_DAILY_LIMIT);
                currentState = STATE_ERROR;
            } else {
                // Return to idle after logging time
                if (stateElapsed >= LOGGING_TIME * 1000) {
                    LOG_INFO("Cycle complete, returning to IDLE");
                    currentState = STATE_IDLE;
                    resetCycle();
                }
            }
            break;
            
        case STATE_MANUAL_OVERRIDE:
            // Handled by onManualPumpComplete
            break;
            
        case STATE_ERROR:
            // Handled at beginning of update()
            break;
    }
}

void WaterAlgorithm::onSensorStateChange(uint8_t sensorNum, bool triggered) {
    uint32_t currentTime = millis();
    
    // Update sensor states
    if (sensorNum == 1) {
        lastSensor1State = triggered;
        if (triggered) {
            sensor1TriggerTime = currentTime;
        } else {
            sensor1ReleaseTime = currentTime;
        }
    } else if (sensorNum == 2) {
        lastSensor2State = triggered;
        if (triggered) {
            sensor2TriggerTime = currentTime;
        } else {
            sensor2ReleaseTime = currentTime;
        }
    }
    
    // Handle state transitions based on sensor changes
    switch (currentState) {
        case STATE_IDLE:
            if (triggered && (lastSensor1State || lastSensor2State)) {
                // TRIGGER detected!
                LOG_INFO("TRIGGER detected! Starting TRYB_1");
                triggerStartTime = currentTime;
                currentCycle.trigger_time = currentTime;
                currentState = STATE_TRYB_1_WAIT;
                stateStartTime = currentTime;
                waitingForSecondSensor = true;
                
                // Record which sensor triggered first
                if (sensorNum == 1 && !sensor2TriggerTime) {
                    LOG_INFO("Sensor 1 triggered first");
                } else if (sensorNum == 2 && !sensor1TriggerTime) {
                    LOG_INFO("Sensor 2 triggered first");
                }
            }
            break;
            
        case STATE_TRYB_1_WAIT:
            if (waitingForSecondSensor && sensor1TriggerTime && sensor2TriggerTime) {
                // Both sensors triggered, calculate TIME_GAP_1
                calculateTimeGap1();
                waitingForSecondSensor = false;
                
                // Evaluate result
                if (sensor_time_match_function(currentCycle.time_gap_1, THRESHOLD_1)) {
                    currentCycle.sensor_results |= PumpCycle::RESULT_GAP1_FAIL;
                }
                
                // Continue waiting for TIME_TO_PUMP
                currentState = STATE_TRYB_1_DELAY;
                stateStartTime = currentTime;
                LOG_INFO("TRYB_1: Both sensors triggered, TIME_GAP_1=%dms", 
                        currentCycle.time_gap_1);
            }
            break;
            
        case STATE_TRYB_2_WAIT_GAP2:
            if (!triggered && waitingForSecondSensor && 
                sensor1ReleaseTime && sensor2ReleaseTime) {
                // Both sensors released, calculate TIME_GAP_2
                calculateTimeGap2();
                waitingForSecondSensor = false;
                LOG_INFO("TRYB_2: TIME_GAP_2=%dms", currentCycle.time_gap_2);
            }
            break;
            
        default:
            // Ignore sensor changes in other states
            break;
    }
}

void WaterAlgorithm::calculateTimeGap1() {
    if (sensor1TriggerTime && sensor2TriggerTime) {
        currentCycle.time_gap_1 = abs((int32_t)sensor2TriggerTime - 
                                      (int32_t)sensor1TriggerTime) / 1000;
    }
}

void WaterAlgorithm::calculateTimeGap2() {
    if (sensor1ReleaseTime && sensor2ReleaseTime) {
        currentCycle.time_gap_2 = abs((int32_t)sensor2ReleaseTime - 
                                      (int32_t)sensor1ReleaseTime) / 1000;
    }
}

void WaterAlgorithm::calculateWaterTrigger() {
    uint32_t earliestRelease = 0;
    
    if (sensor1ReleaseTime > pumpStartTime) {
        earliestRelease = sensor1ReleaseTime;
    }
    if (sensor2ReleaseTime > pumpStartTime && 
        (earliestRelease == 0 || sensor2ReleaseTime < earliestRelease)) {
        earliestRelease = sensor2ReleaseTime;
    }
    
    if (earliestRelease > 0) {
        currentCycle.water_trigger_time = (earliestRelease - pumpStartTime) / 1000;
        
        // Evaluate result
        if (sensor_time_match_function(currentCycle.water_trigger_time, THRESHOLD_WATER)) {
            currentCycle.sensor_results |= PumpCycle::RESULT_WATER_FAIL;
        }
    }
}

void WaterAlgorithm::logCycleComplete() {
    // Calculate volume
    currentCycle.volume_dose = SINGLE_DOSE_VOLUME / 10; // Store in 10ml units
    currentCycle.pump_attempts = pumpAttempts;
    
    // Add to daily volume
    dailyVolumeML += SINGLE_DOSE_VOLUME;
    
    // Store in today's cycles
    todayCycles.push_back(currentCycle);
    
    // Keep only last 100 cycles in RAM
    if (todayCycles.size() > 100) {
        todayCycles.erase(todayCycles.begin());
    }
    
    LOG_INFO("=== CYCLE COMPLETE ===");
    LOG_INFO("TIME_GAP_1: %ds (fail=%d)", currentCycle.time_gap_1, 
            (currentCycle.sensor_results & PumpCycle::RESULT_GAP1_FAIL) ? 1 : 0);
    LOG_INFO("TIME_GAP_2: %ds (fail=%d)", currentCycle.time_gap_2,
            (currentCycle.sensor_results & PumpCycle::RESULT_GAP2_FAIL) ? 1 : 0);
    LOG_INFO("WATER_TRIGGER: %ds (fail=%d)", currentCycle.water_trigger_time,
            (currentCycle.sensor_results & PumpCycle::RESULT_WATER_FAIL) ? 1 : 0);
    LOG_INFO("Daily volume: %dml / %dml", dailyVolumeML, FILL_WATER_MAX);
}

bool WaterAlgorithm::requestManualPump(uint16_t duration_ms) {
    if (currentState == STATE_ERROR) {
        LOG_WARNING("Cannot start manual pump in error state");
        return false;
    }
    
    if (currentState != STATE_IDLE) {
        // Interrupt current cycle
        LOG_INFO("Manual pump interrupting current cycle");
        currentState = STATE_MANUAL_OVERRIDE;
        resetCycle();
    }
    
    // Manual pump handled by pump_controller
    return true;
}

void WaterAlgorithm::onManualPumpComplete() {
    if (currentState == STATE_MANUAL_OVERRIDE) {
        LOG_INFO("Manual pump complete, returning to IDLE");
        currentState = STATE_IDLE;
        resetCycle();
    }
}

const char* WaterAlgorithm::getStateString() const {
    switch (currentState) {
        case STATE_IDLE: return "IDLE";
        case STATE_TRYB_1_WAIT: return "TRYB_1_WAIT";
        case STATE_TRYB_1_DELAY: return "TRYB_1_DELAY";
        case STATE_TRYB_2_PUMP: return "TRYB_2_PUMP";
        case STATE_TRYB_2_VERIFY: return "TRYB_2_VERIFY";
        case STATE_TRYB_2_WAIT_GAP2: return "TRYB_2_WAIT_GAP2";
        case STATE_LOGGING: return "LOGGING";
        case STATE_ERROR: return "ERROR";
        case STATE_MANUAL_OVERRIDE: return "MANUAL_OVERRIDE";
        default: return "UNKNOWN";
    }
}

bool WaterAlgorithm::isInCycle() const {
    return currentState != STATE_IDLE && currentState != STATE_ERROR;
}

void WaterAlgorithm::getAggregateData(uint8_t& xx, uint8_t& yy, uint8_t& zz, uint16_t& vvvv) {
    // Sum failures from recent cycles (simulating 14 days with available data)
    xx = yy = zz = 0;
    
    for (const auto& cycle : todayCycles) {
        if (cycle.sensor_results & PumpCycle::RESULT_GAP1_FAIL) xx++;
        if (cycle.sensor_results & PumpCycle::RESULT_GAP2_FAIL) yy++;
        if (cycle.sensor_results & PumpCycle::RESULT_WATER_FAIL) zz++;
    }
    
    // Cap at 99
    if (xx > 99) xx = 99;
    if (yy > 99) yy = 99;
    if (zz > 99) zz = 99;
    
    // Daily volume
    vvvv = dailyVolumeML;
}

std::vector<PumpCycle> WaterAlgorithm::getRecentCycles(size_t count) {
    size_t start = todayCycles.size() > count ? todayCycles.size() - count : 0;
    return std::vector<PumpCycle>(todayCycles.begin() + start, todayCycles.end());
}

void WaterAlgorithm::startErrorSignal(ErrorCode error) {
    lastError = error;
    errorSignalActive = true;
    errorSignalStart = millis();
    errorPulseCount = 0;
    errorPulseState = false;
    digitalWrite(ERROR_SIGNAL_PIN, LOW);
    
    LOG_ERROR("Starting error signal: %s", 
             error == ERROR_DAILY_LIMIT ? "ERR1" :
             error == ERROR_PUMP_FAILURE ? "ERR2" : "ERR0");
}

void WaterAlgorithm::updateErrorSignal() {
    if (!errorSignalActive) return;
    
    uint32_t elapsed = millis() - errorSignalStart;
    uint8_t pulsesNeeded = (lastError == ERROR_DAILY_LIMIT) ? 1 :
                           (lastError == ERROR_PUMP_FAILURE) ? 2 : 3;
    
    // Calculate current position in signal pattern
    uint32_t cycleTime = 0;
    for (uint8_t i = 0; i < pulsesNeeded; i++) {
        cycleTime += ERROR_PULSE_HIGH + ERROR_PULSE_LOW;
    }
    cycleTime += ERROR_PAUSE - ERROR_PULSE_LOW; // Remove last LOW, add PAUSE
    
    uint32_t posInCycle = elapsed % cycleTime;
    
    // Determine if we should be HIGH or LOW
    bool shouldBeHigh = false;
    uint32_t currentPos = 0;
    
    for (uint8_t i = 0; i < pulsesNeeded; i++) {
        if (posInCycle >= currentPos && posInCycle < currentPos + ERROR_PULSE_HIGH) {
            shouldBeHigh = true;
            break;
        }
        currentPos += ERROR_PULSE_HIGH + ERROR_PULSE_LOW;
    }
    
    // Update pin state
    if (shouldBeHigh != errorPulseState) {
        errorPulseState = shouldBeHigh;
        digitalWrite(ERROR_SIGNAL_PIN, errorPulseState ? HIGH : LOW);
    }
}

void WaterAlgorithm::resetFromError() {
    lastError = ERROR_NONE;
    errorSignalActive = false;
    digitalWrite(ERROR_SIGNAL_PIN, LOW);
    currentState = STATE_IDLE;
    resetCycle();
    LOG_INFO("System reset from error state");
}