const API_BASE = 'http://localhost:3000';

class api_client {
    static async request(method, endpoint, data = null) {
        const options = {
            method: method,
            headers: {
                'Content-Type': 'application/json',
                'Authorization': auth.get_token()
            }
        };
        
        if (data) {
            options.body = JSON.stringify(data);
        }
        
        const response = await fetch(API_BASE + endpoint, options);
        return await response.json();
    }
    
    static async register(username, password) {
        return this.request('POST', '/auth/register', { username, password });
    }
    
    static async login(username, password) {
        return this.request('POST', '/auth/login', { username, password });
    }
    
    static async logout() {
        return this.request('POST', '/auth/logout');
    }
    
    static async get_user(user_id) {
        return this.request('GET', '/user/me');
    }
    
    static async get_leaderboard() {
        return this.request('GET', '/leaderboard');
    }
}
