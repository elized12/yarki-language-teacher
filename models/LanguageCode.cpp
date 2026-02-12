#include "models/LanguageCode.hpp"

using namespace models;

const std::array<int, 2> LanguageCode::_values = {1, 2};

const std::array<std::string, 2> LanguageCode::_names = {"RU", "EN"};

bool LanguageCode::isLangCodeExist(std::string code)
{
    std::transform(code.begin(), code.end(), code.begin(), [](char s)
                   { return std::toupper(s); });
    return std::find(_names.begin(), _names.end(), code) != _names.end();
}

std::string LanguageCode::toString(const LanguageCode::Code &code)
{
    return _names[static_cast<int>(code) - 1];
}

LanguageCode::Code LanguageCode::toCode(std::string code)
{
    std::transform(code.begin(), code.end(), code.begin(), [](char s)
                   { return std::toupper(s); });
    for (size_t i = 0; i < _names.size(); i++)
    {
        if (_names[i] == code)
        {
            return static_cast<LanguageCode::Code>(_values[i]);
        }
    }

    throw std::runtime_error("Не существует такого языкового кода");
}
