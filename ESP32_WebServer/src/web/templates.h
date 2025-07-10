#ifndef WEB_TEMPLATES_H
#define WEB_TEMPLATES_H

#include <Arduino.h>

// ================= HTML TEMPLATES =================
extern const char* LOGIN_PAGE_HTML;
extern const char* DASHBOARD_PAGE_HTML;
extern const char* DASHBOARD_PAGE_END;

// ================= JAVASCRIPT =================
extern const char* DASHBOARD_JS;

// ================= FUNKCJE POMOCNICZE =================
String getCompleteDashboardPage();
String get404Page();

#endif