#include "dto/TrainSettingsConverter.hpp"

using namespace dto;

std::string TrainSettingsConverter::convertSettingsToString(const models::TrainSettings &settings) const
{
    nlohmann::json json;
    json["source_language"] = models::LanguageCode::toString(settings.sourceLanguage);
    json["target_language"] = models::LanguageCode::toString(settings.targetLanguage);
    if (settings.addedAfterWord.has_value())
    {
        json["added_after_word"] = models::timePointToString(settings.addedAfterWord.value());
    }
    else
    {
        json["added_after_word"] = nullptr;
    }

    return json.dump();
}

models::TrainSettings TrainSettingsConverter::convertStringToSettings(const std::string &settings) const
{
    models::TrainSettings trainSettings;

    nlohmann::json json = nlohmann::json::parse(settings);
    if (auto it = json.find("added_after_word"); it != json.end() && !it->is_null())
    {
        try
        {
            trainSettings.addedAfterWord = models::stringToTimePoint(json["added_after_word"].get<std::string>());
        }
        catch (std::exception &ex)
        {
            trainSettings.addedAfterWord = std::nullopt;
        }
    }
    else
    {
        trainSettings.addedAfterWord = std::nullopt;
    }

    if (auto it = json.find("source_language"); it != json.end())
    {
        trainSettings.sourceLanguage = models::LanguageCode::toCode(json["source_language"].get<std::string>());
    }
    else
    {
        throw NotRequiredFieldException("not \"source_language\" required field");
    }

    if (auto it = json.find("target_language"); it != json.end())
    {
        trainSettings.targetLanguage = models::LanguageCode::toCode(json["target_language"].get<std::string>());
    }
    else
    {
        throw NotRequiredFieldException("not \"target_language\" required field");
    }

    return trainSettings;
}

bool TrainSettingsConverter::isValidString(const std::string &settings) const
{
    try
    {
        nlohmann::json json = nlohmann::json::parse(settings);
        if (auto it = json.find("added_after_word"); it == json.end())
        {
            return false;
        }
        else
        {
            if (!it->is_null())
            {
                models::stringToTimePoint(it.value());
            }
        }

        if (auto it = json.find("source_language"); it == json.end())
        {
            return false;
        }
        else
        {
            if (!models::LanguageCode::isLangCodeExist(it.value()))
            {
                return false;
            }
        }

        if (auto it = json.find("target_language"); it == json.end())
        {
            return false;
        }
        else
        {
            if (!models::LanguageCode::isLangCodeExist(it.value()))
            {
                return false;
            }
        }

        return true;
    }
    catch (const std::exception &ex)
    {
        return false;
    }
}