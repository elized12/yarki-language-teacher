#pragma once

#include <stdexcept>

namespace services
{

  class ValidationException : public std::runtime_error
  {
  public:
    ValidationException(const std::string &message) : std::runtime_error(message)
    {
    }
  };

} // namespace services