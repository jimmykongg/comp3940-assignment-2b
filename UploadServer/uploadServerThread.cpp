#include "uploadServerThread.hpp"
#include "UploadServerException.hpp"

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
            if (!httpServlet) {
                throw UploadServer::UploadServerException("HTTP servlet is null");
            }
            httpServlet->doGet(socket, true);
        } else if (httpMethod == "POST" && path == "/upload") {
            if (!httpServlet) {
                throw UploadServer::UploadServerException("HTTP servlet is null");
            }
            httpServlet->doPost(socket, true);
        } else {
            throw UploadServer::UploadServerException("Unsupported HTTP method or path: " + httpMethod + " " + path);
        }

        socket->closeConnection();
    } catch (const UploadServer::UploadServerException& e) {
        cerr << name << ": Upload server error: " << e.what() << endl;
        socket->closeConnection();
    } catch (const std::exception& e) {
        cerr << name << ": Standard error: " << e.what() << endl;
        socket->closeConnection();
    } catch (...) {
        cerr << name << ": Unknown error occurred" << endl;
        socket->closeConnection();
    }
}