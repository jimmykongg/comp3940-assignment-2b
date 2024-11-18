#include "uploadServlet.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

extern int PORT;
const string UploadServlet::CRLF = "\r\n";

void UploadServlet::doGet(Socket* socket, bool isCorrectPath) {
    try {
        string port = to_string(PORT);
        string htmlContent = isCorrectPath ?
            "<!DOCTYPE html>"
            "<html>"
            "<body>"
            "<h2>HTML Forms</h2>"
            "<form action=\"http://localhost:" + to_string(PORT) + "\" method=\"post\" enctype=\"multipart/form-data\">"
            "Caption: <input type=\"text\" name=\"caption\" /><br /><br />"
            "Date: <input type=\"date\" name=\"date\" /><br />"
            "<input type=\"file\" name=\"fileName\" /><br /><br />"
            "<input type=\"submit\" value=\"Submit\" />"
            "</form>"
            "</body>"
            "</html>" :
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n\r\n"
            "Not Found";

        // Calculate the content length
        size_t contentLength = htmlContent.length();

        // Set response header
        string header = "HTTP/1.1 200 OK" + CRLF +
                "Content-Type: text/html; charset=UTF-8" + CRLF +
                "Content-Length: " + to_string(contentLength) + CRLF +
                "Connection: close" + CRLF + CRLF;

        stringstream oss;
        oss << header << htmlContent << CRLF << CRLF;

        socket->write(oss);
    } catch (exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void UploadServlet::doPost(Socket* socket, bool isCorrectPath) {
    try {
        cout << "Starting POST request processing..." << endl;
        stringstream requestStream = socket->read();
        string request = requestStream.str();
        cout << "Request fully read. Processing content..." << endl;

        // Extract boundary
        size_t boundaryPos = request.find("boundary=");
        if (boundaryPos == string::npos) {
            throw runtime_error("Boundary not found");
        }

        string boundary = request.substr(boundaryPos + 9);
        boundary = boundary.substr(0, boundary.find("\r\n"));
        boundary = "--" + boundary;
        cout << "Found boundary: " << boundary << endl;

        // Find file content boundaries
        size_t contentStart = request.find("\r\n\r\n", request.find("Content-Type: multipart/form-data")) + 4;
        if (contentStart == string::npos) {
            throw runtime_error("Invalid request format");
        }

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

        // Create full path for file
        string fullPath = uploadDir + "/" + filename;
        cout << "Saving file to: " << fullPath << endl;

        // Extract and save file content
        string fileContent = request.substr(fileStart, fileEnd - fileStart);
        ofstream outFile(fullPath, ios::binary);
        if (!outFile) {
            throw runtime_error("Failed to create output file: " + fullPath);
        }

        outFile.write(fileContent.c_str(), fileContent.size());
        outFile.close();
        cout << "File saved successfully" << endl;

        // Prepare success message
        string successMsg = "File '" + filename + "' uploaded successfully to " + fullPath;

        // Send success response
        stringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: text/plain\r\n"
                << "Content-Length: " << successMsg.length() << "\r\n"
                << "Connection: close\r\n\r\n"
                << successMsg;

        cout << "Sending response to client..." << endl;
        socket->write(response);
        cout << "Response sent" << endl;

    } catch (const exception& e) {
        cerr << "Error in doPost: " << e.what() << endl;
        stringstream response;
        string errorMsg = string("Upload failed: ") + e.what();
        response << "HTTP/1.1 500 Internal Server Error\r\n"
                << "Content-Type: text/plain\r\n"
                << "Content-Length: " << errorMsg.length() << "\r\n"
                << "Connection: close\r\n\r\n"
                << errorMsg;
        socket->write(response);
    }
}
