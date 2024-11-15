#pragma once
#include "serverSocket.hpp"

using namespace std;

class HttpServlet {
public:
    virtual ~HttpServlet() = default;

    /**
     * Handle get requests
     * @param socket connection socket
     * @param isCorrectPath boolean that checks whether clients use the correct path to access the server
     */
    virtual void doGet(Socket* socket, bool isCorrectPath) = 0;

    virtual void doPost(Socket* socket, bool isCorrectPath) = 0;
};

class UploadServlet : public HttpServlet {
private:
    const static string CRLF;

public:
    void doGet(Socket* socket, bool isCorrectPath) override;

    void doPost(Socket* socket, bool isCorrectPath) override;
};

