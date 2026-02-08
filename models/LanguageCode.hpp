#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>

namespace models
{

class LanguageCode
{
  private:
    static const std::array<int, 2> _values;
    static const std::array<std::string, 2> _names;

  public:
    enum Code
    {
        RU = 0,
        EN = 1,
    };

  public:
    static bool isLangCodeExist(std::string code);
    static std::string toString(const Code& code);
    static Code toCode(std::string code);
};

} // namespace models
