#include "wifi.h"
#include "../core/logging.h"
#include "ntp.h"

// ================= ZMIENNE GLOBALNE =================
WiFiStatus wifiStatus = {false, IPAddress(0,0,0,0), IPAddress(0,0,0,0), IPAddress(0,0,0,0), 0, 0};
unsigned long lastWiFiReconnect = 0;
int wifiReconnectAttempts = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja WiFi
 */
void initializeWiFi() {
    Serial.println("\n[WIFI] Rozpoczynanie połączenia WiFi...");
    Serial.printf("[WIFI] SSID: %s\n", WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    connectWiFi();
}

/**
 * Nawiązanie połączenia WiFi
 */
bool connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(1000);
        Serial.print(".");
        attempts++;
        
        if (attempts % 10 == 0) {
            Serial.printf("\n[WIFI] Próba %d/30...\n", attempts);
        }
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WIFI] ✅ Połączono z WiFi!");
        Serial.printf("[WIFI] IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WIFI] Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("[WIFI] DNS: %s\n", WiFi.dnsIP().toString().c_str());
        Serial.printf("[WIFI] RSSI: %d dBm\n", WiFi.RSSI());
        
        // Aktualizuj status
        wifiStatus.connected = true;
        wifiStatus.localIP = WiFi.localIP();
        wifiStatus.gateway = WiFi.gatewayIP();
        wifiStatus.dns = WiFi.dnsIP();
        wifiStatus.rssi = WiFi.RSSI();
        wifiStatus.lastCheck = millis();
        
        // Uruchom synchronizację NTP
        initializeNTP();
        
        return true;
    } else {
        Serial.println("\n[WIFI] ❌ Nie udało się połączyć z WiFi!");
        Serial.println("[WIFI] Sprawdź SSID i hasło, restartowanie za 10 sekund...");
        wifiStatus.connected = false;
        delay(10000);
        ESP.restart();
        return false;
    }
}

/**
 * Sprawdź połączenie WiFi
 */
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WIFI] ❌ Utracono połączenie WiFi, próba ponownego połączenia...");
        wifiStatus.connected = false;
        reconnectWiFi();
    } else {
        // Aktualizuj status
        wifiStatus.rssi = WiFi.RSSI();
        wifiStatus.lastCheck = millis();
    }
}

/**
 * Ponowne połączenie WiFi
 */
void reconnectWiFi() {
    unsigned long now = millis();
    
    // Nie próbuj zbyt często
    if (now - lastWiFiReconnect < 30000) {
        return;
    }
    
    lastWiFiReconnect = now;
    wifiReconnectAttempts++;
    
    Serial.printf("[WIFI] Próba ponownego połączenia #%d\n", wifiReconnectAttempts);
    
    // Po 5 próbach zrestartuj ESP
    if (wifiReconnectAttempts >= 5) {
        Serial.println("[WIFI] Zbyt wiele nieudanych prób, restart...");
        ESP.restart();
    }
    
    connectWiFi();
}

/**
 * Pobierz status WiFi
 */
WiFiStatus getWiFiStatus() {
    wifiStatus.connected = (WiFi.status() == WL_CONNECTED);
    if (wifiStatus.connected) {
        wifiStatus.rssi = WiFi.RSSI();
        wifiStatus.lastCheck = millis();
    }
    return wifiStatus;
}

/**
 * Sprawdź czy WiFi jest połączone
 */
bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}