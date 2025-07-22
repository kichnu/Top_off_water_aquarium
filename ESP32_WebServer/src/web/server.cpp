#include "server.h"
#include "api.h"
#include "templates.h"
#include "../security/sessions.h"
#include "../security/ratelimit.h"
#include "../config/network_security.h"
#include "../core/logging.h"

AsyncWebServer server(80);

void initializeWebServer() {
    setupRoutes();
    
    server.begin();
    LOG_INFO("Web server started on port 80");
}

bool checkAuthentication(AsyncWebServerRequest* request) {
    IPAddress client_ip = request->client()->remoteIP();
    
    // Check if IP is blocked
    if (isIPBlocked(client_ip)) {
        LOG_WARNING("Blocked IP attempted access: %s", client_ip.toString().c_str());
        return false;
    }
    
    // Check rate limiting
    if (isRateLimited(client_ip)) {
        LOG_WARNING("Rate limited IP: %s", client_ip.toString().c_str());
        recordFailedAttempt(client_ip);
        return false;
    }
    
    // Record this request
    recordRequest(client_ip);
    
    // Check session
    if (request->hasHeader("Cookie")) {
        String cookie = request->getHeader("Cookie")->value();
        int token_start = cookie.indexOf("session_token=");
        if (token_start != -1) {
            token_start += 14; // Length of "session_token="
            int token_end = cookie.indexOf(";", token_start);
            if (token_end == -1) token_end = cookie.length();
            
            String token = cookie.substring(token_start, token_end);
            if (validateSession(token, client_ip)) {
                return true;
            }
        }
    }
    
    recordFailedAttempt(client_ip);
    return false;
}

void setupRoutes() {
    // Static pages
    server.on("/", HTTP_GET, handleDashboard);
    server.on("/login", HTTP_GET, handleLoginPage);
    server.on("/settings", HTTP_GET, handleSettingsPage);
    
    // Authentication
    server.on("/api/login", HTTP_POST, handleLogin);
    server.on("/api/logout", HTTP_POST, handleLogout);
    
    // API endpoints
    server.on("/api/status", HTTP_GET, handleStatus);
    server.on("/api/pump/manual-normal", HTTP_POST, handleManualPumpNormal);
    server.on("/api/pump/manual-1min", HTTP_POST, handleManualPump1Min);
    server.on("/api/settings", HTTP_GET, handleGetSettings);
    server.on("/api/settings", HTTP_POST, handleUpdateSettings);
    
    // 404 handler
    server.onNotFound([](AsyncWebServerRequest* request) {
        LOG_WARNING("404 - Path not found: %s", request->url().c_str());
        request->send(404, "text/plain", "Not Found");
    });
}