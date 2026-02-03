#pragma once

#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

#include <string>

#include "dto/UserData.hpp"
#include "repositories/JwtTokenRepository.hpp"
#include "services/ExpiredTokenException.hpp"

namespace services
{

class JwtService
{
  private:
    std::string _secretKey;
    std::chrono::hours _refreshTokenValidityDuraction;
    std::chrono::minutes _accessTokenValidityDuraction;
    repositories::JwtTokenRepository _tokenRepository;

  public:
    JwtService(
            std::string secretKey,
            std::chrono::hours refreshTokenValidityDuraction,
            std::chrono::minutes accessTokenValidityDuraction,
            repositories::JwtTokenRepository tokenRepository
    );

  public:
    drogon::Task<bool> isValidRefreshToken(const std::string& token);
    bool isValidAccessToken(const std::string& token);
    drogon::Task<void> revoke(models::id userId);
    drogon::Task<std::string> refresh(const std::string& refreshToken);
    drogon::Task<std::pair<std::string, std::string>>
    createJwtTokens(const dto::UserData& userData);

    nlohmann::json getPayload(const std::string& token) const;
};

} // namespace services