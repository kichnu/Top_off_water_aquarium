#include "ratelimit.h"
#include "../core/logging.h"

// ================= ZMIENNE GLOBALNE =================
ClientInfo clients[MAX_CLIENTS];
int clientCount = 0;
unsigned long lastClientCleanup = 0;

// ================= IMPLEMENTACJE =================

/**
 * Inicjalizacja systemu rate limiting
 */
void initializeRateLimit() {
    clientCount = 0;
    lastClientCleanup = millis();
    
    // Wyczyść wszystkich klientów
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].ip = IPAddress(0, 0, 0, 0);
        clients[i].lastRequest = 0;
        clients[i].blockedUntil = 0;
        clients[i].failedAttempts = 0;
        clients[i].requestCount = 0;
        clients[i].requestWindow = 0;
    }
    
    LOG_INFO_MSG("RATELIMIT", "System rate limiting zainicjalizowany");
}

/**
 * Znajdź lub utwórz wpis dla klienta na podstawie IP
 */
ClientInfo* findOrCreateClient(IPAddress ip) {
    // Sprawdź czy klient już istnieje
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].ip == ip) {
            return &clients[i];
        }
    }
    
    // Utwórz nowy wpis jeśli jest miejsce
    if (clientCount < MAX_CLIENTS) {
        clients[clientCount].ip = ip;
        clients[clientCount].lastRequest = 0;
        clients[clientCount].blockedUntil = 0;
        clients[clientCount].failedAttempts = 0;
        clients[clientCount].requestCount = 0;
        clients[clientCount].requestWindow = 0;
        return &clients[clientCount++];
    }
    
    // Jeśli brak miejsca, zwróć najstarszy wpis
    int oldestIndex = 0;
    unsigned long oldestTime = clients[0].lastRequest;
    for (int i = 1; i < MAX_CLIENTS; i++) {
        if (clients[i].lastRequest < oldestTime) {
            oldestTime = clients[i].lastRequest;
            oldestIndex = i;
        }
    }
    
    // Zresetuj najstarszy wpis dla nowego IP
    clients[oldestIndex].ip = ip;
    clients[oldestIndex].lastRequest = 0;
    clients[oldestIndex].blockedUntil = 0;
    clients[oldestIndex].failedAttempts = 0;
    clients[oldestIndex].requestCount = 0;
    clients[oldestIndex].requestWindow = 0;
    
    return &clients[oldestIndex];
}

/**
 * Sprawdź rate limiting dla danego IP
 */
bool checkRateLimit(IPAddress ip) {
    ClientInfo* client = findOrCreateClient(ip);
    unsigned long now = millis();
    
    // Sprawdź czy IP jest zablokowane
    if (client->blockedUntil > now) {
        return false;
    }
    
    // Reset blokady jeśli czas minął
    if (client->blockedUntil != 0 && client->blockedUntil <= now) {
        client->blockedUntil = 0;
        client->failedAttempts = 0;
    }
    
    // Sprawdź okno czasowe dla żądań
    if (now - client->requestWindow > RATE_LIMIT_WINDOW) {
        client->requestWindow = now;
        client->requestCount = 1;
    } else {
        client->requestCount++;
    }
    
    // Sprawdź limit żądań
    if (client->requestCount > MAX_REQUESTS_PER_WINDOW) {
        client->blockedUntil = now + BLOCK_DURATION;
        
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "IP %s przekroczył limit żądań (%d/%d)", 
                ip.toString().c_str(), client->requestCount, MAX_REQUESTS_PER_WINDOW);
        logSecurityEvent(buffer, ip);
        
        return false;
    }
    
    client->lastRequest = now;
    return true;
}

/**
 * Zapisz nieudaną próbę logowania
 */
void recordFailedLogin(IPAddress ip) {
    ClientInfo* client = findOrCreateClient(ip);
    client->failedAttempts++;
    
    if (client->failedAttempts >= MAX_FAILED_ATTEMPTS) {
        client->blockedUntil = millis() + BLOCK_DURATION;
        
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "IP %s zablokowane na 1 minutę", ip.toString().c_str());
        logSecurityEvent(buffer, ip);
    }
}

/**
 * Zresetuj licznik nieudanych prób dla IP
 */
void resetFailedAttempts(IPAddress ip) {
    ClientInfo* client = findOrCreateClient(ip);
    if (client->failedAttempts > 0) {
        client->failedAttempts = 0;
    }
}

/**
 * Wyczyść starych klientów
 */
void cleanupOldClients() {
    unsigned long now = millis();
    int cleanedClients = 0;
    
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].blockedUntil == 0 && 
            (now - clients[i].lastRequest) > 3600000) { // 1 godzina nieaktywności
            if (i < clientCount - 1) {
                clients[i] = clients[clientCount - 1];
                i--;
            }
            clientCount--;
            cleanedClients++;
        }
    }
    
    if (cleanedClients > 0) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Wyczyszczono %d starych klientów", cleanedClients);
        LOG_INFO_MSG("RATELIMIT", buffer);
    }
    
    lastClientCleanup = now;
}