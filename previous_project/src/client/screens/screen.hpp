#ifndef ALACRITY_CHESS_SCREEN_HPP
#define ALACRITY_CHESS_SCREEN_HPP

#include <string>
#include "../network/network_client.hpp"

namespace ac {
namespace client {

enum game_state {
    STATE_SPLASH,
    STATE_LOGIN,
    STATE_MAIN_MENU,
    STATE_PLAY_MENU,
    STATE_QUEUE,
    STATE_IN_GAME,
    STATE_COMMUNITY,
    STATE_SETTINGS,
    STATE_EXIT
};

class screen {
public:
    virtual ~screen() {}
    virtual void update(network_client& net) = 0;
    virtual void draw() = 0;
    virtual game_state get_next_state() = 0;
};

}  // namespace client
}  // namespace ac

#endif // ALACRITY_CHESS_SCREEN_HPP
