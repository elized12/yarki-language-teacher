#pragma once

#include <unicode/regex.h>
#include <unicode/unistr.h>

#include <regex>
#include <string>
#include <vector>

namespace dto
{

  class Validator
  {
  private:
    Validator() = default;

  public:
    static Validator &getInstance();

    std::vector<std::string> isValidEmail(const std::string &email) const;
    std::vector<std::string> isValidNickname(const std::string &nickname) const;
    std::vector<std::string> isValidPassword(const std::string &password) const;
  };

} // namespace dto