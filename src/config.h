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

// ================= KONFIGURACJA SPRZĘTOWA =================
// UWAGA: WebServer nie steruje bezpośrednio sprzętem - tylko wysyła komendy przez UART
extern const int LED_PIN;  // Pin LED (tylko dla referencji - sterowanie przez IoT)

// *** ROZSZERZENIE: Dodaj tutaj więcej pinów dla różnych urządzeń (referencyjnie) ***
// ============= PRZEKAŹNIKI =============
// extern const int RELAY_1_PIN;       // Przekaźnik 1 (np. oświetlenie) - sterowany przez IoT
// extern const int RELAY_2_PIN;       // Przekaźnik 2 (np. wentylator) - sterowany przez IoT
// extern const int RELAY_3_PIN;       // Przekaźnik 3 (np. pompa) - sterowany przez IoT

// ============= SERWOMECHANIZMY =============
// extern const int SERVO_1_PIN;       // Servo 1 (np. żaluzje) - sterowany przez IoT
// extern const int SERVO_2_PIN;       // Servo 2 (np. zawór) - sterowany przez IoT

// ============= SENSORY ANALOGOWE =============
// extern const int TEMP_SENSOR_PIN;   // Sensor temperatury - odczytywany przez IoT
// extern const int LIGHT_SENSOR_PIN;  // Sensor światła - odczytywany przez IoT
// extern const int MOISTURE_SENSOR_PIN; // Sensor wilgotności - odczytywany przez IoT

// ============= SENSORY CYFROWE =============
// extern const int PIR_SENSOR_PIN;    // Czujnik ruchu PIR - odczytywany przez IoT
// extern const int DOOR_SENSOR_PIN;   // Czujnik drzwi (magnetic) - odczytywany przez IoT
// extern const int BUTTON_PIN;        // Przycisk fizyczny - odczytywany przez IoT

// ============= PWM/DIMMER =============
// extern const int LED_STRIP_PIN;     // Taśma LED (PWM) - sterowana przez IoT
// extern const int FAN_SPEED_PIN;     // Sterowanie prędkością wentylatora - przez IoT

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
#define ENABLE_UART_COMM    1  // NOWE: Komunikacja UART

// ================= DEFINICJE IMPLEMENTACJI =================
// Te wartości są zdefiniowane w config.cpp
extern const char* NTP_SERVERS[];
extern const char* TIMEZONE_POLAND;

#endif