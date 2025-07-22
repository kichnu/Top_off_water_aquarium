#ifndef HARDWARE_PINS_H
#define HARDWARE_PINS_H

// Seeed Xiao ESP32-C3 Pin Mapping
#define PUMP_RELAY_PIN      2            // Pump relay control (HIGH = pump ON)
#define WATER_SENSOR1_PIN   3            // Float sensor 1 (pull-up, active LOW)
#define WATER_SENSOR2_PIN   4            // Float sensor 2 (pull-up, active LOW)
#define RTC_SDA_PIN         6            // DS3231M I2C SDA
#define RTC_SCL_PIN         7            // DS3231M I2C SCL
#define STATUS_LED_PIN      10           // Built-in LED for status

// Timing constants
#define DEBOUNCE_DELAY_MS   2000         // 2 seconds debouncing
#define PUMP_DEFAULT_TIME   15           // 15 seconds default pump time
#define STATUS_UPDATE_MS    2000         // 2 seconds status refresh

#endif