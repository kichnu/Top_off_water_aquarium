#include "ntp.h"
#include "../hardware/rtc_ds3231.h"
#include "../core/logging.h"
#include <WiFi.h>
#include <time.h>

static bool ntp_synced = false;
static unsigned long last_sync_attempt = 0;
static const unsigned long SYNC_INTERVAL = 3600000; // 1 hour

void initializeNTP() {
    if (WiFi.status() != WL_CONNECTED) {
        LOG_WARNING("WiFi not connected, skipping NTP initialization");
        return;
    }
    
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    
    LOG_INFO("NTP client initialized");
    
    // Initial sync attempt
    updateNTP();
}

void updateNTP() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }
    
    unsigned long now = millis();
    if (now - last_sync_attempt < SYNC_INTERVAL) {
        return;
    }
    
    last_sync_attempt = now;
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        // Successfully got time from NTP
        ntp_synced = true;
        LOG_INFO("NTP sync successful: %04d-%02d-%02d %02d:%02d:%02d",
                 timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                 timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        
        // Note: RTC time setting removed - RTC manages its own time
        // If you want to sync RTC with NTP, re-implement setRTCTime()
    } else {
        LOG_WARNING("NTP sync failed");
        ntp_synced = false;
    }
}

bool isNTPSynced() {
    return ntp_synced;
}

String getNTPStatus() {
    if (WiFi.status() != WL_CONNECTED) {
        return "WiFi disconnected";
    }
    return ntp_synced ? "Synced" : "Not synced";
}