#include "templates.h"

// ================= STRONA LOGOWANIA =================
const char* LOGIN_PAGE_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>ESP32 Secure Login</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }
        .login-container {
            background: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 10px 25px rgba(0,0,0,0.2);
            width: 100%;
            max-width: 400px;
        }
        .logo {
            text-align: center;
            margin-bottom: 30px;
        }
        .logo h1 {
            color: #333;
            margin: 0;
            font-size: 28px;
        }
        .logo p {
            color: #666;
            margin: 5px 0 0 0;
            font-size: 14px;
        }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 8px;
            color: #333;
            font-weight: bold;
        }
        input[type="password"] {
            width: 100%;
            padding: 12px;
            border: 2px solid #ddd;
            border-radius: 5px;
            font-size: 16px;
            box-sizing: border-box;
            transition: border-color 0.3s;
        }
        input[type="password"]:focus {
            outline: none;
            border-color: #667eea;
        }
        .login-btn {
            width: 100%;
            padding: 12px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 5px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: transform 0.2s;
        }
        .login-btn:hover {
            transform: translateY(-2px);
        }
        .security-info {
            margin-top: 20px;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 5px;
            font-size: 12px;
            color: #666;
        }
    </style>
</head>
<body>
    <div class="login-container">
        <div class="logo">
            <h1>🔒 ESP32 Secure</h1>
            <p>LAN Dashboard + IoT Control</p>
        </div>
        <form method="POST" action="/login">
            <div class="form-group">
                <label for="password">Hasło administratora:</label>
                <input type="password" id="password" name="password" required>
            </div>
            <button type="submit" class="login-btn">Zaloguj się</button>
        </form>
        <div class="security-info">
            <strong>🛡️ Zabezpieczenia:</strong><br>
            • Rate limiting + Session management<br>
            • 📡 UART komunikacja z IoT<br>
            • Automatyczne timeout sesji
        </div>
    </div>
</body>
</html>
)rawliteral";

// ================= JAVASCRIPT DASHBOARD =================
const char* DASHBOARD_JS = R"rawliteral(
        function checkLEDStatus() {
            fetch('/api/led/status')
                .then(response => response.json())
                .then(data => {
                    updateLEDUI(data.state);
                    updateIoTStatus(data.iot_connected, data.source);
                })
                .catch(error => console.error('Błąd sprawdzania stanu LED:', error));
        }
        
        function updateLEDUI(state) {
            const ledSwitch = document.getElementById('ledSwitch');
            const ledStatus = document.getElementById('ledStatus');
            
            ledSwitch.checked = state;
            
            if (state) {
                ledStatus.textContent = 'LED jest WŁĄCZONA';
                ledStatus.className = 'led-status led-on';
            } else {
                ledStatus.textContent = 'LED jest WYŁĄCZONA';
                ledStatus.className = 'led-status led-off';
            }
        }
        
        function updateIoTStatus(connected, source) {
            const iotIndicator = document.getElementById('iotStatus');
            if (iotIndicator) {
                if (connected) {
                    iotIndicator.textContent = '🤖 IoT: Połączony';
                    iotIndicator.className = 'iot-status iot-connected';
                } else {
                    iotIndicator.textContent = '🤖 IoT: Rozłączony (' + (source || 'cache') + ')';
                    iotIndicator.className = 'iot-status iot-disconnected';
                }
            }
        }
        
        function updateStatusInfo() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Update WiFi info
                    const wifiInfo = document.getElementById('wifiInfo');
                    if (wifiInfo) {
                        if (data.wifi.connected) {
                            wifiInfo.textContent = 'WiFi: ' + data.wifi.ip;
                        } else {
                            wifiInfo.textContent = 'WiFi: Rozłączony';
                        }
                    }
                    
                    // Update NTP info
                    const ntpInfo = document.getElementById('ntpInfo');
                    if (ntpInfo) {
                        if (data.ntp.synced) {
                            ntpInfo.textContent = 'Czas: ' + data.ntp.time;
                        } else {
                            ntpInfo.textContent = 'Czas: Brak sync';
                        }
                    }
                    
                    // Update heartbeat info
                    const heartbeatInfo = document.getElementById('heartbeatInfo');
                    if (heartbeatInfo) {
                        if (data.iot.connected) {
                            heartbeatInfo.textContent = 'Heartbeat: ' + formatHeartbeat(data.iot.heartbeat_ago);
                        } else {
                            heartbeatInfo.textContent = 'Heartbeat: Brak';
                        }
                    }
                })
                .catch(error => {
                    console.error('Błąd pobierania statusu:', error);
                    // Set fallback values
                    const wifiInfo = document.getElementById('wifiInfo');
                    const ntpInfo = document.getElementById('ntpInfo');
                    const heartbeatInfo = document.getElementById('heartbeatInfo');
                    
                    if (wifiInfo) wifiInfo.textContent = 'WiFi: Error';
                    if (ntpInfo) ntpInfo.textContent = 'Czas: Error';
                    if (heartbeatInfo) heartbeatInfo.textContent = 'Heartbeat: Error';
                });
        }
        
        function formatHeartbeat(secondsAgo) {
            if (secondsAgo < 60) {
                return secondsAgo + 's ago';
            } else if (secondsAgo < 3600) {
                return Math.floor(secondsAgo / 60) + 'm ago';
            } else {
                return Math.floor(secondsAgo / 3600) + 'h ago';
            }
        }
        
        function toggleLED() {
            const ledSwitch = document.getElementById('ledSwitch');
            const newState = ledSwitch.checked;
            
            // Show loading
            const ledStatus = document.getElementById('ledStatus');
            const originalText = ledStatus.textContent;
            ledStatus.textContent = 'Wysyłanie komendy...';
            ledStatus.className = 'led-status led-loading';
            
            fetch('/api/led/toggle', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ state: newState })
            })
            .then(response => response.ok ? response.json() : Promise.reject('Błąd serwera'))
            .then(data => {
                updateLEDUI(data.state);
                updateIoTStatus(data.iot_connected);
                
                if (data.status === 'queued') {
                    showNotification('Komenda w kolejce - IoT rozłączony', 'warning');
                } else if (data.pending_commands > 0) {
                    showNotification('Komenda wysłana, oczekiwanie na potwierdzenie...', 'info');
                }
            })
            .catch(error => {
                console.error('Błąd przełączania LED:', error);
                ledSwitch.checked = !newState;
                ledStatus.textContent = originalText;
                ledStatus.className = 'led-status led-off';
                showNotification('Błąd podczas przełączania LED', 'error');
            });
        }
        
        function showNotification(message, type) {
            const notification = document.createElement('div');
            notification.className = 'notification notification-' + type;
            notification.textContent = message;
            notification.style.cssText = `
                position: fixed;
                top: 20px;
                right: 20px;
                padding: 10px 20px;
                border-radius: 5px;
                color: white;
                z-index: 1000;
                background: ${type === 'error' ? '#dc3545' : type === 'warning' ? '#ffc107' : '#007bff'};
            `;
            
            document.body.appendChild(notification);
            
            setTimeout(() => {
                notification.remove();
            }, 3000);
        }
        
        function logout() {
            fetch('/logout', { method: 'POST' })
                .then(() => {
                    window.location.href = '/';
                });
        }
        
        window.onload = function() {
            checkLEDStatus();
            updateStatusInfo();
            setInterval(checkLEDStatus, 30000);  // Check LED status every 30 seconds
            setInterval(updateStatusInfo, 10000); // Update status every 10 seconds for fresh heartbeat
        };
)rawliteral";

// ================= STRONA DASHBOARD =================
const char* DASHBOARD_PAGE_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>ESP32 Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            margin: 0;
            padding: 20px;
            min-height: 100vh;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        .header {
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
            margin-bottom: 20px;
            text-align: center;
            position: relative;
        }
        .header h1 {
            margin: 0;
            color: #333;
        }
        .logout-btn {
            position: absolute;
            top: 20px;
            right: 20px;
            background: #dc3545;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
        }
        .logout-btn:hover {
            background: #c82333;
        }
        .iot-status {
            position: absolute;
            top: 20px;
            left: 20px;
            padding: 5px 10px;
            border-radius: 5px;
            font-size: 12px;
            font-weight: bold;
        }
        .iot-connected {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .iot-disconnected {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .card {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
            margin-bottom: 20px;
        }
        .device-control {
            text-align: center;
        }
        .device-status {
            font-size: 18px;
            margin-bottom: 20px;
            padding: 15px;
            border-radius: 5px;
            font-weight: bold;
        }
        .led-on {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .led-off {
            background: #f8f9fa;
            color: #6c757d;
            border: 1px solid #dee2e6;
        }
        .led-loading {
            background: #fff3cd;
            color: #856404;
            border: 1px solid #ffeaa7;
        }
        .switch-container {
            margin: 20px 0;
        }
        .switch {
            position: relative;
            display: inline-block;
            width: 80px;
            height: 40px;
        }
        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 34px;
        }
        .slider:before {
            position: absolute;
            content: "";
            height: 32px;
            width: 32px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }
        input:checked + .slider {
            background-color: #4CAF50;
        }
        input:checked + .slider:before {
            transform: translateX(40px);
        }
        .status-bar {
            background: #f8f9fa;
            padding: 10px 20px;
            border-radius: 5px;
            margin-bottom: 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 14px;
            color: #666;
            flex-wrap: wrap;
            gap: 10px;
        }
        .status-item {
            display: flex;
            align-items: center;
            gap: 5px;
            min-width: 120px;
        }
        .notification {
            position: fixed;
            top: 20px;
            right: 20px;
            padding: 10px 20px;
            border-radius: 5px;
            color: white;
            z-index: 1000;
            font-weight: bold;
        }
        .notification-error {
            background: #dc3545;
        }
        .notification-warning {
            background: #ffc107;
            color: #212529;
        }
        .notification-info {
            background: #007bff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div id="iotStatus" class="iot-status iot-disconnected">🤖 IoT: Łączenie...</div>
            <button class="logout-btn" onclick="logout()">Wyloguj</button>
            <h1>🚀 ESP32 Dashboard</h1>
            <p>Sterowanie LED przez IoT UART</p>
        </div>
        
        <!-- Status Bar -->
        <div class="status-bar">
            <div class="status-item">
                <span>📶</span>
                <span id="wifiInfo">WiFi: Łączenie...</span>
            </div>
            <div class="status-item">
                <span>🕒</span>
                <span id="ntpInfo">Czas: Sync...</span>
            </div>
            <div class="status-item">
                <span>💓</span>
                <span id="heartbeatInfo">Heartbeat: ---</span>
            </div>
        </div>
        
        <!-- LED Control -->
        <div class="card">
            <div class="device-control">
                <h2>💡 Sterowanie LED</h2>
                <div id="ledStatus" class="device-status led-off">
                    LED jest WYŁĄCZONA
                </div>
                
                <div class="switch-container">
                    <label class="switch">
                        <input type="checkbox" id="ledSwitch" onchange="toggleLED()">
                        <span class="slider"></span>
                    </label>
                </div>
                
                <p>Komenda jest wysyłana przez UART do IoT ESP32</p>
                <small style="color: #666;">Stan LED jest kontrolowany przez IoT urządzenie</small>
            </div>
        </div>
    </div>
    <script>
)rawliteral";

// ================= KONIEC STRONY DASHBOARD =================
const char* DASHBOARD_PAGE_END = R"rawliteral(
    </script>
</body>
</html>
)rawliteral";

// ================= FUNKCJE POMOCNICZE =================

/**
 * Pobierz kompletną stronę dashboard
 */
String getCompleteDashboardPage() {
    return String(DASHBOARD_PAGE_HTML) + String(DASHBOARD_JS) + String(DASHBOARD_PAGE_END);
}

/**
 * Pobierz stronę 404
 */
String get404Page() {
    String message = "<!DOCTYPE html>";
    message += "<html><head><title>404 Not Found</title>";
    message += "<style>body{font-family:Arial;text-align:center;padding:50px;}";
    message += ".error{color:#e74c3c;}.back{margin-top:20px;}";
    message += "a{color:#3498db;text-decoration:none;}</style></head>";
    message += "<body><h1 class='error'>404 - Not Found</h1>";
    message += "<p>Żądana strona nie została znaleziona.</p>";
    message += "<div class='back'><a href='/'>← Powrót do strony głównej</a></div>";
    message += "</body></html>";
    
    return message;
}