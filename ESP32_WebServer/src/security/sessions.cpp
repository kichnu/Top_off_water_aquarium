#include "sessions.h"
#include "../config/network_security.h"
#include "../core/logging.h"
#include <vector>
#include <vector>
#include <map>

static std::vector<Session> active_sessions;

void initializeSessions() {
    active_sessions.clear();
    LOG_INFO("Session management initialized");
}

void updateSessions() {
    clearExpiredSessions();
}

String generateToken() {
    String token = "";
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    for (int i = 0; i < SESSION_TOKEN_LENGTH; i++) {
        token += charset[random(0, strlen(charset))];
    }
    
    return token;
}

String createSession(IPAddress ip) {
    // Remove existing sessions for this IP
    for (auto it = active_sessions.begin(); it != active_sessions.end();) {
        if (it->ip == ip) {
            it = active_sessions.erase(it);
        } else {
            ++it;
        }
    }
    
    Session new_session;
    new_session.token = generateToken();
    new_session.ip = ip;
    new_session.created_at = millis();
    new_session.last_activity = millis();
    new_session.is_valid = true;
    
    active_sessions.push_back(new_session);
    
    LOG_INFO("Session created for IP %s, token: %s", ip.toString().c_str(), new_session.token.c_str());
    
    return new_session.token;
}

bool validateSession(const String& token, IPAddress ip) {
    for (auto& session : active_sessions) {
        if (session.token == token && session.ip == ip && session.is_valid) {
            // Check timeout
            if (millis() - session.last_activity > SESSION_TIMEOUT_MS) {
                session.is_valid = false;
                LOG_INFO("Session expired for IP %s", ip.toString().c_str());
                return false;
            }
            
            // Update last activity
            session.last_activity = millis();
            return true;
        }
    }
    
    return false;
}

void destroySession(const String& token) {
    for (auto it = active_sessions.begin(); it != active_sessions.end(); ++it) {
        if (it->token == token) {
            LOG_INFO("Session destroyed for IP %s", it->ip.toString().c_str());
            active_sessions.erase(it);
            break;
        }
    }
}

void clearExpiredSessions() {
    unsigned long now = millis();
    
    for (auto it = active_sessions.begin(); it != active_sessions.end();) {
        if (!it->is_valid || (now - it->last_activity > SESSION_TIMEOUT_MS)) {
            LOG_DEBUG("Removing expired session for IP %s", it->ip.toString().c_str());
            it = active_sessions.erase(it);
        } else {
            ++it;
        }
    }
}