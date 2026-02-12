#pragma once

#include "models/Type.hpp"

namespace models
{

    class UserAnswer
    {
    public:
        models::id id;
        models::id trainCardId;
        models::timePoint createdAt;
        std::string content;
        bool isCorrect;
    };

}