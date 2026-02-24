#include "services/train/generators/SourceToTargetInput.hpp"

using namespace services::train::generators;

SourceToTargetInput::SourceToTargetInput(
    repositories::TrainCardRepository trainCardRepository,
    repositories::TranslateRepository translateRepository,
    repositories::WordRepository wordRepository)
    : _trainCardRepository(trainCardRepository),
      _translateRepository(translateRepository),
      _wordRepository(wordRepository)
{
}

drogon::Task<models::TrainCard> SourceToTargetInput::generate(const models::TrainSession &session)
{
    const models::LanguageCode::Code &sourceLanguage = session.settings.sourceLanguage;
    const models::LanguageCode::Code &targetLanguage = session.settings.targetLanguage;

    std::vector<models::TrainCard> lastCards = co_await this->_trainCardRepository.getLastCards(session.id, SourceToTargetInput::COUNT_LAST_CARD);
    std::vector<models::id> lastWordIds = this->getLastWordIds(lastCards);
    std::vector<models::Translate> translates = co_await this->_translateRepository.getTranslates(
        session.userId,
        sourceLanguage,
        targetLanguage,
        lastWordIds,
        SourceToTargetInput::COUNT_TRANSLATE);

    if (translates.empty())
    {
        throw NotExistException("no translates for session");
    }

    int randomIndex = this->generateRandomIndex(translates.size());
    const models::Translate &selectedTranslate = translates[randomIndex];

    auto wordAOpt = co_await this->_wordRepository.get(selectedTranslate.firstWordId);
    auto wordBOpt = co_await this->_wordRepository.get(selectedTranslate.secondWordId);

    if (!wordAOpt.has_value() || !wordBOpt.has_value())
    {
        throw NotExistException("words for translate not found");
    }

    models::Word &wordA = wordAOpt.value();
    models::Word &wordB = wordBOpt.value();

    models::Word &sourceWord = wordA;
    models::Word &targetWord = wordB;
    if (models::LanguageCode::toCode(wordA.languageCode) != sourceLanguage)
    {
        sourceWord = wordB;
        targetWord = wordA;
    }

    models::TrainCard card;
    card.createdAt = std::chrono::system_clock::now();
    card.position = lastPositionCard + 1;
    card.targetWordId = targetWord.id;
    card.trainCardModeId = services::train::CardMode::SourceToTargetInput;
    card.trainSessionId = session.id;
    card.sourceWordId = sourceWord.id;

    co_return card;
}

std::vector<models::id> SourceToTargetInput::getLastWordIds(const std::vector<models::TrainCard> &lastCards)
{
    std::vector<models::id> lastWordIds;
    for (const models::TrainCard &card : lastCards)
    {
        lastWordIds.push_back(card.targetWordId);
    }

    return lastWordIds;
}

int SourceToTargetInput::generateRandomIndex(int size)
{
    static std::random_device randomDevice;
    static std::mt19937 gen(randomDevice());
    std::uniform_int_distribution<> distrib(0, size - 1);

    return distrib(gen);
}