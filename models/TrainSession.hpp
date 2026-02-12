#pragma once

#include <optional>

#include "models/TrainSettings.hpp"

namespace models
{

    class TrainSession
    {
    public:
        std::string id;
        models::id userId;
        models::timePoint startedAt;
        std::optional<models::timePoint> finishedAt;
        models::TrainSettings settings;
    };

}