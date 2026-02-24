#include "services/train/CardGeneratorFactory.hpp"

using namespace services::train;

drogon::Task<models::TrainCard> CardGeneratorFactory::generate(
    const models::TrainSession &session,
    const services::train::CardMode &cardMode) const
{
    switch (cardMode)
    {
    case services::train::CardMode::SOURCE_TO_TARGET_INPUT:
    {
        static services::train::generators::SourceToTargetInput sourceToTargetInput(
            repositories::TrainCardRepository(drogon::app().getDbClient()),
            repositories::WordRepository(drogon::app().getDbClient()));
        models::TrainCard card = co_await sourceToTargetInput.generate(session);
        co_return card;
    }
    default:
        throw std::invalid_argument("unsupported card mode");
    }
}