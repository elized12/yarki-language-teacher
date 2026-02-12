#pragma once

#include <stdexcept>

namespace services
{

    class NotExistException : public std::runtime_error
    {
    public:
        NotExistException(const std::string &message) : std::runtime_error(message)
        {
        }
    };

} // namespace services