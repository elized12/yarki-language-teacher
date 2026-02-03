#include "dto/Validator.hpp"

using namespace dto;

Validator& Validator::getInstance()
{
    static Validator _instance;
    return _instance;
}

std::vector<std::string> Validator::isValidEmail(const std::string& email) const
{
    std::vector<std::string> errors;

    if (email.empty())
    {
        errors.push_back("Email не должен быть пустым");
        return errors;
    }

    static const std::regex pattern(R"(^[\w\.-]+@[\w\.-]+\.\w+$)");
    if (!std::regex_match(email, pattern))
    {
        errors.push_back("Email должен быть в формате example@domain.com");
    }

    return errors;
}

std::vector<std::string> Validator::isValidNickname(const std::string& nickname) const
{
    std::vector<std::string> errors;

    if (nickname.empty())
    {
        errors.push_back("Никнейм не должен быть пустым");
        return errors;
    }

    if (nickname.size() < 3 || nickname.size() > 20)
    {
        errors.push_back("Никнейм должен содержать от 3 до 20 символов");
    }

    static const std::regex pattern(R"(^[A-Za-z0-9]+$)");
    if (!std::regex_match(nickname, pattern))
    {
        errors.push_back("Никнейм может содержать только латинские буквы и цифры");
    }

    return errors;
}

std::vector<std::string> Validator::isValidPassword(const std::string& password) const
{
    std::vector<std::string> errors;

    if (password.empty())
    {
        errors.push_back("Пароль не должен быть пустым");
        return errors;
    }

    if (password.size() < 8)
    {
        errors.push_back("Пароль должен содержать минимум 8 символов");
    }

    static const std::regex digit(R"(\d)");
    if (!std::regex_search(password, digit))
    {
        errors.push_back("Пароль должен содержать хотя бы одну цифру");
    }

    static const std::regex upper(R"([A-Z])");
    if (!std::regex_search(password, upper))
    {
        errors.push_back("Пароль должен содержать хотя бы одну заглавную букву");
    }

    static const std::regex special(R"([\W_])");
    if (!std::regex_search(password, special))
    {
        errors.push_back("Пароль должен содержать хотя бы один специальный символ");
    }

    return errors;
}
