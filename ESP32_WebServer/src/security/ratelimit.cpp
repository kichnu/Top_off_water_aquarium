#include "ratelimit.h"
#include "../config/network_security.h"
#include "../core/logging.h"
#include <map>
#include <vector>


struct RateLimitData {
    std::vector<unsigned long> request_times;
    unsigned long last_request;
    int failed_attempts;
    unsigned long block_until;
};

static std::map<String, RateLimitData> rate_limit_data;

void initializeRateLimit() {
    rate_limit_data.clear();
    LOG_INFO("Rate limiting initialized");
}

void updateRateLimit() {
    unsigned long now = millis();
    
    // Clean up old data every 5 minutes
    static unsigned long last_cleanup = 0;
    if (now - last_cleanup > 300000) {
        for (auto it = rate_limit_data.begin(); it != rate_limit_data.end();) {
            if (now - it->second.last_request > 300000) { // 5 minutes inactive
                it = rate_limit_data.erase(it);
            } else {
                ++it;
            }
        }
        last_cleanup = now;
    }
}

bool isRateLimited(IPAddress ip) {
    if (isLocalIPAllowed(ip)) {
        return false; // No rate limiting for whitelisted IPs
    }
    
    String ip_str = ip.toString();
    if (rate_limit_data.find(ip_str) == rate_limit_data.end()) {
        return false;
    }
    
    RateLimitData& data = rate_limit_data[ip_str];
    unsigned long now = millis();
    
    // Check if IP is blocked
    if (data.block_until > now) {
        return true;
    }
    
    // Clean old request times (older than 1 second)
    data.request_times.erase(
        std::remove_if(data.request_times.begin(), data.request_times.end(),
                      [now](unsigned long time) { return now - time > 1000; }),
        data.request_times.end());
    
    // Check if too many requests in last second
    return data.request_times.size() >= MAX_REQUESTS_PER_SECOND;
}

void recordRequest(IPAddress ip) {
    String ip_str = ip.toString();
    unsigned long now = millis();
    
    RateLimitData& data = rate_limit_data[ip_str];
    data.request_times.push_back(now);
    data.last_request = now;
}

void recordFailedAttempt(IPAddress ip) {
    if (isLocalIPAllowed(ip)) {
        return; // No blocking for whitelisted IPs
    }
    
    String ip_str = ip.toString();
    unsigned long now = millis();
    
    RateLimitData& data = rate_limit_data[ip_str];
    data.failed_attempts++;
    
    if (data.failed_attempts >= MAX_FAILED_ATTEMPTS) {
        data.block_until = now + BLOCK_DURATION_MS;
        data.failed_attempts = 0; // Reset counter
        LOG_WARNING("IP %s blocked for %d seconds due to failed attempts", 
                   ip_str.c_str(), BLOCK_DURATION_MS / 1000);
    }
}

bool isIPBlocked(IPAddress ip) {
    if (isLocalIPAllowed(ip)) {
        return false;
    }
    
    String ip_str = ip.toString();
    if (rate_limit_data.find(ip_str) == rate_limit_data.end()) {
        return false;
    }
    
    return rate_limit_data[ip_str].block_until > millis();
}