#include "uploadServerThread.hpp"
#include "serverSocket.hpp"

constexpr int PORT = 8999;

/*
 * Driver function and main entry of the upload server
 */
int main() {
    ServerSocket serverSocket{PORT};

    while (true) {
        auto *thread = new UploadServerThread(serverSocket.Accept());
        thread->start();
        delete thread;
    }
}
