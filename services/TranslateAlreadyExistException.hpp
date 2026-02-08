#pragma once

#include <stdexcept>

namespace services
{

    class TranslateAlreadyExistException : public std::runtime_error
    {
    public:
        TranslateAlreadyExistException(const std::string &message) : std::runtime_error(message)
        {
        }
    };

} // namespace services