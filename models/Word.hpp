#pragma once

#include "models/Type.hpp"

namespace models
{

class Word
{
  public:
    models::id id;
    std::string languageCode;
    std::string content;
};

} // namespace models