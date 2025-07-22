#ifndef AUTH_H
#define AUTH_H

#include <Arduino.h>

void initializeAuth();
bool verifyPassword(const String& password);
String hashPassword(const String& password);
bool isPasswordValid(const String& password);

#endif