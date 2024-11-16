#include "ClientSocket.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

ClientSocket::ClientSocket(const std::string& host, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }
}

void ClientSocket::connectToServer() {
    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected to server" << std::endl;
}

void ClientSocket::sendRequest(const std::string& request) {
    send(sock, request.c_str(), request.length(), 0);
}

std::string ClientSocket::receiveResponse() {
    char buffer[4096];
    int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        perror("Receiving response failed");
        return "";
    }
    buffer[bytesRead] = '\0';
    return std::string(buffer);
}

ClientSocket::~ClientSocket() {
    close(sock);
}
