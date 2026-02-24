#pragma once

#include "models/Type.hpp"

namespace models
{

    class TrainCard
    {
    public:
        models::id id;
        std::string trainSessionId;
        models::id trainCardModeId;
        models::timePoint createdAt;
        int position;
        std::string params;
        models::id sourceWordId;
    };

}