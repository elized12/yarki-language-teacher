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

    UserRegistration(
            const std::string& _nickname, const std::string& _email, const std::string& _password
    )
        : nickname(_nickname), email(_email), password(_password)
    {
    }
};

} // namespace dto