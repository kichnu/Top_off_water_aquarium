#include "ntp.h"
#include "../core/logging.h"

// ================= ZMIENNE GLOBALNE =================
bool ntpSynced = false;
unsigned long lastNTPSync = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja NTP
 */
void initializeNTP() {
    syncNTPTime();
}

/**
 * Synchronizacja czasu z serwerami NTP - czas dla Polski z automatycznym czasem letnim/zimowym
 */
bool syncNTPTime() {
    Serial.println("[NTP] Synchronizacja czasu...");
    
    // Konfiguracja dla Polski:
    // CET-1CEST,M3.5.0,M10.5.0/3
    // CET-1: Central European Time UTC+1 (czas zimowy)
    // CEST: Central European Summer Time UTC+2 (czas letni)
    // M3.5.0: ostatnia niedziela marca o 2:00
    // M10.5.0/3: ostatnia niedziela października o 3:00
    configTime(0, 0, NTP_SERVERS[0], NTP_SERVERS[1], NTP_SERVERS[2]);
    setenv("TZ", TIMEZONE_POLAND, 1);
    tzset();
    
    // Czekaj na synchronizację (max 15 sekund)
    int ntpAttempts = 0;
    time_t now = time(nullptr);
    while (now < 1609459200 && ntpAttempts < 15) { // 1609459200 = 1 Jan 2021 (sprawdzenie czy czas jest sensowny)
        delay(1000);
        now = time(nullptr);
        ntpAttempts++;
        Serial.print(".");
    }
    
    if (now >= 1609459200) {
        Serial.println("\n[NTP] ✅ Czas zsynchronizowany!");
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            Serial.printf("[NTP] Aktualny czas: %02d-%02d-%04d %02d:%02d:%02d\n",
                         timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
                         timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            Serial.printf("[NTP] Strefa czasowa: %s\n", timeinfo.tm_isdst ? "CEST (letni)" : "CET (zimowy)");
        }
        ntpSynced = true;
        lastNTPSync = millis();
        return true;
    } else {
        Serial.println("\n[NTP] ⚠️ Nie udało się zsynchronizować czasu");
        Serial.println("[NTP] Używam czasu systemowego (millis), cookie może mieć problemy");
        ntpSynced = false;
        return false;
    }
}

/**
 * Sprawdź czy czas NTP jest zsynchronizowany
 */
bool isNTPSynced() {
    time_t now = time(nullptr);
    return (now >= 1609459200); // Sprawdź czy czas jest po 1 stycznia 2021
}

/**
 * Pobierz aktualny czas
 */
TimeInfo getCurrentTime() {
    TimeInfo timeInfo;
    timeInfo.synced = isNTPSynced();
    
    if (timeInfo.synced) {
        time_t now = time(nullptr);
        
        // Czas lokalny
        struct tm localTimeInfo;
        if (getLocalTime(&localTimeInfo)) {
            char localTimeString[64];
            strftime(localTimeString, sizeof(localTimeString), "%Y-%m-%d %H:%M:%S", &localTimeInfo);
            timeInfo.localTime = String(localTimeString);
            timeInfo.timezone = String(localTimeInfo.tm_isdst ? "CEST (letni)" : "CET (zimowy)");
            timeInfo.isDST = localTimeInfo.tm_isdst;
        }
        
        // Czas UTC
        struct tm utcTimeInfo;
        gmtime_r(&now, &utcTimeInfo);
        char utcTimeString[64];
        strftime(utcTimeString, sizeof(utcTimeString), "%Y-%m-%d %H:%M:%S", &utcTimeInfo);
        timeInfo.utcTime = String(utcTimeString) + " UTC";
    } else {
        timeInfo.localTime = "Nie zsynchronizowany";
        timeInfo.utcTime = "Nie zsynchronizowany";
        timeInfo.timezone = "Brak";
        timeInfo.isDST = false;
    }
    
    return timeInfo;
}

/**
 * Pobierz sformatowany czas
 */
String getFormattedTime(bool useUTC) {
    if (!isNTPSynced()) {
        return "Nie zsynchronizowany";
    }
    
    time_t now = time(nullptr);
    struct tm timeInfo;
    
    if (useUTC) {
        gmtime_r(&now, &timeInfo);
    } else {
        getLocalTime(&timeInfo);
    }
    
    char timeString[64];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
    
    return String(timeString);
}

/**
 * Pobierz Unix timestamp
 */
time_t getUnixTimestamp() {
    return time(nullptr);
}