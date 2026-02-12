#pragma once

#include <stdexcept>

namespace dto
{

    class NotRequiredFieldException : public std::runtime_error
    {
    public:
        NotRequiredFieldException(const std::string &message) : std::runtime_error(message) {}
    };

}