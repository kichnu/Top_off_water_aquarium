#include "wifi.h"
#include "../config/settings.h"
#include "../core/logging.h"
#include <WiFi.h>

static unsigned long last_connection_attempt = 0;
static const unsigned long RECONNECT_INTERVAL = 30000; // 30 seconds

void initializeWiFi() {
    WiFi.mode(WIFI_STA);
    
    String ssid = getWiFiSSID();
    String password = getWiFiPassword();
    
    LOG_INFO("Connecting to WiFi: %s", ssid.c_str());
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        LOG_DEBUG("WiFi connecting... attempt %d", attempts + 1);
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        LOG_INFO("WiFi connected successfully");
        LOG_INFO("IP address: %s", WiFi.localIP().toString().c_str());
        LOG_INFO("Gateway: %s", WiFi.gatewayIP().toString().c_str());
        LOG_INFO("DNS: %s", WiFi.dnsIP().toString().c_str());
    } else {
        LOG_ERROR("WiFi connection failed");
    }
}

void updateWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - last_connection_attempt > RECONNECT_INTERVAL) {
            LOG_WARNING("WiFi disconnected, attempting reconnection");
            WiFi.reconnect();
            last_connection_attempt = now;
        }
    }
}

bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String getWiFiStatus() {
    switch (WiFi.status()) {
        case WL_CONNECTED: return "Connected";
        case WL_NO_SSID_AVAIL: return "SSID not available";
        case WL_CONNECT_FAILED: return "Connection failed";
        case WL_CONNECTION_LOST: return "Connection lost";
        case WL_DISCONNECTED: return "Disconnected";
        default: return "Unknown";
    }
}