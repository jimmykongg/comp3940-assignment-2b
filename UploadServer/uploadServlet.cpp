#include "uploadServlet.hpp"

#include <iostream>
#include <sstream>
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

void UploadServlet::doPost(Socket* socket, bool isCorrectPath) {}