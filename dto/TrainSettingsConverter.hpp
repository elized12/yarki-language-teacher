#pragma once

#include <nlohmann/json.hpp>

#include "models/TrainSettings.hpp"
#include "dto/NotRequiredFieldException.hpp"

namespace dto
{

    class TrainSettingsConverter
    {
    public:
        std::string convertSettingsToString(const models::TrainSettings &settings) const;
        models::TrainSettings convertStringToSettings(const std::string &settings) const;
        bool isValidString(const std::string &settings) const;
    };

}