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
    timePoint expiresAt;
    std::optional<timePoint> revokedAt;
};

} // namespace models