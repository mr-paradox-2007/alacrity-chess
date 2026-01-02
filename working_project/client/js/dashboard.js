class ui {
    static render_login_page() {
        return `
            <div class="login-container">
                <h1>Chess Platform</h1>
                <form id="login-form">
                    <input type="text" id="username" placeholder="Username" required>
                    <input type="password" id="password" placeholder="Password" required>
                    <button type="submit">Login</button>
                    <button type="button" onclick="ui.show_register_page()">Register</button>
                </form>
            </div>
        `;
    }
    
    static render_register_page() {
        return `
            <div class="register-container">
                <h1>Create Account</h1>
                <form id="register-form">
                    <input type="text" id="reg-username" placeholder="Username" required>
                    <input type="password" id="reg-password" placeholder="Password" required>
                    <input type="password" id="reg-confirm" placeholder="Confirm Password" required>
                    <button type="submit">Register</button>
                    <button type="button" onclick="ui.show_login_page()">Back to Login</button>
                </form>
            </div>
        `;
    }
    
    static render_dashboard() {
        return `
            <div class="dashboard">
                <nav class="navbar">
                    <h2>Chess Platform</h2>
                    <button onclick="auth.logout();ui.show_login_page()">Logout</button>
                </nav>
                <div class="content">
                    <div class="stats-panel">
                        <h3>Your Stats</h3>
                        <p>Elo: <span id="user-elo">1600</span></p>
                        <p>Matches: <span id="user-matches">0</span></p>
                    </div>
                    <div class="actions-panel">
                        <button onclick="ui.queue_for_match()">Queue for Match</button>
                        <button onclick="ui.view_leaderboard()">Leaderboard</button>
                        <button onclick="ui.view_friends()">Friends</button>
                    </div>
                </div>
            </div>
        `;
    }
    
    static show_login_page() {
        document.getElementById('app').innerHTML = this.render_login_page();
        document.getElementById('login-form').addEventListener('submit', async (e) => {
            e.preventDefault();
            const username = document.getElementById('username').value;
            const password = document.getElementById('password').value;
            if (await auth.login(username, password)) {
                this.show_dashboard();
            } else {
                alert('Login failed');
            }
        });
    }
    
    static show_register_page() {
        document.getElementById('app').innerHTML = this.render_register_page();
        document.getElementById('register-form').addEventListener('submit', async (e) => {
            e.preventDefault();
            const username = document.getElementById('reg-username').value;
            const password = document.getElementById('reg-password').value;
            const confirm = document.getElementById('reg-confirm').value;
            if (password !== confirm) {
                alert('Passwords do not match');
                return;
            }
            const result = await auth.register(username, password);
            if (result.status === 'ok') {
                alert('Registration successful');
                this.show_login_page();
            } else {
                alert('Registration failed');
            }
        });
    }
    
    static show_dashboard() {
        document.getElementById('app').innerHTML = this.render_dashboard();
        this.load_user_stats();
    }
    
    static async load_user_stats() {
        const user = await api_client.get_user();
        if (user.user_id) {
            document.getElementById('user-elo').textContent = user.elo;
            document.getElementById('user-matches').textContent = user.matches || '0';
        }
    }
    
    static queue_for_match() {
        alert('Queuing for match...');
    }
    
    static view_leaderboard() {
        alert('Leaderboard not implemented yet');
    }
    
    static view_friends() {
        alert('Friends not implemented yet');
    }
}
