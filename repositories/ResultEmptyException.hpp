#pragma once

#include <stdexcept>

namespace repositories
{

class ResultEmptyException : public std::runtime_error
{
  public:
    ResultEmptyException(const std::string message) : std::runtime_error(message)
    {
    }
};

} // namespace repositories