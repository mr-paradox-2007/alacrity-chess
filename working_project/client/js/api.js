const API_BASE = 'http://localhost:8080';

class api_client {
    static async request(method, endpoint, data = null) {
        const options = {
            method: method,
            headers: {
                'Content-Type': 'application/json'
            }
        };
        
        const token = auth.get_token();
        if (token) {
            options.headers['Authorization'] = 'Bearer ' + token;
        }
        
        if (data) {
            options.body = JSON.stringify(data);
        }
        
        try {
            const response = await fetch(API_BASE + endpoint, options);
            if (!response.ok) {
                const errorText = await response.text();
                try {
                    return JSON.parse(errorText);
                } catch {
                    return { error: errorText || 'Request failed' };
                }
            }
            const text = await response.text();
            if (!text) {
                return {};
            }
            try {
                return JSON.parse(text);
            } catch {
                return { error: 'Invalid JSON response' };
            }
        } catch (error) {
            return { error: error.message || 'Network error' };
        }
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
    
    static async search_users(query) {
        return this.request('GET', '/users/search?q=' + encodeURIComponent(query));
    }
    
    static async queue_for_match() {
        return this.request('POST', '/match/queue');
    }
    
    static async find_match() {
        return this.request('POST', '/match/find');
    }
    
    static async get_match_history() {
        return this.request('GET', '/match/history');
    }
    
    static async record_match_result(opponent_id, winner_id) {
        return this.request('POST', '/match/record', { opponent_id, winner_id });
    }
    
    static async send_friend_request(friend_id) {
        return this.request('POST', '/friends/request', { friend_id });
    }
    
    static async accept_friend_request(friend_id) {
        return this.request('POST', '/friends/accept', { friend_id });
    }
    
    static async get_friend_recommendations() {
        return this.request('GET', '/friends/recommendations');
    }
    
    static async get_pending_friend_requests() {
        return this.request('GET', '/friends/pending');
    }
    
    static async get_friends() {
        return this.request('GET', '/friends/list');
    }
    
    static async reject_friend_request(friend_id) {
        return this.request('POST', '/friends/reject', { friend_id });
    }
}
