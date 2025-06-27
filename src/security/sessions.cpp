#include "sessions.h"
#include "../core/logging.h"

// ================= ZMIENNE GLOBALNE =================
SessionInfo sessions[MAX_SESSIONS];
int sessionCount = 0;
unsigned long lastSessionCleanup = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja systemu sesji
 */
void initializeSessions() {
    sessionCount = 0;
    lastSessionCleanup = millis();
    
    // Wyczyść wszystkie sesje
    for (int i = 0; i < MAX_SESSIONS; i++) {
        sessions[i].isValid = false;
        sessions[i].token = "";
        sessions[i].ip = IPAddress(0, 0, 0, 0);
        sessions[i].createdAt = 0;
        sessions[i].lastActivity = 0;
    }
    
    LOG_INFO_MSG("SESSIONS", "System sesji zainicjalizowany");
}

/**
 * Generuj losowy token sesji
 */
String generateSessionToken() {
    String token = "";
    const char chars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    for (int i = 0; i < 32; i++) {
        token += chars[random(0, 62)];
    }
    
    return token;
}

/**
 * Utwórz nową sesję
 */
String createSession(IPAddress ip) {
    String token = generateSessionToken();
    unsigned long now = millis();
    
    // Znajdź wolne miejsce lub najstarszą sesję
    int index = -1;
    if (sessionCount < MAX_SESSIONS) {
        index = sessionCount++;
    } else {
        // Znajdź najstarszą sesję
        unsigned long oldestTime = sessions[0].lastActivity;
        index = 0;
        for (int i = 1; i < MAX_SESSIONS; i++) {
            if (sessions[i].lastActivity < oldestTime) {
                oldestTime = sessions[i].lastActivity;
                index = i;
            }
        }
    }
    
    // Utwórz sesję
    sessions[index].token = token;
    sessions[index].ip = ip;
    sessions[index].createdAt = now;
    sessions[index].lastActivity = now;
    sessions[index].isValid = true;
    
    return token;
}

/**
 * Znajdź sesję po tokenie
 */
SessionInfo* findSessionByToken(const String& token) {
    unsigned long now = millis();
    
    for (int i = 0; i < sessionCount; i++) {
        if (sessions[i].isValid && sessions[i].token == token) {
            // Sprawdź czy sesja nie wygasła
            if (now - sessions[i].lastActivity < SESSION_TIMEOUT) {
                sessions[i].lastActivity = now;  // Odśwież aktywność
                return &sessions[i];
            } else {
                // Sesja wygasła
                sessions[i].isValid = false;
                return nullptr;
            }
        }
    }
    return nullptr;
}

/**
 * Usuń sesję
 */
void removeSession(const String& token) {
    for (int i = 0; i < sessionCount; i++) {
        if (sessions[i].token == token) {
            sessions[i].isValid = false;
            break;
        }
    }
}

/**
 * Wyczyść wygasłe sesje
 */
void cleanupExpiredSessions() {
    unsigned long now = millis();
    int cleanedSessions = 0;
    
    for (int i = 0; i < sessionCount; i++) {
        if (!sessions[i].isValid || 
            (now - sessions[i].lastActivity) > SESSION_TIMEOUT) {
            if (sessions[i].isValid) {
                sessions[i].isValid = false;
                cleanedSessions++;
            }
        }
    }
    
    if (cleanedSessions > 0) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Wyczyszczono %d wygasłych sesji", cleanedSessions);
        LOG_INFO_MSG("SESSIONS", buffer);
    }
    
    lastSessionCleanup = now;
}

/**
 * Pobierz liczbę aktywnych sesji
 */
int getActiveSessionCount() {
    int activeSessions = 0;
    unsigned long now = millis();
    
    for (int i = 0; i < sessionCount; i++) {
        if (sessions[i].isValid && (now - sessions[i].lastActivity) < SESSION_TIMEOUT) {
            activeSessions++;
        }
    }
    
    return activeSessions;
}