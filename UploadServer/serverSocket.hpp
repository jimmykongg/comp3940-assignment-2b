#pragma once
#include <sstream>
#include <unistd.h>

using namespace std;

/*
 * Wrapper class for connection/client/message socket,
 * which is created by ServerSocket when it accepts TCP connections
 */
class Socket {
private:
    int sockfd;

public:
    Socket(int sockfd);

    ~Socket();

    stringstream read() const;

    void write(stringstream& oss) const;

    void closeConnection() const { close(sockfd); }
};

class ServerSocket {
private:
    int sock;

public:
    ServerSocket(int port);

    ~ServerSocket();

    Socket* Accept() const;
};