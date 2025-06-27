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

// ================= KONFIGURACJA SPRZĘTOWA =================
extern const int LED_PIN;

// *** ROZSZERZENIE: Dodaj tutaj więcej pinów dla różnych urządzeń ***
// ============= PRZEKAŹNIKI =============
// extern const int RELAY_1_PIN;       // Przekaźnik 1 (np. oświetlenie)
// extern const int RELAY_2_PIN;       // Przekaźnik 2 (np. wentylator)
// extern const int RELAY_3_PIN;       // Przekaźnik 3 (np. pompa)

// ============= SERWOMECHANIZMY =============
// extern const int SERVO_1_PIN;       // Servo 1 (np. żaluzje)
// extern const int SERVO_2_PIN;       // Servo 2 (np. zawór)

// ============= SENSORY ANALOGOWE =============
// extern const int TEMP_SENSOR_PIN;   // Sensor temperatury
// extern const int LIGHT_SENSOR_PIN;  // Sensor światła
// extern const int MOISTURE_SENSOR_PIN; // Sensor wilgotności

// ============= SENSORY CYFROWE =============
// extern const int PIR_SENSOR_PIN;    // Czujnik ruchu PIR
// extern const int DOOR_SENSOR_PIN;   // Czujnik drzwi (magnetic)
// extern const int BUTTON_PIN;        // Przycisk fizyczny

// ============= PWM/DIMMER =============
// extern const int LED_STRIP_PIN;     // Taśma LED (PWM)
// extern const int FAN_SPEED_PIN;     // Sterowanie prędkością wentylatora

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
#define ENABLE_IP_WHITELIST 0  // TYMCZASOWO WYŁĄCZONE
#define ENABLE_AUTOMATION   1
#define ENABLE_RELAY        1
#define ENABLE_SERVO        1

// ================= DEFINICJE IMPLEMENTACJI =================
// Te wartości są zdefiniowane w config.cpp
extern const char* NTP_SERVERS[];
extern const char* TIMEZONE_POLAND;

#endif