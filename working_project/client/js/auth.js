class auth {
    static set_token(token) {
        localStorage.setItem('auth_token', token);
    }
    
    static get_token() {
        return localStorage.getItem('auth_token') || '';
    }
    
    static clear_token() {
        localStorage.removeItem('auth_token');
    }
    
    static is_authenticated() {
        return !!this.get_token();
    }
    
    static async login(username, password) {
        try {
            const response = await api_client.login(username, password);
            if (response.token) {
                this.set_token(response.token);
                return true;
            }
            if (response.error) {
                console.error('Login error:', response.error);
            }
            return false;
        } catch (error) {
            console.error('Login exception:', error);
            return false;
        }
    }
    
    static async register(username, password) {
        return await api_client.register(username, password);
    }
    
    static async logout() {
        try {
            await api_client.logout();
        } catch (error) {
            console.error('Logout error:', error);
        } finally {
            this.clear_token();
        }
    }
}
