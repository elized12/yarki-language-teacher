#pragma once

#include "dto/UserLogin.hpp"
#include "dto/UserRegistration.hpp"
#include "dto/Validator.hpp"
#include "repositories/UserRepository.hpp"
#include "services/JwtSerivce.hpp"
#include "services/ValidataionException.hpp"
#include "services/auth/UserAlreadyExistException.hpp"

namespace services
{

class AuthSerivce
{
  private:
    repositories::UserRepository _userRepository;
    services::JwtService _jwtService;
    dto::Validator& _validator;

  public:
    AuthSerivce(
            repositories::UserRepository userRepository,
            services::JwtService jwtService,
            dto::Validator& validator
    );

  public:
    drogon::Task<models::id> registerUser(const dto::UserRegistration& userData);
    drogon::Task<std::pair<std::string, std::string>> loginUser(const dto::UserLogin& credentials);
    drogon::Task<std::string> refresh(const std::string& refreshToken);
    drogon::Task<void> logoutUser(const models::id userId);
    drogon::Task<std::optional<models::User>> getUser(const models::id userId);
};

} // namespace services