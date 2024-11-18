#include "UploadServerException.hpp"

namespace UploadServer {

UploadServerException::UploadServerException(const std::string& message)
    : std::runtime_error(message) {}

UploadServerException::UploadServerException(const std::string& message, const std::exception& cause)
    : std::runtime_error(message + ": " + cause.what()) {}


