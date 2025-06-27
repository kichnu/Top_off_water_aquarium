#ifndef NETWORK_WIFI_H
#define NETWORK_WIFI_H

#include <WiFi.h>
#include "../config.h"

// ================= STRUKTURY =================
struct WiFiStatus {
    bool connected;
    IPAddress localIP;
    IPAddress gateway;
    IPAddress dns;
    int rssi;
    unsigned long lastCheck;
};

// ================= FUNKCJE WiFi =================
void initializeWiFi();
bool connectWiFi();
void checkWiFiConnection();
void reconnectWiFi();
WiFiStatus getWiFiStatus();
bool isWiFiConnected();

// ================= ZMIENNE GLOBALNE =================
extern WiFiStatus wifiStatus;
extern unsigned long lastWiFiReconnect;
extern int wifiReconnectAttempts;

#endif