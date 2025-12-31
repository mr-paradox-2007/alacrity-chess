#ifndef ALACRITY_CHESS_SCREEN_LOGIN_HPP
#define ALACRITY_CHESS_SCREEN_LOGIN_HPP

#include "screen.hpp"
#include <string>
#include "input_manager.hpp" // For input handling

namespace ac {
namespace client {

class screen_login : public screen {
public:
    screen_login();
    void update(network_client& net) override;
    void draw() override;
    game_state get_next_state() override;
    
private:
    std::string username_input_;
    std::string password_input_;
    std::string status_message_;
    game_state next_state_ = STATE_LOGIN;
    bool login_button_pressed_ = false;
    bool register_button_pressed_ = false;
    int active_input_field_ = 0; // 0 for username, 1 for password
    input_manager input_; // For mouse and keyboard input
};

}  // namespace client
}  // namespace ac

#endif // ALACRITY_CHESS_SCREEN_LOGIN_HPP
