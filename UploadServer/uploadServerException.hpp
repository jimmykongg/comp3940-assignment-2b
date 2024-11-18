#include <stdexcept>
#include <string>


namespace UploadServer {

class UploadServerException : public std::runtime_error {
public:
    // Constructor with message only
    explicit UploadServerException(const std::string& message);

    // Constructor with message and cause
    UploadServerException(const std::string& message, const std::exception& cause);

    // Destructor
    ~UploadServerException() noexcept override = default;
};

}

