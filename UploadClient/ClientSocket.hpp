#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <string>
#include <netinet/in.h>

class ClientSocket {
private:
    int sock;
    sockaddr_in serverAddress;

public:
    ClientSocket(const std::string& host, int port);
    void connectToServer();
    void sendRequest(const std::string& request);
    std::string receiveResponse();
    ~ClientSocket();
};

#endif
