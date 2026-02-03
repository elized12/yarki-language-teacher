#pragma once

#include <string>

namespace dto
{

class UserRegistration
{
  public:
    std::string nickname;
    std::string email;
    std::string password;
};

} // namespace dto