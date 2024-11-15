#include "uploadServlet.hpp"

#include <iostream>
#include <sys/socket.h>
using namespace std;

extern int PORT;
const string UploadServlet::CRLF = "\r\n";

void UploadServlet::doGet(HttpServletRequest &request, HttpServletResponse &response, Socket* socket) {
    try {
        ostream* out = response.getOutputStream();

        string port = to_string(PORT);
        string htmlContent =
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
            "</html>";

        // Calculate the content length
        size_t contentLength = htmlContent.length();

        // Set response header
        string header = "HTTP/1.1 200 OK" + CRLF +
                "Content-Type: text/html; charset=UTF-8" + CRLF +
                "Content-Length: " + to_string(contentLength) + CRLF +
                "Connection: close" + CRLF + CRLF;
        *out << header << htmlContent << CRLF << CRLF;

        send(socket->getConnection(), static_cast<ostringstream>(*out), response.length(), 0);
    } catch (exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}
