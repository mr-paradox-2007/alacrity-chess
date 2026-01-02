const SERVER_HOST = 'localhost';
const SERVER_PORT = 8080;

let connectionStatus = 'disconnected';

window.addEventListener('DOMContentLoaded', () => {
    setupFormHandlers();
    updateConnectionStatus('Ready', 'connected');
});

function setupFormHandlers() {
    document.getElementById('login-form').addEventListener('submit', async (e) => {
        e.preventDefault();
        const username = document.getElementById('login-username').value;
        const password = document.getElementById('login-password').value;
        
        updateStatus('login-status', 'Logging in...', 'info');
        
        try {
            const response = await sendAuthRequest('login', username, password);
            handleAuthResponse(response, 'login');
        } catch (error) {
            updateStatus('login-status', 'Connection error: ' + error.message, 'error');
        }
    });
    
    document.getElementById('register-form').addEventListener('submit', async (e) => {
        e.preventDefault();
        const username = document.getElementById('register-username').value;
        const password = document.getElementById('register-password').value;
        
        updateStatus('register-status', 'Registering...', 'info');
        
        try {
            const response = await sendAuthRequest('register', username, password);
            handleAuthResponse(response, 'register');
        } catch (error) {
            updateStatus('register-status', 'Connection error: ' + error.message, 'error');
        }
    });
}

async function sendAuthRequest(action, username, password) {
    const message = JSON.stringify({
        action: action,
        username: username,
        password: password
    });
    
    console.log('Sending:', message);
    
    const response = await fetch(`http://${SERVER_HOST}:${SERVER_PORT}`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: message
    });
    
    if (!response.ok) {
        throw new Error(`HTTP error ${response.status}`);
    }
    
    const text = await response.text();
    console.log('Received:', text);
    
    return JSON.parse(text);
}

function handleAuthResponse(response, type) {
    const statusId = type === 'login' ? 'login-status' : 'register-status';
    
    if (response.status === 'success') {
        if (response.token) {
            updateStatus(statusId, 'Login successful! Token: ' + response.token.substring(0, 8) + '...', 'success');
            localStorage.setItem('session_token', response.token);
            
            setTimeout(() => {
                alert('Login successful! Server is ready for game client connection.');
            }, 500);
        } else {
            updateStatus(statusId, response.message, 'success');
            
            if (type === 'register') {
                setTimeout(() => {
                    switchTab('login');
                    updateStatus('login-status', 'Registration successful! Please login.', 'success');
                }, 1500);
            }
        }
    } else {
        updateStatus(statusId, response.message, 'error');
    }
}

function switchTab(tab) {
    document.querySelectorAll('.tab-content').forEach(el => {
        el.classList.remove('active');
    });
    document.querySelectorAll('.tab').forEach(el => {
        el.classList.remove('active');
    });
    
    document.getElementById(tab).classList.add('active');
    event.target.classList.add('active');
}

function updateStatus(elementId, message, type) {
    const element = document.getElementById(elementId);
    element.textContent = message;
    element.className = 'status ' + type;
}

function updateConnectionStatus(message, status) {
    const indicators = document.querySelectorAll('.indicator-dot');
    const statusTexts = document.querySelectorAll('[id$="-connection-status"]');
    
    indicators.forEach(indicator => {
        indicator.className = 'indicator-dot ' + status;
    });
    
    statusTexts.forEach(text => {
        text.textContent = message;
    });
    
    connectionStatus = status;
}