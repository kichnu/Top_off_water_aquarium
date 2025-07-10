/**
 * ESP32 Smart Home WebServer - Refactored Main File
 * 
 * Simplified system for LED control via UART communication with IoT ESP32
 * Features: LED control, authentication, sessions, WiFi, NTP, UART
 * 
 * Version: 2.1 Refactored - Minimal Clean Code
 */

// ================= IMPORTY MODUŁÓW =================
#include "config.h"
#include "core/system.h"
#include "core/logging.h"
#include "network/wifi.h"
#include "network/ntp.h"
#include "security/auth.h"
#include "security/sessions.h"
#include "security/ratelimit.h"
#include "web/server.h"
#include "communication/uart.h"  // UART communication + LED control

// ================= SETUP =================
void setup() {
    // Initialize core systems
    initializeLogging();
    initializeSystem();
    
    Serial.println("[WEBSERVER-START] Uruchamianie ESP32 WebServer + IoT UART");
    Serial.println();
    
    // Initialize UART communication (includes LED control)
    initializeUART();
    
    // Initialize network
    initializeWiFi();
    
    // Initialize security
    initializeSessions();
    initializeRateLimit();
    
    // Initialize web services
    initializeWebServer();
    startWebServer();
    
    // Request initial state sync from IoT
    delay(2000); // Wait for IoT to start
    requestStateSync();
    
    Serial.println("[WEBSERVER-READY] System gotowy do pracy");
    Serial.printf("[WEBSERVER-CONFIG] WiFi: %s, UART: TX=%d RX=%d Baud=%d\n", WIFI_SSID, UART_TX_PIN, UART_RX_PIN, UART_BAUD_RATE);
    Serial.println();
}

// ================= LOOP =================
void loop() {
    // Check network connection
    checkWiFiConnection();
    
    // Process UART communication with IoT
    processUARTData();
    
    // System maintenance
    systemMaintenance();
    
    // Cleanup old sessions and clients every 5 minutes
    static unsigned long lastCleanup = 0;
    if (millis() - lastCleanup > 300000) {
        lastCleanup = millis();
        
        cleanupExpiredSessions();
        cleanupOldClients();
        
        // Show status
        int activeSessions = getActiveSessionCount();
        int pendingCommands = getPendingCommandsCount();
        bool iotConnected = isIoTConnected();
        
        if (activeSessions > 0 || pendingCommands > 0 || !iotConnected) {
            Serial.println("[WEBSERVER-STATUS] Sesje: " + String(activeSessions) + 
                          ", UART Commands: " + String(pendingCommands) + 
                          ", IoT: " + String(iotConnected ? "Connected" : "Disconnected"));
            Serial.println();
        }
    }
    
    // Monitor IoT connection status
    static unsigned long lastIoTCheck = 0;
    if (millis() - lastIoTCheck > 60000) { // Every minute
        lastIoTCheck = millis();
        
        if (!isIoTConnected()) {
            Serial.println("[WEBSERVER-IOT-WARNING] IoT rozłączony - używam cache");
            Serial.println();
        }
    }
    
    delay(100);
}