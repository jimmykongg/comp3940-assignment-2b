//
// Created by Jimmy Kong on 2024-11-14.
//
#pragma once
#include <ostream>

using namespace std;

class HttpServletResponse {
private:
    ostream* outputStream;

public:
    HttpServletResponse(ostream* outputStream): outputStream(outputStream) {}

    ~HttpServletResponse() = default;

    ostream* getOutputStream() const { return outputStream; }
};


