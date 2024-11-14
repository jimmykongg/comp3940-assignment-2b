#include "serverSocket.hpp"

#include <iostream>
#include <sys/socket.h>
#include <resolv.h>
#include <cstring>
#include <unistd.h>

using namespace std;

ServerSocket::ServerSocket(int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) cerr << "opening stream socket" << endl;

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (bind(sock, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
        cerr << "binding stream socket" << endl;
    }

    listen(sock, 5);
}

Socket *ServerSocket::Accept() const {
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int connectionSocket = accept(sock, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressLength);
    Socket *cs = new Socket(connectionSocket);
    return cs;
}

ServerSocket::~ServerSocket() = default;

Socket::Socket(int connectionSocket) {
    this->connectionSocket = connectionSocket;
}

char *Socket::getRequest() const {
    char *buffer = new char[1024];

    if (recv(connectionSocket, buffer, 1024, 0) < 0) {
        cerr << "reading data to connection socket" << endl;
    } else {
        cout << "Data read: " << *buffer << endl;
    }

    return buffer;
}

void Socket::sendResponse(char *res) const {
    if (send(connectionSocket, res, strlen(res), 0) < 0) {
        cerr << "sending data from connection socket" << endl;
    } else {
        cout << "Data Written: " << *res << endl;
    }
}

Socket::~Socket() = default;