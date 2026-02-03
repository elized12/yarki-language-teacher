#pragma once

#include <ctime>
#include <optional>

#include "models/Type.hpp"

namespace models
{

class JwtToken
{
  public:
    models::id id;
    models::id userId;
    std::string hashedToken;
    timePoint issuedAt;
    timePoint expiredAt;
    std::optional<timePoint> revokedAt;
};

} // namespace models