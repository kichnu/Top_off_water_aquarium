#ifndef CONFIG_H
#define CONFIG_H

#include <IPAddress.h>

// ================= KONFIGURACJA SIECI =================
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// ================= KONFIGURACJA BEZPIECZEŃSTWA =================
extern const char* ADMIN_PASSWORD;
extern const IPAddress ALLOWED_IPS[];
extern const int ALLOWED_IPS_COUNT;

// ================= KONFIGURACJA UART KOMUNIKACJI =================
extern const int UART_TX_PIN;         // Pin TX dla UART komunikacji z IoT
extern const int UART_RX_PIN;         // Pin RX dla UART komunikacji z IoT  
extern const int UART_BAUD_RATE;      // Prędkość UART

// ================= TIMEOUTS I LIMITY =================
extern const unsigned long SESSION_TIMEOUT;
extern const unsigned long RATE_LIMIT_WINDOW;
extern const int MAX_REQUESTS_PER_WINDOW;
extern const int MAX_FAILED_ATTEMPTS;
extern const unsigned long BLOCK_DURATION;

constexpr int MAX_SESSIONS = 10;
constexpr int MAX_CLIENTS = 50;

// ================= FEATURE FLAGS =================
#define ENABLE_NTP          1
#define ENABLE_RATE_LIMIT   1
#define ENABLE_IP_WHITELIST 0  // WYŁĄCZONE
#define ENABLE_UART_COMM    1  // Komunikacja UART

// ================= DEFINICJE IMPLEMENTACJI =================
extern const char* NTP_SERVERS[];
extern const char* TIMEZONE_POLAND;

#endif