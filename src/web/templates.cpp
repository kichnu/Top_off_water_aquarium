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
            <p>LAN Dashboard</p>
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
            • IP Whitelist + Rate limiting<br>
            • Sesje z automatycznym timeout<br>
            • Blokada po przekroczeniu limitów
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
                .then(data => updateLEDUI(data.state))
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
        
        function toggleLED() {
            const ledSwitch = document.getElementById('ledSwitch');
            const newState = ledSwitch.checked;
            
            fetch('/api/led/toggle', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ state: newState })
            })
            .then(response => response.ok ? response.json() : Promise.reject('Błąd serwera'))
            .then(data => updateLEDUI(data.state))
            .catch(error => {
                console.error('Błąd przełączania LED:', error);
                ledSwitch.checked = !newState;
                alert('Błąd podczas przełączania LED');
            });
        }
        
        // *** ROZSZERZENIE: Funkcje JavaScript dla nowych urządzeń ***
        
        // ============= STEROWANIE PRZEKAŹNIKAMI =============
        /*
        function toggleRelay(relayNumber) {
            fetch('/api/relay/' + relayNumber + '/toggle', { method: 'POST' })
                .then(response => response.json())
                .then(data => updateRelayUI(relayNumber, data.state))
                .catch(error => console.error('Błąd sterowania przekaźnikiem:', error));
        }
        
        function updateRelayUI(relayNumber, state) {
            const relaySwitch = document.getElementById('relay' + relayNumber + 'Switch');
            const relayStatus = document.getElementById('relay' + relayNumber + 'Status');
            
            relaySwitch.checked = state;
            relayStatus.textContent = 'Przekaźnik ' + relayNumber + ': ' + (state ? 'WŁĄCZONY' : 'WYŁĄCZONY');
            relayStatus.className = state ? 'device-status device-on' : 'device-status device-off';
        }
        */
        
        // ============= STEROWANIE SERWOMECHANIZMAMI =============
        /*
        function setServoPosition(servoNumber, position) {
            fetch('/api/servo/' + servoNumber + '/set', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ position: parseInt(position) })
            })
            .then(response => response.json())
            .then(data => {
                document.getElementById('servo' + servoNumber + 'Position').textContent = data.position + '°';
            })
            .catch(error => console.error('Błąd sterowania servo:', error));
        }
        */
        
        // ============= STEROWANIE PWM/DIMMER =============
        /*
        function setLedStripBrightness(brightness) {
            fetch('/api/ledstrip/set', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ brightness: parseInt(brightness) })
            })
            .then(response => response.json())
            .then(data => {
                document.getElementById('ledStripValue').textContent = data.brightness + '/255';
            });
        }
        
        function setFanSpeed(speed) {
            fetch('/api/fan/set', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ speed: parseInt(speed) })
            })
            .then(response => response.json())
            .then(data => {
                document.getElementById('fanSpeedValue').textContent = data.speed + '/255';
            });
        }
        */
        
        // ============= ODCZYT SENSORÓW =============
        /*
        function readSensors() {
            fetch('/api/sensors/read')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temperature').textContent = data.temperature + '°C';
                    document.getElementById('lightLevel').textContent = data.lightLevel;
                    document.getElementById('moistureLevel').textContent = data.moistureLevel;
                    document.getElementById('motionStatus').textContent = data.motionDetected ? 'WYKRYTY' : 'BRAK';
                    document.getElementById('doorStatus').textContent = data.doorOpen ? 'OTWARTE' : 'ZAMKNIĘTE';
                })
                .catch(error => console.error('Błąd odczytu sensorów:', error));
        }
        */
        
        function getSystemInfo() {
            fetch('/api/system/info')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ipAddress').textContent = data.ip;
                    document.getElementById('uptime').textContent = formatUptime(parseInt(data.uptime));
                    document.getElementById('freeHeap').textContent = data.freeHeap + ' bytes';
                    document.getElementById('rssi').textContent = data.rssi + ' dBm';
                    
                    // Pokaż informacje o czasie jeśli dostępne
                    if (data.currentTime) {
                        document.getElementById('currentTime').textContent = data.currentTime;
                    }
                    if (data.timezone) {
                        document.getElementById('timezone').textContent = data.timezone;
                    }
                    if (data.ntpSynced !== undefined) {
                        document.getElementById('ntpStatus').textContent = data.ntpSynced ? 'Zsynchronizowany' : 'Nie zsynchronizowany';
                        document.getElementById('ntpStatus').className = data.ntpSynced ? 'ntp-synced' : 'ntp-not-synced';
                    }
                })
                .catch(error => console.error('Błąd pobierania informacji o systemie:', error));
        }
        
        function formatUptime(milliseconds) {
            const seconds = Math.floor(milliseconds / 1000);
            const minutes = Math.floor(seconds / 60);
            const hours = Math.floor(minutes / 60);
            const days = Math.floor(hours / 24);
            
            if (days > 0) {
                return days + 'd ' + (hours % 24) + 'h ' + (minutes % 60) + 'm';
            } else if (hours > 0) {
                return hours + 'h ' + (minutes % 60) + 'm ' + (seconds % 60) + 's';
            } else if (minutes > 0) {
                return minutes + 'm ' + (seconds % 60) + 's';
            } else {
                return seconds + 's';
            }
        }
        
        function logout() {
            fetch('/logout', { method: 'POST' })
                .then(() => {
                    window.location.href = '/';
                });
        }
        
        window.onload = function() {
            checkLEDStatus();
            getSystemInfo();
            setInterval(getSystemInfo, 30000);
            
            // *** ROZSZERZENIE: Inicjalizacja nowych funkcji ***
            // setInterval(readSensors, 5000);     // Odczyt sensorów co 5 sekund
            // setInterval(checkSchedules, 60000); // Sprawdzanie harmonogramów co minutę
        };
)rawliteral";

// ================= STRONA DASHBOARD (część 1) =================
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
            max-width: 1200px;
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
        .card {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
            margin-bottom: 20px;
        }
        .card-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
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
        .led-on, .device-on {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .led-off, .device-off {
            background: #f8f9fa;
            color: #6c757d;
            border: 1px solid #dee2e6;
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
        .info-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }
        .info-item {
            background: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            border-left: 4px solid #667eea;
        }
        .info-label {
            font-weight: bold;
            color: #333;
            margin-bottom: 5px;
        }
        .info-value {
            color: #666;
            font-family: monospace;
        }
        .ntp-synced {
            color: #28a745;
            font-weight: bold;
        }
        .ntp-not-synced {
            color: #dc3545;
            font-weight: bold;
        }
        
        /* *** ROZSZERZENIE: Style CSS dla nowych elementów *** */
        
        .control-button {
            background: #007bff;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            margin: 5px;
        }
        .control-button:hover {
            background: #0056b3;
        }
        .control-button.danger {
            background: #dc3545;
        }
        .control-button.danger:hover {
            background: #c82333;
        }
        .slider-control {
            width: 100%;
            margin: 20px 0;
        }
        .slider-control input[type="range"] {
            width: 100%;
            height: 25px;
        }
        .slider-control .value-display {
            font-weight: bold;
            color: #333;
            margin-left: 10px;
        }
        .sensor-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-top: 15px;
        }
        .sensor-item {
            background: #e9ecef;
            padding: 10px;
            border-radius: 5px;
            text-align: center;
        }
        .sensor-value {
            font-size: 24px;
            font-weight: bold;
            color: #495057;
        }
        .sensor-label {
            font-size: 12px;
            color: #6c757d;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <button class="logout-btn" onclick="logout()">Wyloguj</button>
            <h1>🚀 ESP32 LAN Dashboard</h1>
            <p>Inteligentny dom - Panel sterowania</p>
        </div>
        
        <!-- Główne sterowanie LED -->
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
                
                <p>Użyj przełącznika powyżej aby włączyć/wyłączyć LED</p>
            </div>
        </div>
        
        <!-- *** ROZSZERZENIE: Karty dla nowych urządzeń *** -->
        
        <!-- Informacje o systemie -->
        <div class="card">
            <h2>📊 Informacje o systemie</h2>
            <div class="info-grid">
                <div class="info-item">
                    <div class="info-label">Adres IP</div>
                    <div class="info-value" id="ipAddress">Ładowanie...</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Czas działania</div>
                    <div class="info-value" id="uptime">Ładowanie...</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Wolna pamięć</div>
                    <div class="info-value" id="freeHeap">Ładowanie...</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Siła sygnału WiFi</div>
                    <div class="info-value" id="rssi">Ładowanie...</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Aktualny czas</div>
                    <div class="info-value" id="currentTime">Ładowanie...</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Strefa czasowa</div>
                    <div class="info-value" id="timezone">Ładowanie...</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Synchronizacja NTP</div>
                    <div class="info-value" id="ntpStatus">Ładowanie...</div>
                </div>
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