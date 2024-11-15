cmake_minimum_required(VERSION 3.29)
project(comp3940_assignment_2b)

set(CMAKE_CXX_STANDARD 20)

add_executable(comp3940_assignment_2b main.cpp
        UploadServer/serverSocket.hpp
        UploadServer/serverSocket.cpp
        UploadServer/uploadServer.cpp
        UploadServer/uploadServerThread.hpp
        UploadServer/uploadServerThread.cpp
        UploadServer/uploadServlet.hpp
        UploadServer/uploadServlet.cpp)
