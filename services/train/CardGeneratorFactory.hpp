#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/utils/coroutine.h>

#include "models/TrainCard.hpp"
#include "models/Translate.hpp"
#include "services/train/CardMode.hpp"
#include "models/TrainSession.hpp"
#include "services/train/generators/SourceToTargetInput.hpp"

namespace services::train
{

    class CardGeneratorFactory
    {
    public:
        drogon::Task<models::TrainCard> generate(
            const models::TrainSession &session,
            const services::train::CardMode &cardMode) const;
    };
}