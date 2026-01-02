#ifndef MESSAGE_PROTOCOL_HPP
#define MESSAGE_PROTOCOL_HPP

#include <string>
#include <map>

enum MessageType {
    MSG_SIGNUP,
    MSG_LOGIN,
    MSG_LOGOUT,
    MSG_QUEUE,
    MSG_CANCEL_QUEUE,
    MSG_MOVE,
    MSG_GAME_START,
    MSG_GAME_END,
    MSG_GET_LEADERBOARD,
    MSG_ADD_FRIEND,
    MSG_GET_FRIENDS,
    MSG_ERROR,
    MSG_SUCCESS
};

struct Message {
    MessageType type;
    std::map<std::string, std::string> data;
    
    std::string ToJSON() const;
    static Message FromJSON(const std::string& json);
};

#endif
