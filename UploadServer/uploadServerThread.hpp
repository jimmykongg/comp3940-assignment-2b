//
// Created by Jimmy Kong on 2024-11-13.
//
#pragma once

#include <string>
#include <sys/_pthread/_pthread_t.h>

using namespace std;

class Thread {
private:
    pthread_t* tid;
    Thread *childThread;

public:
    Thread(Thread *childThread);
    virtual ~Thread();
    virtual void run() = 0;
    virtual void start();
};

class UploadServerThread : public Thread {
private:
    string name = "UploadServerThread"; /* TODO: Fix the bug of outputting garbage characters code */
    int connectionSocket;

public:
    UploadServerThread(int socket);
    ~UploadServerThread();
    void run() override;
};
