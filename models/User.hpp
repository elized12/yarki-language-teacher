#pragma once

#include <cstdint>
#include <string>

#include "models/Type.hpp"

namespace models
{

  class User
  {
  public:
    models::id id;
    std::string hashedPassword;
    std::string email;
    std::string nickname;
  };

} // namespace models