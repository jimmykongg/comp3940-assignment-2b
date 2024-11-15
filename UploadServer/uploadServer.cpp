#include <iostream>

#include "uploadServerThread.hpp"
#include "serverSocket.hpp"

int PORT = 8999;

/*
 * Driver function and main entry of the upload server
 */
int main() {
    ServerSocket serverSocket{PORT};

    while (true) {
        Thread *thread = new UploadServerThread(serverSocket.Accept());
        thread->start();
        std::cout << "After thread started" << endl;
    }
}
