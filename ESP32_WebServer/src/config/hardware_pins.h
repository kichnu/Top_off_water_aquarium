#ifndef HARDWARE_PINS_H
#define HARDWARE_PINS_H

#include <Arduino.h>

// ESP32-C6 Pin Mapping
#define PUMP_RELAY_PIN      GPIO_NUM_0   // Pump relay control (HIGH = pump ON)
#define WATER_SENSOR1_PIN   GPIO_NUM_1   // Float sensor 1 (GND contact = active LOW)
#define WATER_SENSOR2_PIN   GPIO_NUM_12  // Float sensor 2 (GND contact = active LOW)
#define RTC_SDA_PIN         22           // DS3231M I2C SDA
#define RTC_SCL_PIN         23           // DS3231M I2C SCL

// Timing constants
#define DEBOUNCE_DELAY_MS   2000         // 2 seconds debouncing
#define PUMP_DEFAULT_TIME   15           // 15 seconds default pump time
#define PUMP_EXTENDED_TIME  60           // 60 seconds for manual long cycle

// System constants
#define MAX_DAILY_VOLUME_ML 5000         // 5L daily limit
#define MIN_PUMP_INTERVAL   300          // 5 minutes between auto pumps

#endif