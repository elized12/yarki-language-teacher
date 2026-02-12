#pragma once

#include "models/LanguageCode.hpp"
#include "models/Type.hpp"

namespace dto
{

  class UpdateTranslate
  {
  public:
    models::id userId;
    models::id firstWordId;
    models::id secondWordId;
    models::LanguageCode::Code updateCodeWord;
    std::string newContent;
  };

} // namespace dto