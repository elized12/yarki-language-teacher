#include "services/AuthService.hpp"

using namespace services;

AuthSerivce::AuthSerivce(
        repositories::UserRepository userRepository,
        services::JwtService jwtService,
        dto::Validator& validator
)
    : _userRepository(std::move(userRepository)), _jwtService(std::move(jwtService)),
      _validator(validator)
{
}

drogon::Task<models::id> AuthSerivce::registerUser(const dto::UserRegistration& userData)
{
    std::vector<std::string> errors;
    errors = this->_validator.isValidEmail(userData.email);
    if (!errors.empty())
    {
        throw ValidationException(std::string("Email поле ошибка: " + errors[0]));
    }

    errors = this->_validator.isValidNickname(userData.nickname);
    if (!errors.empty())
    {
        throw ValidationException(std::string("Nickname поле ошибка: " + errors[0]));
    }

    errors = this->_validator.isValidPassword(userData.password);
    if (!errors.empty())
    {
        throw ValidationException(std::string("Password поле ошибка: " + errors[0]));
    }

    auto existingUser =
            co_await this->_userRepository.getByCredentials(userData.email, userData.password);
    if (existingUser.has_value())
    {
        throw auth::UserAlreadyExistException("Пользователь с таким email уже существует");
    }

    models::User newUser;
    newUser.email = userData.email;
    newUser.nickname = userData.nickname;
    newUser.hashedPassword = userData.password;

    co_return co_await this->_userRepository.create(newUser);
}

drogon::Task<std::pair<std::string, std::string>>
AuthSerivce::loginUser(const dto::UserLogin& credentials)
{
    auto userOpt = co_await this->_userRepository.getByCredentials(
            credentials.email, credentials.password
    );
    if (!userOpt.has_value())
    {
        throw ValidationException("Неверный email или пароль");
    }

    std::pair<std::string, std::string> tokens = co_await _jwtService.createJwtTokens(
            {.id = userOpt->id, .nickname = userOpt->nickname, .email = userOpt->email}
    );

    co_return tokens;
}

drogon::Task<std::string> AuthSerivce::refresh(const std::string& refreshToken)
{
    bool valid = co_await _jwtService.isValidRefreshToken(refreshToken);
    if (!valid)
    {
        throw ValidationException("Невалидный или отозванный refresh-token");
    }

    auto tokenDecoded = jwt::decode(refreshToken);
    models::id userId = tokenDecoded.get_payload_claim("userId").as_integer();

    std::optional<models::User> user = co_await _userRepository.get(userId);
    if (!user.has_value())
    {
        throw ValidationException("Пользователь не найден");
    }

    std::string accessToken = co_await this->_jwtService.refresh(refreshToken);

    co_return accessToken;
}

drogon::Task<void> AuthSerivce::logoutUser(const models::id userId)
{
    co_await this->_jwtService.revoke(userId);
}

drogon::Task<std::optional<models::User>> AuthSerivce::getUser(const models::id userId)
{
    co_return co_await this->_userRepository.get(userId);
}
