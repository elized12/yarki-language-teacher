#include "services/train/checkers/SourceToTargetInput.hpp"

using namespace services::train::checkers;

SourceToTargetInput::SourceToTargetInput(
    repositories::TranslateRepository translateRepository,
    repositories::TrainSessionRepository trainSessionRepository,
    services::WordCleaner wordCleaner)
    : _translateRepository(std::move(translateRepository)),
      _trainSessionRepository(std::move(trainSessionRepository)),
      _wordCleaner(std::move(wordCleaner))
{
}

drogon::Task<bool> SourceToTargetInput::check(const models::TrainCard &card, const std::string &answer)
{
    std::optional<models::TrainSession> sessionOpt = co_await this->_trainSessionRepository.get(card.trainSessionId);
    if (!sessionOpt.has_value())
    {
        throw NotExistException("train session not found");
    }

    models::LanguageCode::Code sourceLanguage = sessionOpt->settings.sourceLanguage;
    models::LanguageCode::Code targetLanguage = sessionOpt->settings.targetLanguage;

    std::string cleanAnswer = this->_wordCleaner.clean(answer);

    bool isCorrect = co_await this->_translateRepository.isTranslateExist(
        card.sourceWordId,
        cleanAnswer,
        sourceLanguage,
        sessionOpt->userId);

    co_return isCorrect;
}