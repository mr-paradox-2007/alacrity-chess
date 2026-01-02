const SERVER_URL = 'http://localhost:8080';

function toggleForms() {
    document.getElementById('login-form').classList.toggle('hidden');
    document.getElementById('signup-form').classList.toggle('hidden');
}

function handleLogin(event) {
    event.preventDefault();
    
    const username = document.getElementById('login-username').value;
    const password = document.getElementById('login-password').value;
    
    fetch(`${SERVER_URL}/api/login`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            username: username,
            password: password
        })
    })
    .then(response => response.json())
    .then(data => {
        const msg = document.getElementById('login-message');
        if (data.success) {
            msg.className = 'message success';
            msg.textContent = 'Login successful! Redirecting...';
            localStorage.setItem('username', username);
            localStorage.setItem('token', data.token);
            setTimeout(() => {
                window.location.href = 'dashboard.html';
            }, 1500);
        } else {
            msg.className = 'message error';
            msg.textContent = data.error || 'Login failed';
        }
    })
    .catch(err => {
        const msg = document.getElementById('login-message');
        msg.className = 'message error';
        msg.textContent = 'Connection error: ' + err.message;
    });
}

function handleSignup(event) {
    event.preventDefault();
    
    const username = document.getElementById('signup-username').value;
    const password = document.getElementById('signup-password').value;
    const confirm = document.getElementById('signup-confirm').value;
    
    if (password !== confirm) {
        const msg = document.getElementById('signup-message');
        msg.className = 'message error';
        msg.textContent = 'Passwords do not match';
        return;
    }
    
    fetch(`${SERVER_URL}/api/signup`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            username: username,
            password: password
        })
    })
    .then(response => response.json())
    .then(data => {
        const msg = document.getElementById('signup-message');
        if (data.success) {
            msg.className = 'message success';
            msg.textContent = 'Account created! Switching to login...';
            setTimeout(() => {
                toggleForms();
                document.getElementById('login-username').value = username;
            }, 1500);
        } else {
            msg.className = 'message error';
            msg.textContent = data.error || 'Signup failed';
        }
    })
    .catch(err => {
        const msg = document.getElementById('signup-message');
        msg.className = 'message error';
        msg.textContent = 'Connection error: ' + err.message;
    });
}
