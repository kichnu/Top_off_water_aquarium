#include "network_security.h"

bool isLocalIPAllowed(IPAddress ip) {
    for (int i = 0; i < LOCAL_WHITELIST_COUNT; i++) {
        if (LOCAL_WHITELIST[i] == ip) {
            return true;
        }
    }
    return false;
}