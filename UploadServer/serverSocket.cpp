#include "serverSocket.hpp"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <resolv.h>
#include <sstream>
#include <unistd.h>
#include <vector>

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

    if (::bind(sock, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
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
    cout << "Accepted new client connection" << endl;
    return new Socket(connectionSocket);
}

ServerSocket::~ServerSocket() = default;

Socket::Socket(int sockfd): sockfd(sockfd) {}

Socket::~Socket() = default;

stringstream Socket::read() const {
    stringstream requestStream;
    string headers;
    size_t contentLength = 0;
    bool headersComplete = false;

    cout << "Starting to read headers..." << endl;

    // Read headers
    while (!headersComplete) {
        char c;
        int bytesReceived = recv(sockfd, &c, 1, 0);
        if (bytesReceived <= 0) {
            throw runtime_error("Connection closed while reading headers");
        }
        headers += c;

        // Check if headers are complete
        if (headers.size() >= 4 && headers.substr(headers.size() - 4) == "\r\n\r\n") {
            headersComplete = true;
            cout << "Headers complete. Size: " << headers.size() << " bytes" << endl;
            cout << "Headers:\n" << headers << endl;
        }
    }

    // Write headers to rq stream
    requestStream << headers;

    // Parse content
    size_t contentLengthPos = headers.find("Content-Length: ");
    if (contentLengthPos != string::npos) {
        size_t endPos = headers.find("\r\n", contentLengthPos);
        string contentLengthStr = headers.substr(contentLengthPos + 16, endPos - (contentLengthPos + 16));
        contentLength = stoull(contentLengthStr);
        cout << "Content-Length found: " << contentLength << " bytes" << endl;
    } else {
        cout << "Warning: No Content-Length header found" << endl;
    }

    // Read the rq body based on length
    if (contentLength > 0) {
        cout << "Starting to read body..." << endl;
        const size_t bufferSize = 8192;
        vector<char> buffer(bufferSize);
        size_t totalBytesRead = 0;

        while (totalBytesRead < contentLength) {
            size_t remainingBytes = contentLength - totalBytesRead;
            size_t currentBufferSize = min(bufferSize, remainingBytes);

            int bytesReceived = recv(sockfd, buffer.data(), currentBufferSize, 0);

            if (bytesReceived <= 0) {
                cout << "Error reading body: " << strerror(errno) << endl;
                throw runtime_error("Connection closed while reading body");
            }

            requestStream.write(buffer.data(), bytesReceived);
            totalBytesRead += bytesReceived;
        }
        cout << "Finished reading body" << endl;
    }

    return requestStream;
}

void Socket::write(stringstream &oss) const {
    string message = oss.str();
    size_t totalSent = 0;

    while (totalSent < message.length()) {
        ssize_t sent = send(sockfd, message.c_str() + totalSent, message.length() - totalSent, 0);
        if (sent < 0) {
            throw runtime_error("Failed to send response: " + string(strerror(errno)));
        }
        totalSent += sent;
    }
    cout << "Successfully sent " << totalSent << " bytes" << endl;
}

void Socket::closeConnection() const {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;  // Now we can modify it even in a const method
    }
}