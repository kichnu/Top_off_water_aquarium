#include "html_pages.h"



const char* LOGIN_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-C3 Water System Login</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; min-height: 100vh;
        }
        .login-box { 
            background: white; padding: 40px; border-radius: 15px; 
            box-shadow: 0 10px 30px rgba(0,0,0,0.3); width: 100%; max-width: 400px; 
        }
        h1 { text-align: center; color: #333; margin-bottom: 30px; }
        .form-group { margin-bottom: 20px; }
        label { display: block; margin-bottom: 8px; font-weight: bold; color: #333; }
        input[type="password"] { 
            width: 100%; padding: 15px; border: 2px solid #ddd; border-radius: 8px; 
            font-size: 16px; box-sizing: border-box;
        }
        input[type="password"]:focus { outline: none; border-color: #667eea; }
        .login-btn { 
            width: 100%; padding: 15px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white; border: none; border-radius: 8px; font-size: 16px; 
            font-weight: bold; cursor: pointer;
        }
        .login-btn:hover { opacity: 0.9; }
        .alert { padding: 15px; margin: 15px 0; border-radius: 8px; display: none; }
        .alert.error { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
        .info { margin-top: 20px; padding: 15px; background: #f8f9fa; border-radius: 8px; font-size: 12px; color: #666; }
    </style>
</head>
<body>
    <div class="login-box">
        <h1>🌱 Water System</h1>
        <form id="loginForm">
            <div class="form-group">
                <label for="password">Administrator Password:</label>
                <input type="password" id="password" name="password" required>
            </div>
            <button type="submit" class="login-btn">Login</button>
        </form>
        <div id="error" class="alert error"></div>
        <div class="info">
            <strong>🔒 Security Features:</strong><br>
            • Session-based authentication<br>
            • Rate limiting & IP filtering<br>
            • VPS event logging
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
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    window.location.href = '/';
                } else {
                    errorDiv.textContent = 'Invalid password';
                    errorDiv.style.display = 'block';
                }
            })
            .catch(error => {
                errorDiv.textContent = 'Connection error';
                errorDiv.style.display = 'block';
            });
        });
    </script>
</body>
</html>
)rawliteral";

const char* DASHBOARD_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-C3 Water System Dashboard</title>
    <style>
        body { 
            font-family: Arial, sans-serif; margin: 0; padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh;
        }
        .container { max-width: 1000px; margin: 0 auto; }
        .header { 
            text-align: center; color: white; margin-bottom: 30px; position: relative;
        }
        .header h1 { margin: 0; font-size: 32px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }
        .logout-btn { 
            position: absolute; top: 0; right: 0; background: #dc3545; color: white; 
            border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; font-weight: bold;
        }
        .logout-btn:hover { background: #c82333; }
        .card { 
            background: white; padding: 25px; margin: 15px 0; border-radius: 15px; 
            box-shadow: 0 8px 25px rgba(0,0,0,0.15);
        }
        .card h2 { 
            margin-top: 0; color: #333; border-bottom: 3px solid #667eea; 
            padding-bottom: 10px; font-size: 24px;
        }
        .status-grid { 
            display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); 
            gap: 15px; margin: 20px 0;
        }
        .status-item { 
            display: flex; justify-content: space-between; align-items: center;
            padding: 15px; background: #f8f9fa; border-radius: 10px; 
            border-left: 5px solid #667eea;
        }
        .status-label { font-weight: bold; color: #333; }
        .status-value { font-weight: bold; color: #667eea; }
        .button { 
            background: #28a745; color: white; border: none; padding: 15px 25px; 
            border-radius: 10px; cursor: pointer; font-size: 16px; font-weight: bold; 
            margin: 10px 5px; transition: all 0.3s; min-width: 150px;
        }
        .button:hover { background: #218838; transform: translateY(-2px); }
        .button:disabled { background: #6c757d; cursor: not-allowed; transform: none; }
        .button.extended { background: #ffc107; color: #333; }
        .button.extended:hover { background: #e0a800; }
        .button.danger { background: #dc3545; }
        .button.danger:hover { background: #c82333; }
        .alert { 
            padding: 15px; margin: 15px 0; border-radius: 10px; font-weight: bold; 
        }
        .alert.success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .alert.error { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
        .pump-controls { 
            display: flex; flex-wrap: wrap; justify-content: center; align-items: center; gap: 10px;
        }
        @media (max-width: 600px) {
            .status-grid { grid-template-columns: 1fr; }
            .pump-controls { flex-direction: column; }
            .button { width: 100%; margin: 5px 0; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <button class="logout-btn" onclick="logout()">Logout</button>
            <h1>🌱 ESP32-C3 Water System</h1>
        </div>
        
        <div class="card">
            <h2>📊 System Status</h2>
            <div class="status-grid">
                <div class="status-item">
                    <span class="status-label">Water Level:</span>
                    <span class="status-value" id="waterStatus">Loading...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">Pump Status:</span>
                    <span class="status-value" id="pumpStatus">Loading...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">WiFi Status:</span>
                    <span class="status-value" id="wifiStatus">Loading...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">RTC Time:</span>
                    <span class="status-value" id="rtcTime">Loading...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">Free Memory:</span>
                    <span class="status-value" id="freeHeap">Loading...</span>
                </div>
                <div class="status-item">
                    <span class="status-label">Uptime:</span>
                    <span class="status-value" id="uptime">Loading...</span>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>⚡ Pump Control</h2>
            <div class="pump-controls">
                <button id="normalBtn" class="button" onclick="triggerNormalPump()">
                    💧 Normal Cycle
                </button>
                <button id="extendedBtn" class="button extended" onclick="triggerExtendedPump()">
                    ⏰ Extended Cycle  
                </button>
                <button id="stopBtn" class="button danger" onclick="stopPump()">
                    🛑 Stop Pump
                </button>
            </div>
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
            
            setTimeout(() => {
                if (notifications.contains(alert)) {
                    notifications.removeChild(alert);
                }
            }, 5000);
        }
        
        function triggerNormalPump() {
            const btn = document.getElementById('normalBtn');
            btn.disabled = true;
            btn.textContent = 'Starting...';
            
            fetch('/api/pump/normal', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        showNotification(`Pump started for ${data.duration}s (${data.volume_ml}ml)`, 'success');
                    } else {
                        showNotification('Failed to start pump', 'error');
                    }
                })
                .catch(() => showNotification('Connection error', 'error'))
                .finally(() => {
                    btn.disabled = false;
                    btn.textContent = '💧 Normal Cycle';
                    updateStatus();
                });
        }
        
        function triggerExtendedPump() {
            const btn = document.getElementById('extendedBtn');
            btn.disabled = true;
            btn.textContent = 'Starting...';
            
            fetch('/api/pump/extended', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        showNotification(`Extended pump started for ${data.duration}s`, 'success');
                    } else {
                        showNotification('Failed to start pump', 'error');
                    }
                })
                .catch(() => showNotification('Connection error', 'error'))
                .finally(() => {
                    btn.disabled = false;
                    btn.textContent = '⏰ Extended Cycle';
                    updateStatus();
                });
        }
        
        function stopPump() {
            const btn = document.getElementById('stopBtn');
            btn.disabled = true;
            btn.textContent = 'Stopping...';
            
            fetch('/api/pump/stop', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        showNotification('Pump stopped successfully', 'success');
                    } else {
                        showNotification('Failed to stop pump', 'error');
                    }
                })
                .catch(() => showNotification('Connection error', 'error'))
                .finally(() => {
                    btn.disabled = false;
                    btn.textContent = '🛑 Stop Pump';
                    updateStatus();
                });
        }
        
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('waterStatus').textContent = data.water_status;
                    document.getElementById('pumpStatus').textContent = data.pump_running ? 
                        `Active (${data.pump_remaining}s remaining)` : 'Inactive';
                    document.getElementById('wifiStatus').textContent = data.wifi_status;
                    
                    // Enhanced RTC display with type information
                    const rtcText = data.rtc_time || 'Error';
                    const rtcInfo = data.rtc_info || '';
                    document.getElementById('rtcTime').innerHTML = `${rtcText}<br><small style="color: #666; font-size: 0.8em;">${rtcInfo}</small>`;
                    
                    document.getElementById('freeHeap').textContent = (data.free_heap / 1024).toFixed(1) + ' KB';
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    
                    // Update button states based on pump status
                    const isRunning = data.pump_running;
                    document.getElementById('normalBtn').disabled = isRunning;
                    document.getElementById('extendedBtn').disabled = isRunning;
                    document.getElementById('stopBtn').disabled = !isRunning;
                })
                .catch(error => {
                    console.error('Status update failed:', error);
                });
        }
        
        function formatUptime(milliseconds) {
            const seconds = Math.floor(milliseconds / 1000);
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            return `${hours}h ${minutes}m`;
        }
        
        function logout() {
            fetch('/api/logout', { method: 'POST' })
                .then(() => {
                    window.location.href = '/login';
                });
        }
        
        // Auto-refresh every 2 seconds
        setInterval(updateStatus, 2000);
        updateStatus(); // Initial load
    </script>
</body>
</html>
)rawliteral";

String getLoginHTML() {
    return String(LOGIN_HTML);
}

String getDashboardHTML() {
    return String(DASHBOARD_HTML);
}