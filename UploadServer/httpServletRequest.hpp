#pragma once
#include <istream>

#include "serverSocket.hpp"

using namespace std;

class HttpServletRequest {
private:
    Socket* socket;

public:
    HttpServletRequest(Socket::InputStream inputStream): inputStream(inputStream) {}

    ~HttpServletRequest() = default;

    istream* getInputStream() const { return inputStream; }
};
