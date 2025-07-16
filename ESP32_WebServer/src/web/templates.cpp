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




                // ========== WATER SYSTEM FUNCTIONS ==========
        function checkWaterStatus() {
            fetch('/api/water/status')
                .then(response => response.json())
                .then(data => {
                    updateWaterLevelUI(data.water_level);
                    updatePumpStatusUI(data.pump_active);
                    updateWaterEventsUI(data.events_today);
                    updateIoTStatus(data.iot_connected);
                })
                .catch(error => console.error('Błąd sprawdzania statusu wody:', error));
        }

        function updateWaterLevelUI(level) {
            const waterLevel = document.getElementById('waterLevel');
            if (waterLevel) {
                if (level === 'LOW') {
                    waterLevel.textContent = '💧 Poziom wody: NISKI';
                    waterLevel.className = 'water-status water-low';
                } else {
                    waterLevel.textContent = '💧 Poziom wody: OK';
                    waterLevel.className = 'water-status water-ok';
                }
            }
        }

        function updatePumpStatusUI(active) {
            const pumpStatus = document.getElementById('pumpStatus');
            if (pumpStatus) {
                if (active) {
                    pumpStatus.textContent = '⚡ Pompa: AKTYWNA';
                    pumpStatus.className = 'pump-status pump-active';
                } else {
                    pumpStatus.textContent = '⚡ Pompa: Gotowa';
                    pumpStatus.className = 'pump-status pump-ready';
                }
            }
        }

        function updateWaterEventsUI(events) {
            const eventsToday = document.getElementById('eventsToday');
            if (eventsToday) {
                eventsToday.textContent = 'Zdarzenia dzisiaj: ' + events;
            }
        }

        function manualPump() {
            const pumpButton = document.getElementById('manualPumpBtn');
            if (pumpButton) {
                pumpButton.disabled = true;
                pumpButton.textContent = 'Uruchamianie...';
            }
            
            fetch('/api/water/pump', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.command_sent) {
                        showNotification('Komenda ręcznego pompowania wysłana', 'success');
                        setTimeout(checkWaterStatus, 2000); // Odśwież status po 2s
                    } else {
                        showNotification('Błąd wysyłania komendy', 'error');
                    }
                })
                .catch(error => {
                    console.error('Błąd ręcznego pompowania:', error);
                    showNotification('Błąd komunikacji', 'error');
                })
                .finally(() => {
                    if (pumpButton) {
                        pumpButton.disabled = false;
                        pumpButton.textContent = 'Uruchom pompę';
                    }
                });
        }

        function savePumpConfig() {
            const volumeInput = document.getElementById('volumeInput');
            const timeInput = document.getElementById('timeInput');
            
            if (!volumeInput || !timeInput) return;
            
            const volume = parseInt(volumeInput.value);
            const time = parseInt(timeInput.value);
            
            if (volume < 1 || volume > 1000 || time < 1 || time > 120) {
                showNotification('Nieprawidłowe wartości (Volume: 1-1000ml, Time: 1-120s)', 'error');
                return;
            }
            
            const formData = new FormData();
            formData.append('volume', volume);
            formData.append('time', time);
            
            fetch('/api/water/config', { 
                method: 'POST',
                body: formData
            })
                .then(response => response.json())
                .then(data => {
                    showNotification(`Konfiguracja zapisana: ${volume}ml w ${time}s`, 'success');
                })
                .catch(error => {
                    console.error('Błąd zapisywania konfiguracji:', error);
                    showNotification('Błąd zapisywania konfiguracji', 'error');
                });
        }
        
        window.onload = function() {
            checkLEDStatus();
            checkWaterStatus();
            updateStatusInfo();

            setInterval(checkLEDStatus, 30000);  // Check LED status every 30 seconds
            setInterval(checkWaterStatus, 15000);
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


        .water-system-status {
            margin-bottom: 20px;
        }
        .water-status, .pump-status {
            padding: 10px;
            margin: 5px 0;
            border-radius: 5px;
            font-weight: bold;
            text-align: center;
        }
        .water-low {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .water-ok {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .water-unknown {
            background: #fff3cd;
            color: #856404;
            border: 1px solid #ffeaa7;
        }
        .pump-active {
            background: #d1ecf1;
            color: #0c5460;
            border: 1px solid #bee5eb;
        }
        .pump-ready {
            background: #f8f9fa;
            color: #6c757d;
            border: 1px solid #dee2e6;
        }
        .pump-unknown {
            background: #fff3cd;
            color: #856404;
            border: 1px solid #ffeaa7;
        }
        .events-counter {
            text-align: center;
            margin: 10px 0;
            font-size: 14px;
            color: #666;
        }
        .manual-control, .pump-config {
            margin: 20px 0;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 5px;
        }
        .pump-button {
            background: #007bff;
            font-size: 16px;
            padding: 12px 24px;
        }
        .pump-button:hover {
            background: #0056b3;
        }
        .pump-button:disabled {
            background: #6c757d;
            cursor: not-allowed;
        }
        .config-button {
            background: #28a745;
        }
        .config-button:hover {
            background: #1e7e34;
        }
        .config-row {
            margin: 10px 0;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .config-row label {
            min-width: 120px;
            font-weight: bold;
        }
        .config-row input {
            flex: 1;
            padding: 8px;
            border: 1px solid #ddd;
            border-radius: 4px;
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

        <div class="card">
            <h2>💧 System dolewania wody</h2>
            
            <!-- Status -->
            <div class="water-system-status">
                <div id="waterLevel" class="water-status water-unknown">💧 Poziom wody: Sprawdzanie...</div>
                <div id="pumpStatus" class="pump-status pump-unknown">⚡ Pompa: Sprawdzanie...</div>
                <div id="eventsToday" class="events-counter">Zdarzenia dzisiaj: ---</div>
            </div>
            
            <!-- Manual Control -->
            <div class="manual-control">
                <h3>Ręczne sterowanie</h3>
                <button id="manualPumpBtn" class="control-button pump-button" onclick="manualPump()">
                    Uruchom pompę
                </button>
                <p><small>Uruchamia pompę zgodnie z aktualną konfiguracją</small></p>
            </div>
            
            <!-- Configuration -->
            <div class="pump-config">
                <h3>Konfiguracja pompy</h3>
                <div class="config-row">
                    <label for="volumeInput">Objętość (ml):</label>
                    <input type="number" id="volumeInput" min="1" max="1000" value="50">
                </div>
                <div class="config-row">
                    <label for="timeInput">Czas pracy (s):</label>
                    <input type="number" id="timeInput" min="1" max="120" value="5">
                </div>
                <button class="control-button config-button" onclick="savePumpConfig()">
                    Zapisz konfigurację
                </button>
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