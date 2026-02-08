#include "services/JwtService.hpp"

using namespace services;

JwtService::JwtService(
        std::string secretKey,
        std::chrono::hours refreshTokenValidityDuraction,
        std::chrono::minutes accessTokenValidityDuraction,
        repositories::JwtTokenRepository tokenRepository
)
    : _secretKey(std::move(secretKey)),
      _refreshTokenValidityDuraction(std::move(refreshTokenValidityDuraction)),
      _accessTokenValidityDuraction(std::move(accessTokenValidityDuraction)),
      _tokenRepository(std::move(tokenRepository))
{
}

drogon::Task<bool> JwtService::isValidRefreshToken(const std::string& refreshToken)
{
    try
    {
        auto decodedJwt = jwt::decode(refreshToken);
        auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256(this->_secretKey));

        verifier.verify(decodedJwt);

        std::optional<models::JwtToken> token =
                co_await this->_tokenRepository.getByToken(refreshToken);

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        if (token.has_value() && token->revokedAt == std::nullopt && now < token->expiredAt)
        {
            co_return true;
        }

        co_return false;
    }
    catch (const std::system_error& ex)
    {
        co_return false;
    }
    catch (const std::exception& ex)
    {
        throw;
    }
}

bool JwtService::isValidAccessToken(const std::string& token) const
{
    try
    {
        auto decodedJwt = jwt::decode(token);
        auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256(this->_secretKey));

        verifier.verify(decodedJwt);

        return true;
    }
    catch (const std::exception& ex)
    {
        return false;
    }
}

drogon::Task<std::pair<std::string, std::string>>
JwtService::createJwtTokens(const dto::UserData& userData)
{
    std::chrono::system_clock::time_point issuedAt = std::chrono::system_clock::now();

    std::string refreshToken =
            jwt::create()
                    .set_type("JWT")
                    .set_issued_at(issuedAt)
                    .set_expires_in(this->_refreshTokenValidityDuraction)
                    .set_payload_claim("userId", jwt::claim(std::to_string(userData.id)))
                    .set_payload_claim("nickname", jwt::claim(userData.nickname))
                    .set_payload_claim("email", jwt::claim(userData.email))
                    .sign(jwt::algorithm::hs256(this->_secretKey));

    models::JwtToken token;
    token.userId = userData.id;
    token.hashedToken = refreshToken;
    token.issuedAt = issuedAt;
    token.expiredAt = issuedAt + this->_refreshTokenValidityDuraction;
    token.revokedAt = std::nullopt;

    auto tokenId = this->_tokenRepository.create(token);

    std::string accessToken =
            jwt::create()
                    .set_type("JWT")
                    .set_issued_now()
                    .set_expires_in(this->_accessTokenValidityDuraction)
                    .set_payload_claim("userId", jwt::claim(std::to_string(userData.id)))
                    .set_payload_claim("nickname", jwt::claim(userData.nickname))
                    .set_payload_claim("email", jwt::claim(userData.email))
                    .sign(jwt::algorithm::hs256(this->_secretKey));

    co_await tokenId;

    co_return {accessToken, refreshToken};
}

drogon::Task<void> JwtService::revoke(models::id userId)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    co_await this->_tokenRepository.revokeByUserId(userId, now);
}

drogon::Task<std::string> JwtService::refresh(const std::string& refreshToken)
{
    if (!co_await this->isValidRefreshToken(refreshToken))
    {
        throw ExpiredTokenException("refresh token невалиден");
    }

    auto tokenDecoded = jwt::decode(refreshToken);
    models::id userId = std::stoull(tokenDecoded.get_payload_claim("userId").as_string());
    std::string nickname = tokenDecoded.get_payload_claim("nickname").as_string();
    std::string email = tokenDecoded.get_payload_claim("email").as_string();

    std::string accessToken =
            jwt::create()
                    .set_type("JWT")
                    .set_issued_now()
                    .set_expires_in(this->_accessTokenValidityDuraction)
                    .set_payload_claim("userId", jwt::claim(std::to_string(userId)))
                    .set_payload_claim("nickname", jwt::claim(nickname))
                    .set_payload_claim("email", jwt::claim(email))
                    .sign(jwt::algorithm::hs256(this->_secretKey));

    co_return accessToken;
}

jwt::traits::kazuho_picojson::object_type JwtService::getPayload(const std::string& token) const
{
    auto decodedJwt = jwt::decode(token);

    return decodedJwt.get_payload_json();
}
