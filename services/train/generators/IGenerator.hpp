#pragma once

#include <drogon/utils/coroutine.h>

#include "models/TrainCard.hpp"
#include "models/TrainSession.hpp"
#include "models/Translate.hpp"

namespace services::train::generators
{

    class IGenerator
    {
    public:
        virtual ~IGenerator() = default;

    public:
        virtual drogon::Task<models::TrainCard> generate(const models::TrainSession &session) = 0;
    };

}