#include "auth.h"
#include "sessions.h"
#include "ratelimit.h"
#include "../core/logging.h"

// ================= ZMIENNE GLOBALNE =================
bool authenticationEnabled = true;
unsigned long lastAuthCheck = 0;

// ================= IMPLEMENTACJE =================

/**
 * Sprawdź czy IP jest na liście dozwolonych
 */
bool isIPAllowed(IPAddress ip) {
    for (int i = 0; i < ALLOWED_IPS_COUNT; i++) {
        if (ALLOWED_IPS[i] == ip) {
            return true;
        }
    }
    return false;
}

/**
 * Sprawdź poprawność hasła
 */
bool verifyPassword(const String& password) {
    return (password == ADMIN_PASSWORD);
}

/**
 * Sprawdź uwierzytelnienie (IP + sesja)
 */
bool checkAuthentication(AsyncWebServerRequest* request) {
    IPAddress clientIP = request->client()->remoteIP();
    
    // Sprawdź IP whitelist - TYMCZASOWO WYŁĄCZONE
    #if ENABLE_IP_WHITELIST
    if (!isIPAllowed(clientIP)) {
        logSecurityEvent("Dostęp odrzucony - IP nie na whitelist", clientIP);
        return false;
    }
    #endif
    
    // Sprawdź rate limiting
    if (!checkRateLimit(clientIP)) {
        return false;
    }
    
    // Sprawdź cookie z tokenem sesji
    if (request->hasHeader("Cookie")) {
        String cookies = request->getHeader("Cookie")->value();
        int sessionStart = cookies.indexOf("session=");
        if (sessionStart != -1) {
            sessionStart += 8; // długość "session="
            int sessionEnd = cookies.indexOf(";", sessionStart);
            if (sessionEnd == -1) sessionEnd = cookies.length();
            
            String sessionToken = cookies.substring(sessionStart, sessionEnd);
            SessionInfo* session = findSessionByToken(sessionToken);
            
            if (session != nullptr && session->ip == clientIP) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * Przetwórz żądanie logowania
 */
bool processLogin(AsyncWebServerRequest* request) {
    IPAddress clientIP = request->client()->remoteIP();
    
    // Sprawdź IP whitelist - TYMCZASOWO WYŁĄCZONE
    #if ENABLE_IP_WHITELIST
    if (!isIPAllowed(clientIP)) {
        logSecurityEvent("Logowanie odrzucone - IP nie na whitelist", clientIP);
        return false;
    }
    #endif
    
    if (!checkRateLimit(clientIP)) {
        return false;
    }
    
    String password = "";
    if (request->hasParam("password", true)) {
        password = request->getParam("password", true)->value();
    }
    
    if (verifyPassword(password)) {
        Serial.printf("[AUTH] ✅ Pomyślne logowanie z IP %s\n", clientIP.toString().c_str());
        resetFailedAttempts(clientIP);
        return true;
    } else {
        Serial.printf("[AUTH] ❌ Błędne hasło z IP %s\n", clientIP.toString().c_str());
        recordFailedLogin(clientIP);
        return false;
    }
}

/**
 * Przetwórz żądanie wylogowania
 */
void processLogout(AsyncWebServerRequest* request) {
    IPAddress clientIP = request->client()->remoteIP();
    Serial.printf("[AUTH] Wylogowanie IP %s\n", clientIP.toString().c_str());
    
    if (request->hasHeader("Cookie")) {
        String cookies = request->getHeader("Cookie")->value();
        
        int sessionStart = cookies.indexOf("session=");
        if (sessionStart != -1) {
            sessionStart += 8;
            int sessionEnd = cookies.indexOf(";", sessionStart);
            if (sessionEnd == -1) sessionEnd = cookies.length();
            
            String sessionToken = cookies.substring(sessionStart, sessionEnd);
            removeSession(sessionToken);
        }
    }
}