#pragma once

#include <drogon/utils/coroutine.h>

#include "services/train/checkers/SourceToTargetInput.hpp"
#include "models/TrainCard.hpp"
#include "services/train/CardMode.hpp"

namespace services::train
{

    class CardCheckerFactory
    {
    public:
        drogon::Task<bool> check(
            const models::TrainCard &card,
            const std::string &answer,
            const services::train::CardMode &mode);
    };

}