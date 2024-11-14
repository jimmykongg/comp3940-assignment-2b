//
// Created by Jimmy Kong on 2024-11-13.
//
#pragma once

#include <string>

#include "serverSocket.hpp"

using namespace std;

class Thread {
private:
    void *state;
    Thread *childThread;

public:
    Thread(Thread *childThread);
    virtual ~Thread();
    virtual void run() = 0;
    virtual void start();
};

class UploadServerThread : public Thread {
private:
    string name;
    Socket *connectionSocket;
    int clientSocket = 0;

public:
    UploadServerThread(string name, Socket *socket);
    ~UploadServerThread();
    void run() override;
};
