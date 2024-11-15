#include "serverSocket.hpp"

#include <iostream>
#include <sys/socket.h>
#include <resolv.h>
#include <sstream>
#include <unistd.h>

using namespace std;

ServerSocket::ServerSocket(int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (bind(sock, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
        perror("binding stream socket");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 5) < 0) {
        perror("listening stream socket");
        close(sock);
        exit(EXIT_FAILURE);
    }
    cout << "Server listening on port " << port << endl;
}

Socket* ServerSocket::Accept() const {
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int connectionSocket = accept(sock, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressLength);
    return new Socket(connectionSocket);
}

ServerSocket::~ServerSocket() = default;

Socket::Socket(int sockfd): sockfd(sockfd) {}

Socket::~Socket() = default;

char *Socket::read() const {
    auto buffer = new char[1024];
    recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    return buffer;
}

void Socket::write(istringstream &iss) const {
    string message = iss.str();
    send(sockfd, message.c_str(), message.size(), 0);
}

