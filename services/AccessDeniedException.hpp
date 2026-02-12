#pragma once

#include <stdexcept>

namespace services
{

    class AccessDeniedException : public std::runtime_error
    {
    public:
        AccessDeniedException(const std::string &message) : std::runtime_error(message) {}
    };

}