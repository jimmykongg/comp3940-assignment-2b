#pragma once

#include <string>
#include <sys/_pthread/_pthread_t.h>

#include "serverSocket.hpp"
#include "uploadServlet.hpp"

using namespace std;

class Thread {
private:
    pthread_t* tid = nullptr;

public:
    Thread();
    virtual ~Thread();
    virtual void run() = 0;
    virtual void start();
};

class UploadServerThread : public Thread {
private:
    string name;
    Socket* socket;
    HttpServlet* httpServlet = new UploadServlet();

public:
    UploadServerThread(Socket* socket);

    ~UploadServerThread();

    void run() override;
};
