#ifndef ALACRITY_CHESS_SCREEN_SPLASH_HPP
#define ALACRITY_CHESS_SCREEN_SPLASH_HPP

#include "screen.hpp"
#include <raylib.h>
#include <chrono>

namespace ac {
namespace client {

class screen_splash : public screen {
public:
    screen_splash();
    void update(network_client& net) override;
    void draw() override;
    game_state get_next_state() override;
    
private:
    std::chrono::time_point<std::chrono::system_clock> start_time_;
    game_state next_state_ = STATE_SPLASH;
};

}  // namespace client
}  // namespace ac

#endif // ALACRITY_CHESS_SCREEN_SPLASH_HPP
