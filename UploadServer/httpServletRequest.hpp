//
// Created by Jimmy Kong on 2024-11-14.
//
#pragma once
#include <istream>

using namespace std;

class HttpServletRequest {
private:
    istream* inputStream;

public:
    HttpServletRequest(istream* inputStream): inputStream(inputStream) {}

    ~HttpServletRequest() = default;

    istream* getInputStream() const { return inputStream; }
};
