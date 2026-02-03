#pragma once

#include <stdexcept>

namespace services::auth
{

class UserAlreadyExistException : public std::runtime_error
{
  public:
    UserAlreadyExistException(const std::string& message) : std::runtime_error(message)
    {
    }
};

} // namespace services