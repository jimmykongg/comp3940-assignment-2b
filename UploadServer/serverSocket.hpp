#pragma once
#include <sstream>
#include <sys/socket.h>

using namespace std;

/*
 * Wrapper class for connection/client/message socket,
 * which is created by ServerSocket when it accepts TCP connections
 */
class Socket {
private:
    int sockfd;

public:
    Socket(int connectionSocket);

    ~Socket();

    int getSocket() const;

    char *read() const;

    void write(istringstream& iss) const;
};

class ServerSocket {
private:
    int sock;

public:
    ServerSocket(int port);

    ~ServerSocket();

    Socket* Accept() const;
};