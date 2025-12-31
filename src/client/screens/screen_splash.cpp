#include "screen_splash.hpp"

namespace ac {
namespace client {

screen_splash::screen_splash() {
    start_time_ = std::chrono::system_clock::now();
}

void screen_splash::update(network_client& net) {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - start_time_).count();
    
    if (elapsed > 3000) {  // 3 seconds
        next_state_ = STATE_LOGIN;
    }
}

void screen_splash::draw() {
    ClearBackground(DARKBLUE);
    
    DrawText("ALACRITY CHESS", 
        GetScreenWidth() / 2 - 200, 
        GetScreenHeight() / 2 - 50,
        60, WHITE);
    
    DrawText("Loading...",
        GetScreenWidth() / 2 - 50,
        GetScreenHeight() / 2 + 100,
        20, LIGHTGRAY);
}

game_state screen_splash::get_next_state() {
    return next_state_;
}

}  // namespace client
}  // namespace ac
