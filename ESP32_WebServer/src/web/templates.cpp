#include "templates.h"
#include "server.h"
#include "../core/logging.h"

// ================= STRONA DASHBOARD =================
const char* DASHBOARD_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>System Nawadniania ESP32-C6</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #74b9ff 0%, #0984e3 100%);
            min-height: 100vh;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }
        .header h1 {
            margin: 0;
            font-size: 32px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .logout-btn {
            float: right;
            background: #d63031;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-weight: bold;
            transition: background 0.3s;
        }
        .logout-btn:hover {
            background: #e17055;
        }
        .card {
            background: white;
            padding: 25px;
            margin: 15px 0;
            border-radius: 10px;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }
        .card h2 {
            margin-top: 0;
            color: #2d3436;
            border-bottom: 2px solid #74b9ff;
            padding-bottom: 10px;
        }
        .status-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin: 20px 0;
        }
        .status-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 8px;
            border-left: 4px solid #74b9ff;
        }
        .status-label {
            font-weight: bold;
            color: #2d3436;
        }
        .status-value {
            font-weight: bold;
            color: #0984e3;
        }
        .button {
            background: #00b894;
            color: white;
            border: none;
            padding: 15px 30px;
            border-radius: 8px;
            cursor: pointer;
            font-size: 16px;
            font-weight: bold;
            margin: 10px 5px;
            transition: all 0.3s;
        }
        .button:hover {
            background: #00a085;
            transform: translateY(-2px);
        }
        .button:disabled {
            background: #b2bec3;
            cursor: not-allowed;
            transform: none;
        }
        .button.secondary {
            background: #fdcb6e;
        }
        .button.secondary:hover {
            background: #e17055;
        }
        .alert {
            padding: 15px;
            margin: 15px 0;
            border-radius: 8px;
            font-weight: bold;
        }
        .alert.success {
            background: #d1f2eb;
            color: #27ae60;
            border: 1px solid #27ae60;
        }
        .alert.error {
            background: #fadbd8;
            color: #e74c3c;
            border: 1px solid #e74c3c;
        }
        @media (max-width: 600px) {
            .status-grid {
                grid-template-columns: 1fr;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <button class="logout-btn" onclick="logout()">Wyloguj</button>
            <h1>🌱 System Nawadniania ESP32-C6</h1>
        </div>
        
        <div class="card">
            <h2>📊 Status systemu</h2>
            <div class="status-grid">
                <div class="status-item">
                    <span class="status-label">Czujnik poziomu:</span>
                    <span class="status-value" id="waterStatus">Ładowanie...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">Pompa:</span>
                    <span class="status-value" id="pumpStatus">Ładowanie...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">WiFi:</span>
                    <span class="status-value" id="wifiStatus">Ładowanie...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">RTC czas:</span>
                    <span class="status-value" id="rtcTime">Ładowanie...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">Temperatura RTC:</span>
                    <span class="status-value" id="rtcTemp">Ładowanie...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">Czas pracy:</span>
                    <span class="status-value" id="uptime">Ładowanie...</span>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>⚡ Sterowanie ręczne</h2>
            <button id="normalPumpBtn" class="button" onclick="triggerNormalPump()">
                💧 Normalny cykl
            </button>
            <button id="longPumpBtn" class="button secondary" onclick="trigger1MinPump()">
                ⏰ Tryb długi (1 min)
            </button>
        </div>
        
        <div class="card">
            <h2>⚙️ Ustawienia</h2>
            <button class="button" onclick="window.location.href='/settings'">
                🔧 Konfiguracja systemu
            </button>
        </div>
        
        <div id="notifications"></div>
    </div>

    <script>
        function showNotification(message, type) {
            const notifications = document.getElementById('notifications');
            const alert = document.createElement('div');
            alert.className = 'alert ' + type;
            alert.textContent = message;
            notifications.appendChild(alert);
            
            setTimeout(function() {
                if (notifications.contains(alert)) {
                    notifications.removeChild(alert);
                }
            }, 5000);
        }
        
        function triggerNormalPump() {
            const button = document.getElementById('normalPumpBtn');
            button.disabled = true;
            button.textContent = 'Uruchamianie...';
            
            fetch('/api/pump/manual-normal', { method: 'POST' })
                .then(function(response) { return response.json(); })
                .then(function(data) {
                    if (data.success) {
                        showNotification('Pompa uruchomiona na ' + data.duration + 's', 'success');
                        setTimeout(updateStatus, 2000);
                    } else {
                        showNotification('Błąd uruchomienia pompy', 'error');
                    }
                })
                .catch(function(error) {
                    showNotification('Błąd komunikacji', 'error');
                })
                .finally(function() {
                    button.disabled = false;
                    button.textContent = '💧 Normalny cykl';
                });
        }
        
        function trigger1MinPump() {
            const button = document.getElementById('longPumpBtn');
            button.disabled = true;
            button.textContent = 'Uruchamianie...';
            
            fetch('/api/pump/manual-1min', { method: 'POST' })
                .then(function(response) { return response.json(); })
                .then(function(data) {
                    if (data.success) {
                        showNotification('Pompa uruchomiona na 1 minutę', 'success');
                        setTimeout(updateStatus, 2000);
                    } else {
                        showNotification('Błąd uruchomienia pompy', 'error');
                    }
                })
                .catch(function(error) {
                    showNotification('Błąd komunikacji', 'error');
                })
                .finally(function() {
                    button.disabled = false;
                    button.textContent = '⏰ Tryb długi (1 min)';
                });
        }
        
        function updateStatus() {
            fetch('/api/status')
                .then(function(response) { return response.json(); })
                .then(function(data) {
                    document.getElementById('waterStatus').textContent = data.water_status;
                    document.getElementById('pumpStatus').textContent = data.pump_running ? 
                        'Aktywna (' + data.pump_remaining + 's)' : 'Nieaktywna';
                    document.getElementById('wifiStatus').textContent = data.wifi_status;
                    document.getElementById('rtcTime').textContent = data.rtc_time || 'Brak';
                    document.getElementById('rtcTemp').textContent = data.rtc_temperature ? 
                        data.rtc_temperature.toFixed(1) + '°C' : 'Brak';
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                })
                .catch(function(error) {
                    console.error('Status update failed:', error);
                });
        }
        
        function formatUptime(milliseconds) {
            const seconds = Math.floor(milliseconds / 1000);
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            return hours + 'h ' + minutes + 'm';
        }
        
        function logout() {
            fetch('/api/logout', { method: 'POST' })
                .then(function() {
                    window.location.href = '/login';
                });
        }
        
        // Auto-refresh status every 5 seconds
        setInterval(updateStatus, 5000);
        updateStatus(); // Initial load
    </script>
</body>
</html>
)rawliteral";

// ================= STRONA LOGOWANIA =================
const char* LOGIN_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Logowanie - System Nawadniania</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #74b9ff 0%, #0984e3 100%);
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
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
            width: 100%;
            max-width: 400px;
        }
        .logo {
            text-align: center;
            margin-bottom: 30px;
        }
        .logo h1 {
            color: #2d3436;
            margin: 0;
            font-size: 28px;
        }
        .logo p {
            color: #636e72;
            margin: 5px 0 0 0;
            font-size: 14px;
        }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 8px;
            color: #2d3436;
            font-weight: bold;
        }
        input[type="password"] {
            width: 100%;
            padding: 15px;
            border: 2px solid #ddd;
            border-radius: 8px;
            font-size: 16px;
            box-sizing: border-box;
            transition: border-color 0.3s;
        }
        input[type="password"]:focus {
            outline: none;
            border-color: #74b9ff;
        }
        .login-btn {
            width: 100%;
            padding: 15px;
            background: linear-gradient(135deg, #74b9ff 0%, #0984e3 100%);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: transform 0.2s;
        }
        .login-btn:hover {
            transform: translateY(-2px);
        }
        .alert {
            padding: 15px;
            margin: 15px 0;
            border-radius: 8px;
            font-weight: bold;
            display: none;
        }
        .alert.error {
            background: #fadbd8;
            color: #e74c3c;
            border: 1px solid #e74c3c;
        }
        .security-info {
            margin-top: 25px;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 8px;
            font-size: 12px;
            color: #636e72;
        }
    </style>
</head>
<body>
    <div class="login-container">
        <div class="logo">
            <h1>🌱 System Nawadniania</h1>
            <p>ESP32-C6 Water Control</p>
        </div>
        <form id="loginForm">
            <div class="form-group">
                <label for="password">Hasło administratora:</label>
                <input type="password" id="password" name="password" required>
            </div>
            <button type="submit" class="login-btn">Zaloguj się</button>
        </form>
        <div id="error" class="alert error"></div>
        <div class="security-info">
            <strong>🛡️ Zabezpieczenia:</strong><br>
            • Rate limiting + Session management<br>
            • 🌐 RTC + VPS logging<br>
            • Automatyczne timeout sesji
        </div>
    </div>

    <script>
        document.getElementById('loginForm').addEventListener('submit', function(e) {
            e.preventDefault();
            
            const password = document.getElementById('password').value;
            const errorDiv = document.getElementById('error');
            
            fetch('/api/login', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: 'password=' + encodeURIComponent(password)
            })
            .then(function(response) { return response.json(); })
            .then(function(data) {
                if (data.success) {
                    window.location.href = '/';
                } else {
                    errorDiv.textContent = 'Nieprawidłowe hasło';
                    errorDiv.style.display = 'block';
                }
            })
            .catch(function(error) {
                errorDiv.textContent = 'Błąd połączenia';
                errorDiv.style.display = 'block';
            });
        });
    </script>
</body>
</html>
)rawliteral";

// ================= STRONA USTAWIEŃ =================
const char* SETTINGS_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ustawienia - System Nawadniania</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #74b9ff 0%, #0984e3 100%);
            min-height: 100vh;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }
        .header h1 {
            margin: 0;
            font-size: 32px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .back-btn {
            float: right;
            background: #636e72;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-weight: bold;
            transition: background 0.3s;
        }
        .back-btn:hover {
            background: #2d3436;
        }
        .card {
            background: white;
            padding: 25px;
            margin: 15px 0;
            border-radius: 10px;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }
        .card h2 {
            margin-top: 0;
            color: #2d3436;
            border-bottom: 2px solid #74b9ff;
            padding-bottom: 10px;
        }
        .form-group {
            margin: 20px 0;
        }
        .form-group label {
            display: block;
            margin-bottom: 8px;
            font-weight: bold;
            color: #2d3436;
        }
        .form-group input,
        .form-group select {
            width: 100%;
            padding: 12px;
            border: 2px solid #ddd;
            border-radius: 8px;
            font-size: 16px;
            box-sizing: border-box;
            transition: border-color 0.3s;
        }
        .form-group input:focus,
        .form-group select:focus {
            outline: none;
            border-color: #74b9ff;
        }
        .button {
            background: #00b894;
            color: white;
            border: none;
            padding: 15px 30px;
            border-radius: 8px;
            cursor: pointer;
            font-size: 16px;
            font-weight: bold;
            margin: 10px 5px;
            transition: all 0.3s;
        }
        .button:hover {
            background: #00a085;
            transform: translateY(-2px);
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <button class="back-btn" onclick="window.location.href='/'">Powrót</button>
            <h1>⚙️ Ustawienia systemu</h1>
        </div>
        
        <div class="card">
            <h2>🔧 Konfiguracja pompy</h2>
            <form id="pumpSettings">
                <div class="form-group">
                    <label for="pumpTime">Czas pompowania (sekundy):</label>
                    <input type="number" id="pumpTime" min="5" max="300" value="15">
                </div>
                <div class="form-group">
                    <label for="autoPump">Automatyczne pompowanie:</label>
                    <select id="autoPump">
                        <option value="true">Włączone</option>
                        <option value="false">Wyłączone</option>
                    </select>
                </div>
                <button type="submit" class="button">Zapisz ustawienia pompy</button>
            </form>
        </div>
        
        <div class="card">
            <h2>🔐 Zmiana hasła</h2>
            <form id="passwordSettings">
                <div class="form-group">
                    <label for="newPassword">Nowe hasło:</label>
                    <input type="password" id="newPassword" minlength="6">
                </div>
                <div class="form-group">
                    <label for="confirmPassword">Potwierdź hasło:</label>
                    <input type="password" id="confirmPassword" minlength="6">
                </div>
                <button type="submit" class="button">Zmień hasło</button>
            </form>
        </div>
    </div>

    <script>
        // Load current settings
        fetch('/api/settings')
            .then(function(response) { return response.json(); })
            .then(function(data) {
                document.getElementById('pumpTime').value = data.pump_time;
                document.getElementById('autoPump').value = data.auto_pump.toString();
            });
        
        document.getElementById('pumpSettings').addEventListener('submit', function(e) {
            e.preventDefault();
            alert('Funkcjonalność zapisu ustawień zostanie zaimplementowana');
        });
        
        document.getElementById('passwordSettings').addEventListener('submit', function(e) {
            e.preventDefault();
            alert('Funkcjonalność zmiany hasła zostanie zaimplementowana');
        });
    </script>
</body>
</html>
)rawliteral";



// ================= FUNKCJE OBSŁUGI =================

void handleDashboard(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->redirect("/login");
        return;
    }
    
    request->send(200, "text/html", DASHBOARD_HTML);
}

void handleLoginPage(AsyncWebServerRequest* request) {
    request->send(200, "text/html", LOGIN_HTML);
}

void handleSettingsPage(AsyncWebServerRequest* request) {
    if (!checkAuthentication(request)) {
        request->redirect("/login");
        return;
    }
    
    request->send(200, "text/html", SETTINGS_HTML);
}

String getDashboardHTML() {
    return String(DASHBOARD_HTML);
}

String getLoginHTML() {
    return String(LOGIN_HTML);
}