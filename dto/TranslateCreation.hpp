#pragma once

#include "dto/Word.hpp"
#include "models/Type.hpp"

namespace dto
{

  class TranslateCreation
  {
  public:
    models::id userId;
    dto::Word word;
    dto::Word translate;
  };

} // namespace dto