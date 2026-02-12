#pragma once

#include <optional>

#include "models/Type.hpp"
#include "models/LanguageCode.hpp"

namespace models
{

    class TrainSettings
    {
    public:
        std::optional<models::timePoint> addedAfterWord;
        models::LanguageCode::Code sourceLanguage;
        models::LanguageCode::Code targetLanguage;
    };

}