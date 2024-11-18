#include "ClientSocket.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
void uploadFile(const std::string& host, int port, const std::string& filePath) {
    try {
        ClientSocket client(host, port);
        client.connectToServer();

        // Read file content
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return;
        }

        // Get file size
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        std::cout << "File size: " << fileSize << " bytes" << std::endl;

        // Read the file content
        std::vector<char> fileContent(fileSize);
        file.read(fileContent.data(), fileSize);
        file.close();

        // Get filename from path
        std::string filename = filePath.substr(filePath.find_last_of("/\\") + 1);
        std::cout << "Filename: " << filename << std::endl;

        // Create boundary
        std::string boundary = "------------------------" + std::to_string(time(nullptr));
        std::cout << "Using boundary: " << boundary << std::endl;

        // Create multipart body
        std::string bodyStart = "--" + boundary + "\r\n"
                               "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n"
                               "Content-Type: image/jpeg\r\n\r\n";

        std::string bodyEnd = "\r\n--" + boundary + "--\r\n";

        // Calculate total content length
        size_t contentLength = bodyStart.length() + fileSize + bodyEnd.length();
        std::cout << "Total content length: " << contentLength << " bytes" << std::endl;

        // Create rq headers
        std::string headers = "POST /upload HTTP/1.1\r\n"
                            "Host: " + host + ":" + std::to_string(port) + "\r\n"
                            "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
                            "Content-Length: " + std::to_string(contentLength) + "\r\n\r\n";

        std::cout << "Sending headers..." << std::endl;
        client.sendRequest(headers);

        std::cout << "Sending body start..." << std::endl;
        client.sendRequest(bodyStart);

        std::cout << "Sending file content..." << std::endl;
        // Send file content in chunks
        const size_t chunkSize = 8192;
        size_t totalSent = 0;
        for (size_t i = 0; i < fileSize; i += chunkSize) {
            size_t currentChunkSize = std::min(chunkSize, fileSize - i);
            std::string chunk(fileContent.data() + i, currentChunkSize);
            client.sendRequest(chunk);
            totalSent += currentChunkSize;
            std::cout << "Sent " << totalSent << "/" << fileSize << " bytes" << std::endl;
        }

        std::cout << "Sending body end..." << std::endl;
        client.sendRequest(bodyEnd);

        std::cout << "Waiting for response..." << std::endl;
        std::string response = client.receiveResponse();
        std::cout << "Server Response:\n" << response << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    const std::string host = "127.0.0.1";
    const int port = 8999;
    // Change file name here
    const std::string filePath = "/Users/stevenly/Desktop/damage.jpeg";

    std::cout << "Uploading file: " << filePath << " to server " << host << ":" << port << std::endl;
    uploadFile(host, port, filePath);

    return 0;
}


