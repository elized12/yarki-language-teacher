#pragma once

#include "models/TrainCard.hpp"
#include "models/TrainSettings.hpp"
#include "models/Translate.hpp"
#include "services/train/CardMode.hpp"

namespace services::train
{

    class CardGenerator
    {
    public:
        models::TrainCard generate(
            const std::vector<models::TrainCard> &lastCards,
            const std::vector<models::Translate> &translates,
            const models::TrainSettings &settings,
            const CardMode &cardMode);
    };