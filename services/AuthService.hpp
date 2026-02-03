#pragma once

#include <nlohmann/json.hpp>

#include "dto/UserLogin.hpp"
#include "dto/UserRegistration.hpp"
#include "dto/Validator.hpp"
#include "repositories/UserRepository.hpp"
#include "services/JwtSerivce.hpp"
#include "services/ValidataionException.hpp"
#include "services/auth/UserAlreadyExistException.hpp"

namespace services
{

class AuthService
{
  private:
    repositories::UserRepository _userRepository;
    services::JwtService _jwtService;
    dto::Validator& _validator;

  public:
    AuthService(
            repositories::UserRepository userRepository,
            services::JwtService jwtService,
            dto::Validator& validator
    );

  public:
    drogon::Task<models::id> registerUser(const dto::UserRegistration& userData);
    drogon::Task<std::pair<std::string, std::string>> loginUser(const dto::UserLogin& credentials);
    bool isValidAccessToken(const std::string& accessToken);
    drogon::Task<bool> isValidRefreshToken(const std::string& refreshToken);
    drogon::Task<std::string> refresh(const std::string& refreshToken);
    drogon::Task<void> logoutUser(const models::id userId);
    drogon::Task<std::optional<models::User>> getUser(const models::id userId);

    nlohmann::json getPayload(const std::string& token) const;
};

} // namespace services