#pragma once

#include "models/Type.hpp"

namespace dto
{

  class UserData
  {
  public:
    models::id id;
    std::string nickname;
    std::string email;
  };

} // namespace dto