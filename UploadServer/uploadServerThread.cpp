#include "uploadServerThread.hpp"

#include <iostream>
#include <ostream>
#include <sstream>
#include <pthread.h>
#include <sys/_pthread/_pthread_t.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <stdexcept>
#include <fcntl.h>

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
        std::cout << name << ": started handling client request" << std::endl;

        // Read rq
        std::stringstream requestStream = socket->read();
        std::string request = requestStream.str();

        // Extract the rq line from the full rq
        std::string requestLine = request.substr(0, request.find("\r\n"));

        // Parse HTTP method and path
        std::istringstream lineStream(requestLine);
        std::string httpMethod, path;
        lineStream >> httpMethod >> path;
        std::cout << "Request HTTP method: " << httpMethod << std::endl;
        std::cout << "Request path: " << path << std::endl;

        if (httpMethod == "POST" && path == "/upload") {
            cout << "Starting POST request processing..." << endl;
            // Create uploads directory if it doesn't exist
            const string uploadDir = "uploads";

            // Use proper includes and system calls for directory creation
#ifdef _WIN32
            _mkdir(uploadDir.c_str());
#else
            mkdir(uploadDir.c_str(), 0755);
#endif
            // Extract boundary
            size_t boundaryPos = request.find("boundary=");
            if (boundaryPos == string::npos) {
                throw runtime_error("Boundary not found");
            }

            string boundary = request.substr(boundaryPos + 9);
            boundary = boundary.substr(0, boundary.find("\r\n"));
            boundary = "--" + boundary;
            cout << "Found boundary: " << boundary << endl;

            // Extract filename from header
            string filename;
            size_t filenamePos = request.find("filename=\"");
            if (filenamePos != string::npos) {
                size_t filenameEnd = request.find("\"", filenamePos + 10);
                filename = request.substr(filenamePos + 10, filenameEnd - (filenamePos + 10));

                size_t lastSlash = filename.find_last_of("/\\");
                if (lastSlash != string::npos) {
                    filename = filename.substr(lastSlash + 1);
                }
                cout << "Extracted filename: " << filename << endl;
            } else {
                throw runtime_error("Filename not found in request");
            }

            // Find file content
            size_t fileStart = request.find("\r\n\r\n", request.find("Content-Disposition: form-data;")) + 4;
            size_t fileEnd = request.find("\r\n" + boundary + "--", fileStart);

            if (fileStart == string::npos || fileEnd == string::npos || fileEnd <= fileStart) {
                throw runtime_error("Invalid file content boundaries");
            }

            string fullPath = uploadDir + "/" + filename;
            cout << "Saving file to: " << fullPath << endl;

            // Extract and save file content
            string fileContent = request.substr(fileStart, fileEnd - fileStart);
            std::ofstream outFile(fullPath.c_str(), std::ios::binary);
            if (!outFile) {
                throw runtime_error("Failed to create output file: " + fullPath);
            }

            outFile.write(fileContent.c_str(), fileContent.size());
            outFile.close();
            cout << "File saved successfully" << endl;

            // Send success rs
            stringstream response;
            string successMsg = "File '" + filename + "' uploaded successfully to " + fullPath;
            response << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: text/plain\r\n"
                    << "Content-Length: " << successMsg.length() << "\r\n"
                    << "Connection: close\r\n\r\n"
                    << successMsg;

            cout << "Sending response to client..." << endl;
            socket->write(response);
            cout << "Response sent" << endl;

        } else {
            stringstream response;
            string errorMsg = "404 Not Found";
            response << "HTTP/1.1 404 Not Found\r\n"
                    << "Content-Type: text/plain\r\n"
                    << "Content-Length: " << errorMsg.length() << "\r\n"
                    << "Connection: close\r\n\r\n"
                    << errorMsg;
            socket->write(response);
        }

    } catch (const exception& e) {
        cerr << "Error in request handling: " << e.what() << endl;
        stringstream response;
        string errorMsg = string("Error: ") + e.what();
        response << "HTTP/1.1 500 Internal Server Error\r\n"
                << "Content-Type: text/plain\r\n"
                << "Content-Length: " << errorMsg.length() << "\r\n"
                << "Connection: close\r\n\r\n"
                << errorMsg;
        socket->write(response);
    }

    cout << "Closing connection..." << endl;
    socket->closeConnection();
}