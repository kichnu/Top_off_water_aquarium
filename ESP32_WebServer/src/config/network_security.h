#ifndef NETWORK_SECURITY_H
#define NETWORK_SECURITY_H

#include <WiFi.h>

// IP Whitelist for local network access
const IPAddress LOCAL_WHITELIST[] = {
    // Primary local network 192.168.1.x
    IPAddress(192, 168, 1, 100),    // Desktop computer
    IPAddress(192, 168, 1, 101),    // Laptop
    IPAddress(192, 168, 1, 102),    // Mobile phone
    IPAddress(192, 168, 1, 103),    // Tablet
    
    // Alternative network 192.168.0.x  
    IPAddress(192, 168, 0, 100),
    IPAddress(192, 168, 0, 101),
    
    // Router admin interfaces
    IPAddress(192, 168, 1, 1),      // Common router IP
    IPAddress(192, 168, 0, 1),      // Alternative router IP
};
const int LOCAL_WHITELIST_COUNT = sizeof(LOCAL_WHITELIST) / sizeof(LOCAL_WHITELIST[0]);

// Rate limiting constants
#define MAX_REQUESTS_PER_SECOND 5
#define BLOCK_DURATION_MS (60 * 1000)  // 1 minute
#define MAX_FAILED_ATTEMPTS 20

// Session constants
#define SESSION_TIMEOUT_MS (5 * 60 * 1000)  // 5 minutes
#define SESSION_TOKEN_LENGTH 32

bool isLocalIPAllowed(IPAddress ip);

#endif