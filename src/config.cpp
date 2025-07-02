#include "config.h"

// ================= KONFIGURACJA SIECI =================
const char* WIFI_SSID = "KiG";        // ZMIEŃ NA SWOJĄ SIEĆ
const char* WIFI_PASSWORD = "3YqvXx5s3Z";    // ZMIEŃ NA SWOJE HASŁO WIFI

// ================= KONFIGURACJA BEZPIECZEŃSTWA =================
const char* ADMIN_PASSWORD = "admin";  // ZMIEŃ NA SWOJE HASŁO

const IPAddress ALLOWED_IPS[] = {
    IPAddress(192, 168, 0, 124),  // Przykład: komputer
    IPAddress(192, 168, 1, 101),  // Przykład: telefon  
    IPAddress(192, 168, 1, 102)   // Dodaj więcej IP tutaj
};
const int ALLOWED_IPS_COUNT = sizeof(ALLOWED_IPS) / sizeof(ALLOWED_IPS[0]);

// ================= KONFIGURACJA UART KOMUNIKACJI =================
const int UART_TX_PIN = 16;        // Pin TX do komunikacji z IoT
const int UART_RX_PIN = 17;        // Pin RX do komunikacji z IoT
const int UART_BAUD_RATE = 1200;   // Prędkość transmisji

// ================= TIMEOUTS I LIMITY =================
const unsigned long SESSION_TIMEOUT = 300000;     // 5 minut w ms
const unsigned long RATE_LIMIT_WINDOW = 1000;     // 1 sekunda
const int MAX_REQUESTS_PER_WINDOW = 5;            // 5 żądań na sekundę
const int MAX_FAILED_ATTEMPTS = 20;               // 20 nieudanych prób
const unsigned long BLOCK_DURATION = 60000;       // 1 minuta blokady

// ================= SERWERY NTP =================
const char* NTP_SERVERS[] = {
    "pool.ntp.org",
    "time.nist.gov", 
    "time.google.com"
};

// ================= STREFA CZASOWA POLSKA =================
const char* TIMEZONE_POLAND = "CET-1CEST,M3.5.0,M10.5.0/3";