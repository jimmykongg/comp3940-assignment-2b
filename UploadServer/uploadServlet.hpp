#pragma once
#include "httpServletRequest.hpp"
#include "httpServletResponse.hpp"
#include "serverSocket.hpp"

using namespace std;

class HttpServlet {
public:
    virtual ~HttpServlet() = default;

protected:
    virtual void doGet(HttpServletRequest &request, HttpServletResponse &response, Socket* socket) = 0;

    virtual void doPost(HttpServletRequest &request, HttpServletResponse &response, Socket* socket) = 0;
};

class UploadServlet : public HttpServlet {
private:
    const static string CRLF;

protected:
    void doGet(HttpServletRequest &request, HttpServletResponse &response, Socket* socket) override;

    void doPost(HttpServletRequest &request, HttpServletResponse &response, Socket* socket) override;
};

