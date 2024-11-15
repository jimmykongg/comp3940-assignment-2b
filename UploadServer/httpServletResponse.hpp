//
// Created by Jimmy Kong on 2024-11-14.
//
#pragma once
#include <ostream>

using namespace std;

struct HttpServletResponse {
    HttpServletResponse(ostream* outputStream): outputStream(outputStream) {}

    ~HttpServletResponse() = default;

    ostream* getOutputStream() const { return outputStream; }
};


