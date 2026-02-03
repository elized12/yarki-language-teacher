#pragma once

#include "models/Type.hpp"

namespace models
{

class Translate
{
  public:
    models::id firstWordId;
    models::id secondWordId;
    models::timePoint createdAt;
    models::id userId;
};

} // namespace models