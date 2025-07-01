#include "actuators.h"
#include "../core/logging.h"
#include "../communication/uart.h"

// ================= CACHE STANÓW URZĄDZEŃ =================
// WebServer cache'uje stany urządzeń na podstawie odpowiedzi z IoT
bool ledState = false;

// *** ROZSZERZENIE: Dodaj tutaj zmienne cache dla nowych urządzeń ***
// ============= CACHE STANÓW PRZEKAŹNIKÓW =============
// bool relay1State = false;  // Cache stanu przekaźnika 1
// bool relay2State = false;  // Cache stanu przekaźnika 2
// bool relay3State = false;  // Cache stanu przekaźnika 3

// ============= CACHE POZYCJI SERWOMECHANIZMÓW =============
// int servo1Position = 90;   // Cache pozycji servo 1 (0-180°)
// int servo2Position = 90;   // Cache pozycji servo 2 (0-180°)

// ============= CACHE PWM/DIMMER =============
// int ledStripBrightness = 0;  // Cache jasności taśmy LED (0-255)
// int fanSpeed = 0;           // Cache prędkości wentylatora (0-255)

// ============= CACHE DANYCH SENSORÓW =============
// float temperature = 0.0;    // Cache temperatury
// int lightLevel = 0;         // Cache poziomu światła
// int moistureLevel = 0;      // Cache poziomu wilgotności
// bool motionDetected = false; // Cache wykrytego ruchu
// bool doorOpen = false;      // Cache stanu drzwi

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja wszystkich actuatorów
 */
void initializeActuators() {
    // UWAGA: WebServer nie inicjalizuje pinów sprzętowych!
    // Wszystkie urządzenia są sterowane przez IoT ESP32
    
    Serial.println("[ACTUATORS] WebServer - brak bezpośredniego sterowania sprzętem");
    Serial.println("[ACTUATORS] Wszystkie urządzenia sterowane przez IoT przez UART");
    
    // Ustaw domyślne wartości cache
    ledState = false;
    
    // *** ROZSZERZENIE: Ustaw domyślne wartości cache dla nowych urządzeń ***
    /*
    relay1State = false;
    relay2State = false;
    relay3State = false;
    servo1Position = 90;
    servo2Position = 90;
    ledStripBrightness = 0;
    fanSpeed = 0;
    temperature = 0.0;
    lightLevel = 0;
    moistureLevel = 0;
    motionDetected = false;
    doorOpen = false;
    */
    
    LOG_INFO_MSG("ACTUATORS", "Actuators zainicjalizowane (UART mode)");
}

/**
 * Sterowanie LED przez UART
 */
void setLEDState(bool state) {
    // Przygotuj parametry JSON
    String params = "{\"state\":" + String(state ? "true" : "false") + "}";
    
    // Wyślij komendę przez UART
    if (sendCommand("set_led", params)) {
        // Tymczasowo zaktualizuj cache (będzie potwierdzony przez response)
        ledState = state;
        
        logHardwareEvent("LED", state ? "KOMENDA: WŁĄCZ" : "KOMENDA: WYŁĄCZ");
        Serial.printf("[ACTUATORS] LED command sent: %s\n", state ? "ON" : "OFF");
    } else {
        LOG_ERROR_MSG("ACTUATORS", "Failed to send LED command");
        Serial.println("[ACTUATORS] ERROR: Failed to send LED command");
    }
}

/**
 * Pobierz stan LED z cache
 */
bool getLEDState() {
    return ledState;
}

/**
 * Aktualizuj cache na podstawie odpowiedzi z IoT
 */
void updateStateFromResponse(const String& action, const String& result) {
    if (action == "set_led") {
        // Parsuj result JSON i zaktualizuj cache
        if (result.indexOf("\"led_state\":true") != -1) {
            ledState = true;
        } else if (result.indexOf("\"led_state\":false") != -1) {
            ledState = false;
        }
        Serial.printf("[ACTUATORS] LED state confirmed: %s\n", ledState ? "ON" : "OFF");
    }
    
    // *** ROZSZERZENIE: Dodaj parsowanie dla innych urządzeń ***
    /*
    else if (action == "set_relay1") {
        if (result.indexOf("\"relay1_state\":true") != -1) {
            relay1State = true;
        } else if (result.indexOf("\"relay1_state\":false") != -1) {
            relay1State = false;
        }
    }
    else if (action == "set_servo1") {
        // Parsuj pozycję servo z result
        int pos = result.substring(result.indexOf("\"position\":") + 11).toInt();
        if (pos >= 0 && pos <= 180) {
            servo1Position = pos;
        }
    }
    */
}

/**
 * Żądaj synchronizacji stanu z IoT
 */
void requestStateSync() {
    Serial.println("[ACTUATORS] Requesting state sync from IoT...");
    
    // Wyślij komendę get_status aby pobrać wszystkie stany z IoT
    if (sendCommand("get_status", "{}")) {
        LOG_INFO_MSG("ACTUATORS", "State sync request sent");
    } else {
        LOG_ERROR_MSG("ACTUATORS", "Failed to request state sync");
    }
}

/**
 * Przetwórz odpowiedź synchronizacji stanu
 */
void processSyncResponse(const String& result) {
    Serial.println("[ACTUATORS] Processing sync response: " + result);
    
    // Parsuj JSON response i zaktualizuj wszystkie cache
    if (result.indexOf("\"led_state\":true") != -1) {
        ledState = true;
    } else if (result.indexOf("\"led_state\":false") != -1) {
        ledState = false;
    }
    
    // *** ROZSZERZENIE: Parsuj inne urządzenia ***
    /*
    if (result.indexOf("\"relay1_state\":true") != -1) {
        relay1State = true;
    } else if (result.indexOf("\"relay1_state\":false") != -1) {
        relay1State = false;
    }
    
    // Parsuj sensory
    int tempStart = result.indexOf("\"temperature\":") + 14;
    if (tempStart > 13) {
        temperature = result.substring(tempStart, result.indexOf(",", tempStart)).toFloat();
    }
    */
    
    LOG_INFO_MSG("ACTUATORS", "State sync completed");
}

// *** ROZSZERZENIE: Dodaj tutaj funkcje sterowania różnymi urządzeniami ***

// ============= FUNKCJE STEROWANIA PRZEKAŹNIKAMI =============
/*
void setRelayState(int relayNumber, bool state) {
    String action = "set_relay" + String(relayNumber);
    String params = "{\"state\":" + String(state ? "true" : "false") + "}";
    
    if (sendCommand(action, params)) {
        // Tymczasowo zaktualizuj cache
        switch(relayNumber) {
            case 1: relay1State = state; break;
            case 2: relay2State = state; break;
            case 3: relay3State = state; break;
        }
        
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "RELAY_%d KOMENDA: %s", relayNumber, state ? "WŁĄCZ" : "WYŁĄCZ");
        logHardwareEvent("RELAY", buffer);
    } else {
        LOG_ERROR_MSG("ACTUATORS", "Failed to send relay command");
    }
}

bool getRelayState(int relayNumber) {
    switch(relayNumber) {
        case 1: return relay1State;
        case 2: return relay2State;
        case 3: return relay3State;
        default: return false;
    }
}
*/

// ============= FUNKCJE STEROWANIA SERWOMECHANIZMAMI =============
/*
void setServoPosition(int servoNumber, int position) {
    position = constrain(position, 0, 180);
    
    String action = "set_servo" + String(servoNumber);
    String params = "{\"position\":" + String(position) + "}";
    
    if (sendCommand(action, params)) {
        // Tymczasowo zaktualizuj cache
        switch(servoNumber) {
            case 1: servo1Position = position; break;
            case 2: servo2Position = position; break;
        }
        
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "SERVO_%d KOMENDA: %d°", servoNumber, position);
        logHardwareEvent("SERVO", buffer);
    } else {
        LOG_ERROR_MSG("ACTUATORS", "Failed to send servo command");
    }
}

int getServoPosition(int servoNumber) {
    switch(servoNumber) {
        case 1: return servo1Position;
        case 2: return servo2Position;
        default: return 0;
    }
}
*/

// ============= FUNKCJE PWM/DIMMER =============
/*
void setLEDStripBrightness(int brightness) {
    brightness = constrain(brightness, 0, 255);
    
    String params = "{\"brightness\":" + String(brightness) + "}";
    
    if (sendCommand("set_ledstrip", params)) {
        ledStripBrightness = brightness;
        
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "LED_STRIP KOMENDA: %d/255", brightness);
        logHardwareEvent("LED_STRIP", buffer);
    } else {
        LOG_ERROR_MSG("ACTUATORS", "Failed to send LED strip command");
    }
}

void setFanSpeed(int speed) {
    speed = constrain(speed, 0, 255);
    
    String params = "{\"speed\":" + String(speed) + "}";
    
    if (sendCommand("set_fan", params)) {
        fanSpeed = speed;
        
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "FAN KOMENDA: %d/255", speed);
        logHardwareEvent("FAN", buffer);
    } else {
        LOG_ERROR_MSG("ACTUATORS", "Failed to send fan command");
    }
}

int getLEDStripBrightness() {
    return ledStripBrightness;
}

int getFanSpeed() {
    return fanSpeed;
}
*/

// ============= FUNKCJE ODCZYTU SENSORÓW =============
/*
void requestSensorData() {
    Serial.println("[ACTUATORS] Requesting sensor data from IoT...");
    
    if (sendCommand("read_sensors", "{}")) {
        LOG_INFO_MSG("ACTUATORS", "Sensor data request sent");
    } else {
        LOG_ERROR_MSG("ACTUATORS", "Failed to request sensor data");
    }
}

void updateSensorCache(const String& sensorData) {
    // Parsuj dane sensorów z JSON i zaktualizuj cache
    int tempStart = sensorData.indexOf("\"temperature\":") + 14;
    if (tempStart > 13) {
        temperature = sensorData.substring(tempStart, sensorData.indexOf(",", tempStart)).toFloat();
    }
    
    int lightStart = sensorData.indexOf("\"light_level\":") + 14;
    if (lightStart > 13) {
        lightLevel = sensorData.substring(lightStart, sensorData.indexOf(",", lightStart)).toInt();
    }
    
    // Podobnie dla innych sensorów...
    
    Serial.printf("[ACTUATORS] Sensors updated: temp=%.1f°C, light=%d\n", temperature, lightLevel);
}

float getTemperature() {
    return temperature;
}

int getLightLevel() {
    return lightLevel;
}

bool getMotionDetected() {
    return motionDetected;
}
*/

/**
 * Pobierz status wszystkich urządzeń (cache)
 */
String getActuatorStatus() {
    String status = "LED: " + String(ledState ? "ON" : "OFF");
    
    // *** ROZSZERZENIE: Dodaj status innych urządzeń ***
    /*
    status += ", RELAY1: " + String(relay1State ? "ON" : "OFF");
    status += ", RELAY2: " + String(relay2State ? "ON" : "OFF");
    status += ", RELAY3: " + String(relay3State ? "ON" : "OFF");
    status += ", SERVO1: " + String(servo1Position) + "°";
    status += ", SERVO2: " + String(servo2Position) + "°";
    status += ", LED_STRIP: " + String(ledStripBrightness) + "/255";
    status += ", FAN: " + String(fanSpeed) + "/255";
    */
    
    return status;
}

/**
 * Aktualizuj stany actuatorów (dla synchronizacji)
 */
void updateActuatorStates() {
    // W trybie UART nie sprawdzamy pinów GPIO - stan pochodzi tylko z IoT
    // Można tutaj wysłać request o aktualny stan jeśli cache jest stary
    
    static unsigned long lastSyncRequest = 0;
    if (millis() - lastSyncRequest > 60000) { // Sync co minutę
        requestStateSync();
        lastSyncRequest = millis();
    }
}