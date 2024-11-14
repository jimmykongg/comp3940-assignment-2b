#include "uploadServerThread.hpp"
#include "serverSocket.hpp"

int main() {
    ServerSocket serverSocket{8999};

    while (true) {
        auto *thread = new UploadServerThread(serverSocket.Accept());
        thread->start();
        delete thread;
    }
}
