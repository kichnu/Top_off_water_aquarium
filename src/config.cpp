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

// ================= KONFIGURACJA SPRZĘTOWA =================
const int LED_PIN = 2;  // Pin LED (dla ESP32C6 użyj pin 15)

// *** ROZSZERZENIE: Odkomentuj i skonfiguruj według potrzeb ***
// ============= PRZEKAŹNIKI =============
// const int RELAY_1_PIN = 4;       // Przekaźnik 1 (np. oświetlenie)
// const int RELAY_2_PIN = 5;       // Przekaźnik 2 (np. wentylator)
// const int RELAY_3_PIN = 18;      // Przekaźnik 3 (np. pompa)

// ============= SERWOMECHANIZMY =============
// const int SERVO_1_PIN = 9;       // Servo 1 (np. żaluzje)
// const int SERVO_2_PIN = 10;      // Servo 2 (np. zawór)

// ============= SENSORY ANALOGOWE =============
// const int TEMP_SENSOR_PIN = A0;  // Sensor temperatury
// const int LIGHT_SENSOR_PIN = A1; // Sensor światła
// const int MOISTURE_SENSOR_PIN = A2; // Sensor wilgotności

// ============= SENSORY CYFROWE =============
// const int PIR_SENSOR_PIN = 12;   // Czujnik ruchu PIR
// const int DOOR_SENSOR_PIN = 13;  // Czujnik drzwi (magnetic)
// const int BUTTON_PIN = 14;       // Przycisk fizyczny

// ============= PWM/DIMMER =============
// const int LED_STRIP_PIN = 16;    // Taśma LED (PWM)
// const int FAN_SPEED_PIN = 17;    // Sterowanie prędkością wentylatora

// ================= TIMEOUTS I LIMITY =================
const unsigned long SESSION_TIMEOUT = 300000;     // 5 minut w ms (300000)
const unsigned long RATE_LIMIT_WINDOW = 1000;     // 1 sekunda
const int MAX_REQUESTS_PER_WINDOW = 5;            // 5 żądań na sekundę
const int MAX_FAILED_ATTEMPTS = 20;               // 20 nieudanych prób
const unsigned long BLOCK_DURATION = 60000;       // 1 minuta blokady
// const int MAX_SESSIONS = 10;
// const int MAX_CLIENTS = 50;

// ================= SERWERY NTP =================
const char* NTP_SERVERS[] = {
    "pool.ntp.org",
    "time.nist.gov", 
    "time.google.com"
};

// ================= STREFA CZASOWA POLSKA =================
// CET-1CEST,M3.5.0,M10.5.0/3
// CET-1: Central European Time UTC+1 (czas zimowy)
// CEST: Central European Summer Time UTC+2 (czas letni)
// M3.5.0: ostatnia niedziela marca o 2:00
// M10.5.0/3: ostatnia niedziela października o 3:00
const char* TIMEZONE_POLAND = "CET-1CEST,M3.5.0,M10.5.0/3";