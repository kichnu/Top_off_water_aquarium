#include "system.h"
#include "logging.h"
#include <esp_system.h>
#include <esp_heap_caps.h>

static unsigned long start_time = 0;
static size_t min_free_heap = SIZE_MAX;

void initializeSystem() {
    start_time = millis();
    min_free_heap = getFreeHeap();
    
    LOG_INFO("System initialized - Free heap: %d bytes", getFreeHeap());
}

void updateSystem() {
    size_t current_heap = getFreeHeap();
    if (current_heap < min_free_heap) {
        min_free_heap = current_heap;
    }
    
    // Log memory status every 5 minutes
    static unsigned long last_memory_log = 0;
    if (millis() - last_memory_log > 300000) {
        LOG_INFO("Memory status - Free: %d, Min: %d bytes", current_heap, min_free_heap);
        last_memory_log = millis();
    }
}

size_t getFreeHeap() {
    return esp_get_free_heap_size();
}

size_t getMinFreeHeap() {
    return min_free_heap;
}

float getCPUTemperature() {
    // ESP32-C6 temperature sensor (if available)
    return 0.0; // Placeholder - implementation depends on specific sensor availability
}

unsigned long getUptime() {
    return millis() - start_time;
}

void resetSystem() {
    LOG_INFO("System reset requested");
    ESP.restart();
}