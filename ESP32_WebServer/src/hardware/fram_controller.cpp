#include "fram_controller.h"
#include "../core/logging.h"
#include "../config/hardware_pins.h"
#include <Wire.h>
#include <Adafruit_FRAM_I2C.h>



Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
bool framInitialized = false;

// Calculate simple checksum
uint16_t calculateChecksum(uint8_t* data, size_t len) {
    uint16_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

bool initFRAM() {
    LOG_INFO("Initializing FRAM at address 0x50...");
    
    // FRAM uses same I2C as RTC - already initialized in rtc_controller
    // Just begin FRAM communication
    if (!fram.begin(0x50)) {
        LOG_ERROR("FRAM not found at address 0x50!");
        framInitialized = false;
        return false;
    }
    
    framInitialized = true;
    LOG_INFO("FRAM initialized successfully (256Kbit = 32KB)");
    
    // Verify FRAM integrity
    if (!verifyFRAM()) {
        LOG_WARNING("FRAM not initialized or corrupted, initializing...");
        
        // Write magic number
        uint32_t magic = FRAM_MAGIC_NUMBER;
        fram.write(FRAM_ADDR_MAGIC, (uint8_t*)&magic, 4);
        
        // Write version
        uint16_t version = FRAM_DATA_VERSION;
        fram.write(FRAM_ADDR_VERSION, (uint8_t*)&version, 2);
        
        // Write default volume
        float defaultVolume = 1.0;
        fram.write(FRAM_ADDR_VOLUME_ML, (uint8_t*)&defaultVolume, 4);
        
        // Calculate and write checksum
        uint8_t buffer[4];
        fram.read(FRAM_ADDR_VOLUME_ML, buffer, 4);
        uint16_t checksum = calculateChecksum(buffer, 4);
        fram.write(FRAM_ADDR_CHECKSUM, (uint8_t*)&checksum, 2);
        
        LOG_INFO("FRAM initialized with defaults");
    }
    
    return true;
}

bool verifyFRAM() {
    if (!framInitialized) return false;
    
    // Check magic number
    uint32_t magic = 0;
    fram.read(FRAM_ADDR_MAGIC, (uint8_t*)&magic, 4);
    
    if (magic != FRAM_MAGIC_NUMBER) {
        LOG_WARNING("FRAM magic number mismatch: 0x%08X", magic);
        return false;
    }
    
    // Check version
    uint16_t version = 0;
    fram.read(FRAM_ADDR_VERSION, (uint8_t*)&version, 2);
    
    if (version != FRAM_DATA_VERSION) {
        LOG_WARNING("FRAM version mismatch: %d", version);
        return false;
    }
    
    // Verify checksum
    uint8_t buffer[4];
    fram.read(FRAM_ADDR_VOLUME_ML, buffer, 4);
    uint16_t calculatedChecksum = calculateChecksum(buffer, 4);
    
    uint16_t storedChecksum = 0;
    fram.read(FRAM_ADDR_CHECKSUM, (uint8_t*)&storedChecksum, 2);
    
    if (calculatedChecksum != storedChecksum) {
        LOG_WARNING("FRAM checksum mismatch: stored=%d, calculated=%d", 
                    storedChecksum, calculatedChecksum);
        return false;
    }
    
    LOG_INFO("FRAM verification successful");
    return true;
}

bool loadVolumeFromFRAM(float& volume) {
    if (!framInitialized) {
        LOG_ERROR("FRAM not initialized, cannot load volume");
        return false;
    }
    
    // Read volume value
    fram.read(FRAM_ADDR_VOLUME_ML, (uint8_t*)&volume, 4);
    
    // Verify checksum
    uint8_t buffer[4];
    memcpy(buffer, &volume, 4);
    uint16_t calculatedChecksum = calculateChecksum(buffer, 4);
    
    uint16_t storedChecksum = 0;
    fram.read(FRAM_ADDR_CHECKSUM, (uint8_t*)&storedChecksum, 2);
    
    if (calculatedChecksum != storedChecksum) {
        LOG_ERROR("FRAM checksum error when loading volume!");
        return false;
    }
    
    // Sanity check the value
    if (volume < 0.1 || volume > 20.0) {
        LOG_WARNING("FRAM volume out of range: %.2f, using default", volume);
        volume = 1.0;
        return false;
    }
    
    LOG_INFO("Loaded volume from FRAM: %.1f ml/s", volume);
    return true;
}

bool saveVolumeToFRAM(float volume) {
    if (!framInitialized) {
        LOG_ERROR("FRAM not initialized, cannot save volume");
        return false;
    }
    
    // Sanity check
    if (volume < 0.1 || volume > 20.0) {
        LOG_ERROR("Invalid volume value: %.2f", volume);
        return false;
    }
    
    // Write volume
    fram.write(FRAM_ADDR_VOLUME_ML, (uint8_t*)&volume, 4);
    
    // Calculate and write checksum
    uint8_t buffer[4];
    memcpy(buffer, &volume, 4);
    uint16_t checksum = calculateChecksum(buffer, 4);
    fram.write(FRAM_ADDR_CHECKSUM, (uint8_t*)&checksum, 2);
    
    // Verify write by reading back
    float readBack = 0;
    fram.read(FRAM_ADDR_VOLUME_ML, (uint8_t*)&readBack, 4);
    
    if (abs(readBack - volume) > 0.01) {
        LOG_ERROR("FRAM write verification failed! Wrote: %.2f, Read: %.2f", 
                  volume, readBack);
        return false;
    }
    
    LOG_INFO("SUCCESS: Saved volume to FRAM: %.1f ml/s", volume);
    return true;
}

void testFRAM() {
    if (!framInitialized) {
        LOG_ERROR("FRAM not initialized for testing");
        return;
    }
    
    LOG_INFO("=== FRAM Test Start ===");
    
    // Test 1: Write/Read test pattern
    uint8_t testData[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t readData[16];
    
    fram.write(0x1000, testData, 16);  // Test address at 4KB offset
    fram.read(0x1000, readData, 16);
    
    bool testPassed = true;
    for (int i = 0; i < 16; i++) {
        if (testData[i] != readData[i]) {
            LOG_ERROR("FRAM test failed at byte %d: wrote 0x%02X, read 0x%02X", 
                     i, testData[i], readData[i]);
            testPassed = false;
        }
    }
    
    if (testPassed) {
        LOG_INFO("FRAM read/write test PASSED");
    }
    
    // Test 2: Volume persistence
    float testVolume = 3.14;
    if (saveVolumeToFRAM(testVolume)) {
        float loadedVolume = 0;
        if (loadVolumeFromFRAM(loadedVolume)) {
            if (abs(loadedVolume - testVolume) < 0.01) {
                LOG_INFO("FRAM volume persistence test PASSED");
            } else {
                LOG_ERROR("FRAM volume mismatch: saved %.2f, loaded %.2f", 
                         testVolume, loadedVolume);
            }
        }
    }
    
    // Restore default volume
    saveVolumeToFRAM(1.0);
    
    LOG_INFO("=== FRAM Test Complete ===");
}