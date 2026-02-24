#pragma once

#include <drogon/utils/coroutine.h>

#include "models/TrainCard.hpp"

namespace services::train::checkers
{

    class IChecker
    {
    public:
        virtual ~IChecker() = default;

    public:
        virtual drogon::Task<bool>
        check(const models::TrainCard &card, const std::string &answer) = 0;
    };

}