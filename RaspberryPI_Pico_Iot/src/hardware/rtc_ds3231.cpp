#include "rtc_ds3231.h"
#include "../config.h"
#include <Wire.h>
#include "DFRobot_DS3231M.h"

// ================= ZMIENNE GLOBALNE =================
DFRobot_DS3231M rtc;
bool rtcInitialized = false;
unsigned long lastRTCRead = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja RTC DS3231M (DFRobot)
 */
void initializeRTC() {
    Serial.println("[RTC-INIT] Inicjalizacja RTC DS3231M (DFRobot)");
    
    // Konfiguruj I2C piny dla Pico2
    Wire.setSDA(RTC_SDA_PIN);
    Wire.setSCL(RTC_SCL_PIN);
    Wire.begin();
    
    Serial.printf("[RTC-CONFIG] I2C SDA: GP%d, SCL: GP%d\n", RTC_SDA_PIN, RTC_SCL_PIN);
    
    // Inicjalizuj RTC - czekaj aż się zainicjalizuje
    int initAttempts = 0;
    while(rtc.begin() != true && initAttempts < 10) {
        Serial.println("[RTC-INIT] Próba inicjalizacji RTC...");
        delay(1000);
        initAttempts++;
    }
    
    if (initAttempts >= 10) {
        Serial.println("[RTC-ERROR] Nie można zainicjalizować RTC DS3231M!");
        Serial.println("[RTC-ERROR] Sprawdź połączenia I2C i zasilanie RTC");
        rtcInitialized = false;
        return;
    }
    
    rtcInitialized = true;
    lastRTCRead = millis();
    
    // Sprawdź czy RTC stracił zasilanie
    if (rtc.lostPower()) {
        Serial.println("[RTC-WARNING] RTC stracił zasilanie - czas może być nieprawidłowy");
        Serial.println("[RTC-WARNING] Ustaw czas w RTC przed użyciem systemu");
    }
    
    // Wyświetl aktualny czas
    rtc.getNowTime();
    Serial.println("[RTC-SUCCESS] RTC DS3231M zainicjalizowany pomyślnie");
    Serial.printf("[RTC-TIME] Aktualny czas: %04d-%02d-%02d %02d:%02d:%02d\n",
                  rtc.year(), rtc.month(), rtc.day(),
                  rtc.hour(), rtc.minute(), rtc.second());
    Serial.println();
}

/**
 * Pobierz aktualny czas jako string
 */
String getCurrentTimeString() {
    if (!rtcInitialized) {
        return "RTC-ERROR";
    }
    
    rtc.getNowTime();
    lastRTCRead = millis();
    
    char timeBuffer[20];
    snprintf(timeBuffer, sizeof(timeBuffer), "%04d-%02d-%02d %02d:%02d:%02d",
             rtc.year(), rtc.month(), rtc.day(),
             rtc.hour(), rtc.minute(), rtc.second());
    
    return String(timeBuffer);
}

/**
 * Pobierz aktualny czas jako strukturę TimeInfo
 */
TimeInfo getCurrentTime() {
    TimeInfo timeInfo;
    
    if (!rtcInitialized) {
        timeInfo.valid = false;
        timeInfo.year = 0;
        timeInfo.month = 0;
        timeInfo.day = 0;
        timeInfo.hour = 0;
        timeInfo.minute = 0;
        timeInfo.second = 0;
        timeInfo.temperature = 0.0;
        return timeInfo;
    }
    
    rtc.getNowTime();
    lastRTCRead = millis();
    
    timeInfo.valid = true;
    timeInfo.year = rtc.year();
    timeInfo.month = rtc.month();
    timeInfo.day = rtc.day();
    timeInfo.hour = rtc.hour();
    timeInfo.minute = rtc.minute();
    timeInfo.second = rtc.second();
    timeInfo.temperature = rtc.getTemperatureC();
    
    return timeInfo;
}

/**
 * Pobierz Unix timestamp (przybliżony)
 */
unsigned long getUnixTimestamp() {
    if (!rtcInitialized) {
        return 0;
    }
    
    TimeInfo timeInfo = getCurrentTime();
    if (!timeInfo.valid) {
        return 0;
    }
    
    // Prosta konwersja na Unix timestamp (przybliżona)
    // Zakładamy rok > 2000, więc odejmujemy 2000 i dodajemy 30 lat (1970-2000)
    int years = timeInfo.year + 30;  // Rok względem 1970
    unsigned long timestamp = 0;
    
    // Przybliżona kalkulacja (365.25 dni * 24h * 3600s * lata)
    timestamp += (unsigned long)years * 31557600L;  // sekundy w roku
    timestamp += (unsigned long)(timeInfo.month - 1) * 2629800L;  // sekundy w miesiącu
    timestamp += (unsigned long)(timeInfo.day - 1) * 86400L;
    timestamp += (unsigned long)timeInfo.hour * 3600L;
    timestamp += (unsigned long)timeInfo.minute * 60L;
    timestamp += (unsigned long)timeInfo.second;
    
    return timestamp;
}

/**
 * Sprawdź czy RTC jest zainicjalizowany i działa
 */
bool isRTCWorking() {
    if (!rtcInitialized) {
        return false;
    }
    
    // Sprawdź czy RTC odpowiada - porównaj dwa odczyty
    rtc.getNowTime();
    int second1 = rtc.second();
    delay(1100);  // Poczekaj ponad sekundę
    rtc.getNowTime();
    int second2 = rtc.second();
    
    // Jeśli sekundy się zmieniły, RTC prawdopodobnie działa
    return (second2 != second1);
}

/**
 * Pobierz temperaturę z RTC (DS3231M ma wbudowany czujnik)
 */
float getRTCTemperature() {
    if (!rtcInitialized) {
        return -999.0;  // Błąd
    }
    
    return rtc.getTemperatureC();
}

/**
 * Formatuj TimeInfo jako string
 */
String formatTimeInfo(TimeInfo timeInfo) {
    if (!timeInfo.valid) {
        return "INVALID-TIME";
    }
    
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             timeInfo.year, timeInfo.month, timeInfo.day,
             timeInfo.hour, timeInfo.minute, timeInfo.second);
    return String(buffer);
}

/**
 * Formatuj TimeInfo jako string dla nazwy pliku (bez spacji i dwukropków)
 */
String formatDateTimeForFilename(TimeInfo timeInfo) {
    if (!timeInfo.valid) {
        return "INVALID";
    }
    
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d%02d%02d_%02d%02d%02d",
             timeInfo.year, timeInfo.month, timeInfo.day,
             timeInfo.hour, timeInfo.minute, timeInfo.second);
    return String(buffer);
}

/**
 * Pobierz tylko datę jako string
 */
String getCurrentDateString() {
    if (!rtcInitialized) {
        return "RTC-ERROR";
    }
    
    rtc.getNowTime();
    char dateBuffer[12];
    snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d",
             rtc.year(), rtc.month(), rtc.day());
    
    return String(dateBuffer);
}

/**
 * Pobierz tylko czas jako string
 */
String getCurrentTimeOnlyString() {
    if (!rtcInitialized) {
        return "RTC-ERROR";
    }
    
    rtc.getNowTime();
    char timeBuffer[10];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d",
             rtc.hour(), rtc.minute(), rtc.second());
    
    return String(timeBuffer);
}

/**
 * Debug - wyświetl pełny status RTC
 */
void debugRTC() {
    Serial.println("[RTC-DEBUG] === STATUS RTC DS3231M (DFRobot) ===");
    Serial.printf("[RTC-DEBUG] Zainicjalizowany: %s\n", rtcInitialized ? "TAK" : "NIE");
    
    if (rtcInitialized) {
        rtc.getNowTime();
        float temp = getRTCTemperature();
        unsigned long timestamp = getUnixTimestamp();
        
        Serial.printf("[RTC-DEBUG] Aktualny czas: %04d-%02d-%02d %02d:%02d:%02d\n",
                      rtc.year(), rtc.month(), rtc.day(),
                      rtc.hour(), rtc.minute(), rtc.second());
        Serial.printf("[RTC-DEBUG] Unix timestamp: %lu\n", timestamp);
        Serial.printf("[RTC-DEBUG] Temperatura RTC: %.2f°C\n", temp);
        Serial.printf("[RTC-DEBUG] Ostatni odczyt: %lu ms temu\n", millis() - lastRTCRead);
        Serial.printf("[RTC-DEBUG] RTC działa: %s\n", isRTCWorking() ? "TAK" : "NIE");
        
        if (rtc.lostPower()) {
            Serial.println("[RTC-DEBUG] WARNING: RTC stracił zasilanie!");
        }
    }
    
    Serial.println("[RTC-DEBUG] ===================================");
    Serial.println();
}