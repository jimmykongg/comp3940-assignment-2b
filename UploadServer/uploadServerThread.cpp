#include "uploadServerThread.hpp"

#include <iostream>
#include <ostream>
#include <sstream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/_pthread/_pthread_t.h>

using namespace std;

void *startRunning(void *arg) {
    if (arg == nullptr) return nullptr;

    UploadServerThread *thread = static_cast<UploadServerThread *>(arg);
    thread->run();
    return nullptr;
}

Thread::Thread(Thread *childThread): childThread(childThread) {
    state = malloc(sizeof(pthread_t));
}

Thread::~Thread() { free(state); }

void Thread::start() {
    pthread_t tid;
    pthread_create(&tid, nullptr, startRunning, this);
    cout << "Thread created and started" << endl;
    memcpy(state, &tid, sizeof(pthread_t));
}

UploadServerThread::UploadServerThread(string name, Socket* socket): Thread(this),
    name(name), connectionSocket(socket) {}

UploadServerThread::~UploadServerThread() = default;

void UploadServerThread::run() {
    try {
        cout << name << ": started handling client request" << endl;
        char buffer[1024] = {}; // zero initialization

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived < 0) {
            perror("recv");
            close(clientSocket);
            return;
        }

        // Load buffer into stringstream
        stringstream requestStream(buffer);
        string requestLine;
        getline(requestStream, requestLine);

        // Parse HTTP method from request line
        string httpMethod;
        stringstream lineStream(requestLine);
        lineStream >> httpMethod;

        if (httpMethod == "GET") {
            cout << "HTTP Method: GET" << endl;
            string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET request received!";
            send(clientSocket, response.c_str(), response.length(), 0);
        } else if (httpMethod == "POST") {
            cout << "HTTP Method: POST" << endl;
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPOST request received!";
            send(clientSocket, response.c_str(), response.length(), 0);
        } else {
            cout << "Unknown HTTP Method: " << httpMethod << endl;
            string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nUnsupported method!";
            send(clientSocket, response.c_str(), response.length(), 0);
        }

        close(clientSocket);
    } catch (...) {
        cerr << "Error handling client connection" << endl;
        close(clientSocket);
    }
}