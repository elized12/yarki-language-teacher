#include "services/train/generators/SourceToTargetInput.hpp"

using namespace services::train::generators;

SourceToTargetInput::SourceToTargetInput(
    repositories::TrainCardRepository trainCardRepository,
    repositories::WordRepository wordRepository)
    : _trainCardRepository(trainCardRepository),
      _wordRepository(wordRepository)
{
}

drogon::Task<models::TrainCard> SourceToTargetInput::generate(const models::TrainSession &session)
{
    const models::LanguageCode::Code &sourceLanguage = session.settings.sourceLanguage;
    const models::LanguageCode::Code &targetLanguage = session.settings.targetLanguage;

    std::vector<models::Word> wordCandidates = co_await this->_wordRepository.getNextWordCandidates(session, sourceLanguage);
    if (wordCandidates.empty())
    {
        throw NotExistException("no words for session");
    }

    int randomIndex = this->generateRandomIndex(wordCandidates.size());
    const models::Word &selectedWord = wordCandidates[randomIndex];

    std::optional<int> lastPositionCardOpt = co_await this->_trainCardRepository.getLastPosition(session.id);
    int lastPositionCard = lastPositionCardOpt.has_value() ? lastPositionCardOpt.value() : 0;

    models::TrainCard card;
    card.createdAt = std::chrono::system_clock::now();
    card.position = lastPositionCard + 1;
    card.sourceWordId = selectedWord.id;
    card.trainCardModeId = static_cast<int>(services::train::CardMode::SOURCE_TO_TARGET_INPUT);
    card.trainSessionId = session.id;

    nlohmann::json cardParams;
    cardParams["source_word_content"] = selectedWord.content;
    cardParams["source_word_id"] = selectedWord.id;
    cardParams["source_word_language_code"] = selectedWord.languageCode;

    card.params = cardParams.dump();

    models::id createdId = co_await this->_trainCardRepository.create(card);
    card.id = createdId;

    co_return card;
}

int SourceToTargetInput::generateRandomIndex(int size)
{
    static std::random_device randomDevice;
    static std::mt19937 gen(randomDevice());
    std::uniform_int_distribution<> distrib(0, size - 1);

    return distrib(gen);
}