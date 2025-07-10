#include "server.h"
#include "api.h"
#include "templates.h"
#include "../security/auth.h"
#include "../security/sessions.h"
#include "../network/ntp.h"
#include "../core/logging.h"

// ================= ZMIENNE GLOBALNE =================
AsyncWebServer server(80);  // HTTP na porcie 80
bool serverRunning = false;
unsigned long serverStartTime = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja serwera web
 */
void initializeWebServer() {
    setupRoutes();
    LOG_INFO_MSG("WEBSERVER", "Serwer web zainicjalizowany");
}

/**
 * Uruchomienie serwera
 */
void startWebServer() {
    server.begin();
    serverRunning = true;
    serverStartTime = millis();
    
    Serial.println("[SERVER] ✅ Serwer HTTP uruchomiony na porcie 80");
    Serial.printf("[SERVER] Dostęp: http://%s\n", WiFi.localIP().toString().c_str());
    
    // Wyświetl dozwolone IP (jeśli włączone)
    #if ENABLE_IP_WHITELIST
    Serial.println("[SECURITY] Dozwolone adresy IP:");
    for (int i = 0; i < ALLOWED_IPS_COUNT; i++) {
        Serial.printf("  - %s\n", ALLOWED_IPS[i].toString().c_str());
    }
    #endif
    
    LOG_INFO_MSG("WEBSERVER", "Serwer HTTP uruchomiony");
}

/**
 * Zatrzymanie serwera
 */
void stopWebServer() {
    server.end();
    serverRunning = false;
    LOG_INFO_MSG("WEBSERVER", "Serwer HTTP zatrzymany");
}

/**
 * Konfiguracja tras serwera
 */
void setupRoutes() {
    Serial.println("[SERVER] Konfigurowanie endpointów...");
    
    // ================= GŁÓWNA STRONA =================
    server.on("/", HTTP_GET, handleRoot);
    
    // ================= LOGOWANIE I WYLOGOWANIE =================
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/logout", HTTP_POST, handleLogout);
    
    // ================= API ROUTES =================
    setupAPIRoutes();
    
    // ================= 404 HANDLER =================
    server.onNotFound(handleNotFound);
}

/**
 * Obsługa głównej strony
 */
void handleRoot(AsyncWebServerRequest* request) {
    IPAddress clientIP = request->client()->remoteIP();
    
    // Sprawdź IP whitelist - TYMCZASOWO WYŁĄCZONE
    #if ENABLE_IP_WHITELIST
    if (!isIPAllowed(clientIP)) {
        logSecurityEvent("Dostęp odrzucony - IP nie na whitelist", clientIP);
        request->send(403, "text/plain", "Access Denied");
        return;
    }
    #endif
    
    // Sprawdź czy użytkownik ma już ważną sesję
    bool hasValidSession = false;
    if (request->hasHeader("Cookie")) {
        String cookies = request->getHeader("Cookie")->value();
        
        int sessionStart = cookies.indexOf("session=");
        if (sessionStart != -1) {
            sessionStart += 8;
            int sessionEnd = cookies.indexOf(";", sessionStart);
            if (sessionEnd == -1) sessionEnd = cookies.length();
            
            String sessionToken = cookies.substring(sessionStart, sessionEnd);
            SessionInfo* session = findSessionByToken(sessionToken);
            if (session != nullptr && session->ip == clientIP) {
                hasValidSession = true;
            }
        }
    }
    
    if (hasValidSession) {
        // Użytkownik ma ważną sesję - pokaż dashboard
        String completePage = getCompleteDashboardPage();
        request->send(200, "text/html", completePage);
    } else {
        // Brak sesji - pokaż login
        request->send(200, "text/html", LOGIN_PAGE_HTML);
    }
}

/**
 * Obsługa logowania
 */
void handleLogin(AsyncWebServerRequest* request) {
    IPAddress clientIP = request->client()->remoteIP();
    
    if (processLogin(request)) {
        // Logowanie pomyślne
        String sessionToken = createSession(clientIP);
        String completePage = getCompleteDashboardPage();
        
        // Ustaw cookie z sesją używając prawdziwego czasu Unix
        String cookieHeader;
        if (isNTPSynced()) {
            // Użyj Unix timestamp + SESSION_TIMEOUT
            time_t expireTime = time(nullptr) + (SESSION_TIMEOUT / 1000);
            struct tm* expireTimeInfo = gmtime(&expireTime);
            
            char expireString[64];
            strftime(expireString, sizeof(expireString), "%a, %d %b %Y %H:%M:%S GMT", expireTimeInfo);
            
            cookieHeader = "session=" + sessionToken + "; Path=/; Expires=" + String(expireString) + "; SameSite=Lax";
            Serial.printf("[AUTH] Cookie z NTP: expires %s\n", expireString);
        } else {
            // Fallback: session cookie (bez expiration)
            cookieHeader = "session=" + sessionToken + "; Path=/; SameSite=Lax";
            Serial.println("[AUTH] Cookie fallback: session cookie (brak NTP)");
        }
        
        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", completePage);
        response->addHeader("Set-Cookie", cookieHeader);
        request->send(response);
    } else {
        // Logowanie nieudane
        request->send(401, "text/html", LOGIN_PAGE_HTML);
    }
}

/**
 * Obsługa wylogowania
 */
void handleLogout(AsyncWebServerRequest* request) {
    processLogout(request);
    
    AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "Logged out");
    
    // Wyczyść cookie używając prawidłowego formatu
    if (isNTPSynced()) {
        time_t pastTime = time(nullptr) - 86400; // 24 godziny temu
        struct tm* pastTimeInfo = gmtime(&pastTime);
        char expireString[64];
        strftime(expireString, sizeof(expireString), "%a, %d %b %Y %H:%M:%S GMT", pastTimeInfo);
        response->addHeader("Set-Cookie", "session=; Path=/; Expires=" + String(expireString) + "; SameSite=Lax");
    } else {
        response->addHeader("Set-Cookie", "session=; Path=/; Max-Age=0; SameSite=Lax");
    }
    
    request->send(response);
}

/**
 * Obsługa 404 Not Found
 */
void handleNotFound(AsyncWebServerRequest* request) {
    IPAddress clientIP = request->client()->remoteIP();
    Serial.printf("[HTTP] 404 - %s %s od %s\n", 
                 request->methodToString(), 
                 request->url().c_str(), 
                 clientIP.toString().c_str());
    
    String message = get404Page();
    request->send(404, "text/html", message);
}