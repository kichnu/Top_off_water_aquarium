#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ================= HARDWARE PINS - RASPBERRY PI PICO2 =================
#define WATER_SENSOR_PIN    15  // GP15 - Kontakton (INPUT_PULLUP) - LOW = niski poziom wody
#define PUMP_RELAY_PIN      14  // GP14 - Przekaźnik sterowania pompy (OUTPUT)

// SD Card SPI pins (SPI0)
#define SD_CS_PIN           17  // GP17 - SD Card SPI CS
#define SD_MOSI_PIN         19  // GP19 - SPI0 MOSI (DI)
#define SD_MISO_PIN         16  // GP16 - SPI0 MISO (DO)
#define SD_CLK_PIN          18  // GP18 - SPI0 CLK

// RTC DS3231M I2C pins (I2C0)
#define RTC_SDA_PIN         4   // GP4 - I2C0 SDA
#define RTC_SCL_PIN         5   // GP5 - I2C0 SCL

// ================= UART COMMUNICATION =================
#define UART_BAUD_RATE      1200   // Zgodne z WebServerem
#define UART_TX_PIN         8      // GP8 - UART1 TX do WebServera
#define UART_RX_PIN         9      // GP9 - UART1 RX z WebServera

// ================= TIMING CONSTANTS =================
#define DEBOUNCE_DELAY_MS   1000   // 1 sekunda debouncing dla kontaktonu
#define HEARTBEAT_INTERVAL  60000  // 1 minuta heartbeat
#define MAIN_LOOP_DELAY     100    // 100ms delay w głównej pętli

// ================= SD CARD SETTINGS =================
#define LOG_FILE_PATH       "/water_addition_events.json"
#define MAX_LOG_SIZE        1000   // Maksymalnie 1000 zdarzeń w pamięci przed zapisem

// ================= EEPROM ADDRESSES =================
#define EEPROM_SIZE         1024   // Większy rozmiar dla Pico2
#define EVENT_ID_ADDR       0      // Adres countera ID zdarzeń (4 bajty)
#define PUMP_CONFIG_ADDR    10     // Adres konfiguracji pompy (8 bajtów)

// ================= DEFAULT PUMP SETTINGS =================
#define DEFAULT_VOLUME_ML   50     // Domyślna objętość [ml]
#define DEFAULT_PUMP_TIME_S 5      // Domyślny czas pracy pompy [s]

// ================= SYSTEM SETTINGS =================
#define SERIAL_DEBUG_SPEED  115200 // Prędkość Serial USB dla debugowania
#define SYSTEM_NAME         "Pico2-WaterSystem"
#define VERSION             "1.0-Pico2"

// ================= FEATURE FLAGS =================
#define ENABLE_DEBUG_LOGS   1      // Włącz szczegółowe logi debug
#define ENABLE_HEARTBEAT    1      // Włącz heartbeat do WebServera
#define ENABLE_WATCHDOG     0      // Wyłącz watchdog (na razie)

#endif