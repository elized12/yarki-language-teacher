#include "services/train/CardCheckerFactory.hpp"

using namespace services::train;

drogon::Task<bool> CardCheckerFactory::check(
    const models::TrainCard &card,
    const std::string &answer,
    const services::train::CardMode &mode)
{
    switch (mode)
    {
    case CardMode::SOURCE_TO_TARGET_INPUT:
        static services::train::checkers::SourceToTargetInput sourceToTargetInputChecker(
            repositories::TranslateRepository(drogon::app().getDbClient()),
            repositories::TrainSessionRepository(drogon::app().getDbClient(), dto::TrainSettingsConverter()),
            services::WordCleaner());

        co_return co_await sourceToTargetInputChecker.check(card, answer);
    default:
        co_return false;
    }
}