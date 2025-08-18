#ifndef WATER_ALGORITHM_H
#define WATER_ALGORITHM_H

#include "algorithm_config.h"
#include <vector>

class WaterAlgorithm {
private:
    AlgorithmState currentState;
    PumpCycle currentCycle;
    
    // Timing variables
    uint32_t stateStartTime;
    uint32_t triggerStartTime;
    uint32_t sensor1TriggerTime;
    uint32_t sensor2TriggerTime;
    uint32_t sensor1ReleaseTime;
    uint32_t sensor2ReleaseTime;
    uint32_t pumpStartTime;
    uint32_t lastPumpTime;
    
    // Sensor states
    bool lastSensor1State;
    bool lastSensor2State;
    bool waitingForSecondSensor;
    uint8_t pumpAttempts;
    
    // Daily volume tracking
    std::vector<PumpCycle> todayCycles;
    uint32_t dayStartTime;
    uint16_t dailyVolumeML;
    
    // Error handling
    ErrorCode lastError;
    bool errorSignalActive;
    uint32_t errorSignalStart;
    uint8_t errorPulseCount;
    bool errorPulseState;
    
    // Private methods
    void resetCycle();
    void calculateTimeGap1();
    void calculateTimeGap2();
    void calculateWaterTrigger();
    void logCycleComplete();
    uint16_t calculateDailyVolume();
    void startErrorSignal(ErrorCode error);
    void updateErrorSignal();
    
public:
    WaterAlgorithm();
    
    // Main algorithm update - call this from loop()
    void update();
    
    // Sensor inputs
    void onSensorStateChange(uint8_t sensorNum, bool triggered);
    
    // Manual pump interface
    bool requestManualPump(uint16_t duration_ms);
    void onManualPumpComplete();
    
    // Status and data access
    AlgorithmState getState() const { return currentState; }
    const char* getStateString() const;
    bool isInCycle() const;
    uint16_t getDailyVolume() const { return dailyVolumeML; }
    ErrorCode getLastError() const { return lastError; }
    
    // Aggregate data for XX-YY-ZZ-VVVV format
    void getAggregateData(uint8_t& xx, uint8_t& yy, uint8_t& zz, uint16_t& vvvv);
    
    // Get recent cycles for debugging
    std::vector<PumpCycle> getRecentCycles(size_t count = 10);
    
    // Reset after error
    void resetFromError();
};

extern WaterAlgorithm waterAlgorithm;

#endif