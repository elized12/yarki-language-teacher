#pragma once

#include <stdexcept>

namespace services
{

  class ExpiredTokenException : public std::runtime_error
  {
  public:
    ExpiredTokenException(const std::string &message) : std::runtime_error(message)
    {
    }
  };

} // namespace services