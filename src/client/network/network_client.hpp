#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include <string>

namespace ac {
namespace client {

class network_client {
private:
    int sock;
    std::string host;
    int port;
    bool connected;

public:
    network_client();
    ~network_client();

    bool connect_to_server(const std::string& host, int port);
    void disconnect_from_server();
    std::string send_request(const std::string& json_request);
    bool is_connected() const;
};

} // namespace client
} // namespace ac

#endif // NETWORK_CLIENT_HPP
