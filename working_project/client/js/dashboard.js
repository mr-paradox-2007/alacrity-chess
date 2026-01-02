class ui {
    static show_notification(message, type = 'success') {
        const notif = document.createElement('div');
        notif.className = `notification ${type}`;
        notif.innerHTML = `<strong>${type.toUpperCase()}:</strong> ${message}`;
        document.body.appendChild(notif);
        setTimeout(() => notif.remove(), 4000);
    }

    static render_login_page() {
        return `
            <div class="dashboard" style="display: flex; align-items: center; justify-content: center; min-height: 100vh;">
                <div class="card" style="max-width: 400px; width: 100%;">
                    <div class="card-header">♟️ Chess Platform</div>
                    <p style="color: var(--text-muted); margin-bottom: 1.5rem; text-align: center;">
                        Master Your Game • Compete Globally
                    </p>
                    <form id="login-form">
                        <input type="text" id="username" placeholder="Username" required>
                        <input type="password" id="password" placeholder="Password" required>
                        <button type="submit" class="btn-primary" style="width: 100%;">Sign In</button>
                        <button type="button" onclick="ui.show_register_page()" class="btn-secondary" style="width: 100%;">Create Account</button>
                    </form>
                </div>
            </div>
        `;
    }

    static render_register_page() {
        return `
            <div class="dashboard" style="display: flex; align-items: center; justify-content: center; min-height: 100vh;">
                <div class="card" style="max-width: 400px; width: 100%;">
                    <div class="card-header">Create Your Account</div>
                    <form id="register-form">
                        <input type="text" id="reg-username" placeholder="Username (3-20 chars)" required minlength="3" maxlength="20">
                        <input type="password" id="reg-password" placeholder="Password (min 8 chars)" required minlength="8">
                        <input type="password" id="reg-confirm" placeholder="Confirm Password" required>
                        <button type="submit" class="btn-primary" style="width: 100%;">Register</button>
                        <button type="button" onclick="ui.show_login_page()" class="btn-secondary" style="width: 100%;">Back to Login</button>
                    </form>
                </div>
            </div>
        `;
    }

    static render_dashboard() {
        return `
            <div class="navbar">
                <div class="navbar-content">
                    <h1>♟️ Chess Platform</h1>
                    <div>
                        <button onclick="ui.show_profile()" class="btn-primary">Profile</button>
                        <button onclick="ui.logout()" class="btn-danger">Logout</button>
                    </div>
                </div>
            </div>
            <div class="dashboard">
                <div style="margin: 2rem 0;">
                    <h2 style="color: var(--primary); margin-bottom: 1rem;">Welcome Back!</h2>
                </div>
                
                <div class="stats-grid" id="stats-container">
                    <div class="stat-box">
                        <div class="stat-label">Elo Rating</div>
                        <div class="stat-value" id="user-elo">1600</div>
                    </div>
                    <div class="stat-box">
                        <div class="stat-label">Matches Played</div>
                        <div class="stat-value" id="user-matches">0</div>
                    </div>
                    <div class="stat-box">
                        <div class="stat-label">Wins</div>
                        <div class="stat-value" id="user-wins">0</div>
                    </div>
                    <div class="stat-box">
                        <div class="stat-label">Win Rate</div>
                        <div class="stat-value" id="user-winrate">0%</div>
                    </div>
                </div>

                <div class="dashboard-grid">
                    <div class="action-card" onclick="ui.queue_for_match()">
                        <h3>⚔️ Play Match</h3>
                        <p>Find an opponent and play</p>
                    </div>
                    <div class="action-card" onclick="ui.view_leaderboard()">
                        <h3>🏆 Leaderboard</h3>
                        <p>See top players</p>
                    </div>
                    <div class="action-card" onclick="ui.view_friends()">
                        <h3>👥 Friends</h3>
                        <p>Manage your friends</p>
                    </div>
                    <div class="action-card" onclick="ui.view_match_history()">
                        <h3>📊 History</h3>
                        <p>View your matches</p>
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
                this.show_notification('Login failed - check credentials', 'error');
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
                this.show_notification('Passwords do not match', 'error');
                return;
            }
            
            const result = await auth.register(username, password);
            if (result.status === 'ok') {
                this.show_notification('Account created! You can now login', 'success');
                this.show_login_page();
            } else {
                this.show_notification('Registration failed', 'error');
            }
        });
    }

    static show_dashboard() {
        document.getElementById('app').innerHTML = this.render_dashboard();
        this.load_user_stats();
    }

    static async load_user_stats() {
        try {
            const user = await api_client.get_user();
            console.log('User data received:', user);
            if (user && !user.error && user.user_id) {
                if (document.getElementById('user-elo')) {
                    document.getElementById('user-elo').textContent = user.elo || user.elo_rating || '1600';
                }
                if (document.getElementById('user-matches')) {
                    document.getElementById('user-matches').textContent = user.matches || user.total_matches || '0';
                }
                if (document.getElementById('user-wins')) {
                    document.getElementById('user-wins').textContent = user.wins || '0';
                }
                
                const totalMatches = user.matches || user.total_matches || 0;
                const wins = user.wins || 0;
                const winrate = totalMatches > 0 ? Math.round((wins / totalMatches) * 100) : 0;
                if (document.getElementById('user-winrate')) {
                    document.getElementById('user-winrate').textContent = winrate + '%';
                }
            } else if (user && user.error) {
                console.error('Error loading user stats:', user.error);
            }
        } catch (error) {
            console.error('Failed to load user stats:', error);
        }
    }

    static show_profile() {
        this.load_user_stats();
        this.show_notification('Profile updated', 'success');
    }

    static async queue_for_match() {
        try {
            this.show_notification('Queuing for matchmaking...', 'success');
            const result = await api_client.queue_for_match();
            console.log('Queue result:', result);
            
            if (result.status === 'ok' || result.message) {
                this.show_notification('Queued for matchmaking...', 'success');
                
                for (let i = 0; i < 5; i++) {
                    await new Promise(r => setTimeout(r, 1000));
                    const match = await api_client.find_match();
                    console.log('Find match result:', match);
                    
                    if (match.status === 'ok' && match.opponent_id) {
                        this.start_game(match.opponent_id, match.opponent_username || 'Opponent');
                        return;
                    }
                }
                
                this.show_notification('No opponent found. Try again later.', 'warning');
                this.show_dashboard();
            } else {
                this.show_notification('Failed to queue: ' + (result.error || 'Unknown error'), 'error');
            }
        } catch (error) {
            console.error('Queue error:', error);
            this.show_notification('Error queuing for match: ' + error.message, 'error');
        }
    }

    static start_game(opponent_id, opponent_name) {
        const user_id = parseInt(auth.get_token().split('_')[0]);
        
        document.getElementById('app').innerHTML = `
            <div class="navbar">
                <div class="navbar-content">
                    <h1>♟️ Match in Progress</h1>
                    <button onclick="ui.show_dashboard()" class="btn-secondary">Back</button>
                </div>
            </div>
            <div class="dashboard">
                <div class="match-container card">
                    <h2 style="margin-bottom: 1.5rem;">Play Your Match</h2>
                    
                    <div class="match-players">
                        <div class="player-card">
                            <div class="player-name" id="player1-name">You</div>
                            <div class="player-elo" id="player1-elo">1600</div>
                        </div>
                        <div class="vs-label">VS</div>
                        <div class="player-card">
                            <div class="player-name">${opponent_name}</div>
                            <div class="player-elo">${1600}</div>
                        </div>
                    </div>

                    <div class="chess-board" id="chess-board">
                        ${Array.from({length: 64}).map((_, i) => 
                            `<div class="chess-square" data-square="${i}">${
                                i === 0 ? '♜' : i === 7 ? '♜' : i === 8 ? '♟' : i === 56 ? '♖' : i === 63 ? '♖' : i === 57 ? '♙' : ''
                            }</div>`
                        ).join('')}
                    </div>

                    <div style="text-align: center; margin-top: 2rem;">
                        <p style="margin-bottom: 1rem; color: var(--text-muted);">Record the match result:</p>
                        <button onclick="ui.record_win(${opponent_id})" class="btn-success" style="margin: 0 0.5rem;">Win</button>
                        <button onclick="ui.record_loss(${opponent_id})" class="btn-danger" style="margin: 0 0.5rem;">Loss</button>
                        <button onclick="ui.record_draw(${opponent_id})" class="btn-secondary" style="margin: 0 0.5rem;">Draw</button>
                    </div>
                </div>
            </div>
        `;
    }

    static async record_win(opponent_id) {
        const token = auth.get_token();
        const user_id = parseInt(token.split('_')[0]);
        const result = await api_client.record_match_result(opponent_id, user_id);
        this.show_notification('✓ Win recorded! +16 Elo', 'success');
        this.show_dashboard();
    }

    static async record_loss(opponent_id) {
        const token = auth.get_token();
        const user_id = parseInt(token.split('_')[0]);
        await api_client.record_match_result(opponent_id, opponent_id);
        this.show_notification('Loss recorded. -16 Elo', 'warning');
        this.show_dashboard();
    }

    static async record_draw(opponent_id) {
        const token = auth.get_token();
        const user_id = parseInt(token.split('_')[0]);
        await api_client.record_match_result(opponent_id, 0);  // 0 means draw
        this.show_notification('Draw recorded. No Elo change', 'warning');
        this.show_dashboard();
    }

    static async view_leaderboard() {
        try {
            const result = await api_client.get_leaderboard();
            console.log('Leaderboard result:', result);
            
            let html = `
                <div class="navbar">
                    <div class="navbar-content">
                        <h1>🏆 Leaderboard</h1>
                        <button onclick="ui.show_dashboard()" class="btn-secondary">Back</button>
                    </div>
                </div>
                <div class="dashboard">
                    <div class="card">
                        <div class="card-header">Top Players</div>
            `;
            
            if (result.leaderboard && Array.isArray(result.leaderboard) && result.leaderboard.length > 0) {
                html += `<table>
                    <thead>
                        <tr>
                            <th>Rank</th>
                            <th>Player</th>
                            <th>Elo Rating</th>
                            <th>Matches</th>
                            <th>Wins</th>
                            <th>Losses</th>
                        </tr>
                    </thead>
                    <tbody>`;
                
                result.leaderboard.forEach((player, idx) => {
                    const medal = idx === 0 ? '🥇' : idx === 1 ? '🥈' : idx === 2 ? '🥉' : '';
                    const rank = player.rank || (idx + 1);
                    html += `
                        <tr>
                            <td><strong>${medal} #${rank}</strong></td>
                            <td>${player.username || 'Unknown'}</td>
                            <td style="color: var(--primary); font-weight: bold;">${player.elo || player.elo_rating || 1600}</td>
                            <td>${player.matches || player.total_matches || 0}</td>
                            <td><span class="badge badge-success">${player.wins || 0}</span></td>
                            <td><span class="badge badge-danger">${player.losses || 0}</span></td>
                        </tr>
                    `;
                });
                
                html += `</tbody></table>`;
            } else {
                html += `<p style="color: var(--text-muted); text-align: center; padding: 2rem;">No players yet. Be the first!</p>`;
            }
            
            html += `</div></div>`;
            document.getElementById('app').innerHTML = html;
        } catch (error) {
            console.error('Leaderboard error:', error);
            this.show_notification('Failed to load leaderboard: ' + error.message, 'error');
        }
    }

    static async view_friends() {
        document.getElementById('app').innerHTML = `
            <div class="navbar">
                <div class="navbar-content">
                    <h1>👥 Friends & Search</h1>
                    <button onclick="ui.show_dashboard()" class="btn-secondary">Back</button>
                </div>
            </div>
            <div class="dashboard">
                <div class="card" style="margin-bottom: 2rem;">
                    <div class="card-header">Find & Add Friends</div>
                    <input type="text" id="search-box" placeholder="Search players..." class="search-box">
                    <div id="search-results"></div>
                </div>
            </div>
        `;
        
        document.getElementById('search-box').addEventListener('input', async (e) => {
            if (e.target.value.length > 1) {
                const results = await api_client.search_users(e.target.value);
                let html = '';
                if (results.users && Array.isArray(results.users)) {
                    results.users.forEach(user => {
                        html += `
                            <div class="user-item">
                                <div>
                                    <strong>${user.username}</strong><br>
                                    <small style="color: var(--text-muted);">Elo: ${user.elo}</small>
                                </div>
                                <button onclick="ui.add_friend(${user.user_id})" class="btn-primary">Add Friend</button>
                            </div>
                        `;
                    });
                }
                document.getElementById('search-results').innerHTML = html || '<p style="color: var(--text-muted);">No users found</p>';
            }
        });
    }

    static async add_friend(friend_id) {
        const result = await api_client.send_friend_request(friend_id);
        if (result.status === 'ok') {
            this.show_notification('✓ Friend request sent!', 'success');
        }
    }

    static async view_match_history() {
        try {
            const result = await api_client.get_match_history();
            console.log('Match history result:', result);
            
            let html = `
                <div class="navbar">
                    <div class="navbar-content">
                        <h1>📊 Match History</h1>
                        <button onclick="ui.show_dashboard()" class="btn-secondary">Back</button>
                    </div>
                </div>
                <div class="dashboard">
                    <div class="card">
                        <div class="card-header">Your Matches</div>
            `;
            
            if (result.matches && Array.isArray(result.matches) && result.matches.length > 0) {
                html += `<table>
                    <thead>
                        <tr>
                            <th>Match ID</th>
                            <th>Opponent</th>
                            <th>Result</th>
                            <th>Elo Change</th>
                        </tr>
                    </thead>
                    <tbody>`;
                
                result.matches.forEach(match => {
                    const token = auth.get_token();
                    const user_id = parseInt(token.split('_')[0]);
                    const is_winner = match.winner_id === user_id;
                    const is_draw = match.winner_id === 0;
                    const result_text = is_winner ? '✓ WIN' : is_draw ? 'DRAW' : '✗ LOSS';
                    const result_class = is_winner ? 'badge-success' : is_draw ? 'badge-warning' : 'badge-danger';
                    const opponent_name = match.opponent_username || `Player ${match.opponent_id || match.player2_id}`;
                    
                    html += `
                        <tr>
                            <td>#${match.match_id}</td>
                            <td>${opponent_name}</td>
                            <td><span class="badge ${result_class}">${result_text}</span></td>
                            <td style="color: ${match.elo_change > 0 ? '#22c55e' : match.elo_change < 0 ? '#ef4444' : '#666'};">${match.elo_change > 0 ? '+' : ''}${match.elo_change}</td>
                        </tr>
                    `;
                });
                
                html += `</tbody></table>`;
            } else {
                html += `<p style="color: var(--text-muted); text-align: center; padding: 2rem;">No matches yet. Go play!</p>`;
            }
            
            html += `</div></div>`;
            document.getElementById('app').innerHTML = html;
        } catch (error) {
            console.error('Match history error:', error);
            this.show_notification('Failed to load match history: ' + error.message, 'error');
        }
    }

    static async logout() {
        await auth.logout();
        this.show_notification('Logged out successfully', 'success');
        this.show_login_page();
    }
}
