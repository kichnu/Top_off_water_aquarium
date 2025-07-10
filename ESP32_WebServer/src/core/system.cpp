#include "system.h"
#include "logging.h"
#include "../network/wifi.h"
#include "../network/ntp.h"

// ================= ZMIENNE GLOBALNE =================
unsigned long systemStartTime = 0;
bool systemInitialized = false;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja systemu
 */
void initializeSystem() {
    systemStartTime = millis();
    
    // Inicjalizuj generator liczb losowych dla tokenów sesji
    randomSeed(analogRead(0));
    
    Serial.println("\n" + String("=").substring(0, 50));
    Serial.println("🚀 ESP32 HTTP Server - Secure LAN Version");
    Serial.println("📅 Wersja: 2.1 - " + String(__DATE__) + " " + String(__TIME__));
    Serial.println(String("=").substring(0, 50));
    
    systemInitialized = true;
}

/**
 * Konserwacja systemu - wywoływana w głównej pętli
 */
void systemMaintenance() {
    // Monitoring systemu co 30 sekund
    static unsigned long lastSystemCheck = 0;
    if (millis() - lastSystemCheck > 30000) {
        lastSystemCheck = millis();
        checkMemoryUsage();
    }
}

/**
 * Pobierz informacje o systemie
 */
SystemInfo getSystemInfo() {
    SystemInfo info;
    info.uptime = millis();
    info.freeHeap = ESP.getFreeHeap();
    info.wifiRSSI = WiFi.RSSI();
    info.ntpSynced = isNTPSynced();
    
    if (info.ntpSynced) {
        time_t now = time(nullptr);
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            char timeString[64];
            strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
            info.currentTime = String(timeString);
            info.timezone = String(timeinfo.tm_isdst ? "CEST (letni)" : "CET (zimowy)");
        } else {
            info.currentTime = "Błąd odczytu";
            info.timezone = "Nieznana";
        }
    } else {
        info.currentTime = "Nie zsynchronizowany";
        info.timezone = "Brak";
    }
    
    return info;
}

/**
 * Restart systemu
 */
void restartSystem() {
    Serial.println("[SYSTEM] Restart systemu...");
    delay(1000);
    ESP.restart();
}

/**
 * Sprawdź użycie pamięci i wypisz statystyki
 */
void checkMemoryUsage() {
    Serial.printf("[SYSTEM] Uptime: %lu min, Pamięć: %d bytes\n", 
                 millis() / 60000, ESP.getFreeHeap());
    
    // Ostrzeżenie o niskiej pamięci
    if (ESP.getFreeHeap() < 10000) {
        Serial.println("[SYSTEM] ⚠️ Niska ilość wolnej pamięci!");
    }
}