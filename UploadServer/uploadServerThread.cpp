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

Thread::Thread(Thread *childThread): tid(new pthread_t()), childThread(childThread) {}

Thread::~Thread() { delete tid; }

void Thread::start() {
    pthread_create(tid, nullptr, startRunning, this);
    cout << "Thread created and started" << endl;
}

UploadServerThread::UploadServerThread(Socket* socket): Thread(this), socket(socket) {}

UploadServerThread::~UploadServerThread() { delete socket; };

void UploadServerThread::run() {
    try {
        cout << name << ": started handling client request" << endl;
        char buffer[1024] = {}; // zero initialization

        int bytesReceived = recv(socket->getSocket(), buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived < 0) {
            perror("recv");
            close(socket->getSocket());
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
            send(socket->getSocket(), response.c_str(), response.length(), 0);
        } else if (httpMethod == "POST") {
            cout << "HTTP Method: POST" << endl;
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPOST request received!";
            send(socket->getSocket(), response.c_str(), response.length(), 0);
        } else {
            cout << "Unknown HTTP Method: " << httpMethod << endl;
            string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nUnsupported method!";
            send(socket->getSocket(), response.c_str(), response.length(), 0);
        }

        close(socket->getSocket());
    } catch (...) {
        cerr << "Error handling client connection" << endl;
        close(socket->getSocket());
    }
}