#include "uploadServerThread.hpp"

#include <iostream>
#include <ostream>
#include <sstream>
#include <pthread.h>
#include <sys/_pthread/_pthread_t.h>

using namespace std;

void* startRunning(void* arg) {
    if (arg == nullptr) {
        cerr << "Error: startRunning received a null pointer" << endl;
        return nullptr;
    }
    cout << "startRunning received thread: " << arg << endl;

    auto* thread = (Thread*) arg;
    thread->run();
    return nullptr;
}

Thread::Thread(): tid(new pthread_t()) {}

Thread::~Thread() { delete tid; }

void Thread::start() {
    pthread_create(tid, nullptr, startRunning, this);
}

UploadServerThread::UploadServerThread(Socket* socket): socket(socket), name("UploadServerThread") {}

UploadServerThread::~UploadServerThread() {
    delete socket;
    delete httpServlet;
};

void UploadServerThread::run() {
    try {
        cout << name << ": started handling client request" << endl;

        stringstream requestStream = socket->read();
        string requestLine;
        getline(requestStream, requestLine);

        // Parse HTTP method from request line
        istringstream lineStream(requestLine);
        string httpMethod, path;
        lineStream >> httpMethod >> path;
        cout << "Request HTTP method: " << httpMethod << endl;
        cout << "Request path: " << path << endl;

        if (httpMethod == "GET" && path == "/upload") {
            httpServlet->doGet(socket, true);
        } else {
            /* TODO: Call doPost */
        }

        socket->closeConnection();
    } catch (...) {
        cerr << "Error handling client connection" << endl;
        socket->closeConnection();
    }
}