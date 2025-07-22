#include <Arduino.h>
#include <WiFi.h>
#include "config/hardware_pins.h"
#include "config/settings.h"
#include "hardware/water_sensor.h"
#include "hardware/pump_controller.h"
#include "hardware/rtc_ds3231.h"
#include "network/wifi.h"
#include "network/ntp.h"
#include "network/vps_logger.h"
#include "security/auth.h"
#include "security/sessions.h"
#include "security/ratelimit.h"
#include "web/server.h"
#include "core/logging.h"
#include "core/system.h"

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(2, OUTPUT);
    
  
}

void loop() {
  
   
    digitalWrite(2, HIGH);
    
    
    delay(100);
    
    digitalWrite(2, LOW);
    delay(100);
}