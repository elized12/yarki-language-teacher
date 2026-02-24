#include "dto/Validator.hpp"

using namespace dto;

Validator &Validator::getInstance()
{
    static Validator _instance;
    return _instance;
}

std::vector<std::string> Validator::isValidEmail(const std::string &email) const
{
    icu::UnicodeString utfEmail = icu::UnicodeString::fromUTF8(email);

    std::vector<std::string> errors;

    if (utfEmail.isEmpty())
    {
        errors.push_back("Email не должен быть пустым");
        return errors;
    }

    static const icu::UnicodeString pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher matcher(pattern, utfEmail, 0, status);
    if (!matcher.find())
    {
        errors.push_back("Email имеет некорректный формат");
    }

    return errors;
}

std::vector<std::string> Validator::isValidNickname(const std::string &nickname) const
{
    icu::UnicodeString utfNickname = icu::UnicodeString::fromUTF8(nickname);

    std::vector<std::string> errors;

    if (utfNickname.isEmpty())
    {
        errors.push_back("Никнейм не должен быть пустым");
        return errors;
    }

    if (utfNickname.length() < 3 || utfNickname.length() > 20)
    {
        errors.push_back("Никнейм должен содержать от 3 до 20 символов");
    }

    static const icu::UnicodeString pattern(R"(^[A-Za-z0-9]+$)");
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher matcher(pattern, utfNickname, 0, status);
    if (!matcher.find())
    {
        errors.push_back("Никнейм может содержать только латинские буквы и цифры");
    }

    return errors;
}

std::vector<std::string> Validator::isValidPassword(const std::string &password) const
{
    icu::UnicodeString utfPassword = icu::UnicodeString::fromUTF8(password);

    std::vector<std::string> errors;

    if (utfPassword.isEmpty())
    {
        errors.push_back("Пароль не должен быть пустым");
        return errors;
    }

    if (utfPassword.length() < 8)
    {
        errors.push_back("Пароль должен содержать минимум 8 символов");
    }

    static const icu::UnicodeString pattern(R"(\d)");
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher matcher(pattern, utfPassword, 0, status);

    if (!matcher.find())
    {
        errors.push_back("Пароль должен содержать хотя бы одну цифру");
    }

    static const icu::UnicodeString upperPattern(R"([A-Z])");
    icu::RegexMatcher upperMatcher(upperPattern, utfPassword, 0, status);
    if (!upperMatcher.find())
    {
        errors.push_back("Пароль должен содержать хотя бы одну заглавную букву");
    }

    return errors;
}
