#pragma once

class Socket {
private:
    int connectionSocket;

public:
    Socket(int connectionSocket);
    char* getRequest() const;
    void sendResponse(char* res) const;
    ~Socket();
};

class ServerSocket {
private:
    int sock;

public:
    ServerSocket(int port);
    Socket* Accept() const;
    ~ServerSocket();
};