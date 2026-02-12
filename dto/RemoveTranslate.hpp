#pragma once

#include "models/Type.hpp"

namespace dto
{

  class RemoveTranslate
  {
  public:
    models::id userId;
    models::id firstWordId;
    models::id secondWordId;
  };

} // namespace dto