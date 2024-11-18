#pragma once
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

/*
 * Wrapper class for connection/client/message socket,
 * which is created by ServerSocket when it accepts TCP connections
 */
class Socket {
private:
    mutable int sockfd;

public:
    Socket(int sockfd);

    ~Socket();

    stringstream read() const;

    void write(stringstream& oss) const;

    void closeConnection() const;
};

class ServerSocket {
private:
    int sock;

public:
    ServerSocket(int port);

    ~ServerSocket();

    Socket* Accept() const;
};