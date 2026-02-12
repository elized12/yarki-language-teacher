#include "services/TrainService.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cctype>

using namespace services;

TrainService::TrainService(
    repositories::TrainSessionRepository trainSessionRepository,
    repositories::TrainCardRepository trainCardRepository,
    repositories::WordRepository wordRepository,
    repositories::TranslateRepository translateRepository,
    repositories::TrainUserAnswerRepository trainUserAnswerRepository,
    services::WordCleaner wordCleaner)
    : _trainSessionRepository(std::move(trainSessionRepository)),
      _trainCardRepository(std::move(trainCardRepository)),
      _wordRepository(std::move(wordRepository)),
      _translateRepository(std::move(translateRepository)),
      _wordCleaner(std::move(wordCleaner)),
      _trainUserAnswerRepository(std::move(trainUserAnswerRepository))
{
}

drogon::Task<std::string> TrainService::startSession(models::id userId, const models::TrainSettings &settings)
{
    models::TrainSession session;
    session.userId = userId;
    session.settings = settings;
    session.startedAt = std::chrono::system_clock::now();

    std::string id = co_await this->_trainSessionRepository.create(session);

    co_return id;
}

drogon::Task<bool> TrainService::finishSession(models::id userId, const std::string &id)
{
    std::optional<models::TrainSession> session = co_await this->_trainSessionRepository.get(id);
    if (!session.has_value())
    {
        throw NotExistException("this session not exists");
    }

    if (session->userId != userId)
    {
        throw AccessDeniedException("this user dont finished");
    }

    if (session->finishedAt.has_value())
    {
        throw services::ValidationException("this session already finished");
    }

    session->finishedAt = std::chrono::system_clock::now();
    bool isFinished = co_await this->_trainSessionRepository.update(session.value());

    co_return isFinished;
}

drogon::Task<models::TrainCard> TrainService::getTask(models::id userId, const std::string &sessionId)
{
    std::optional<models::TrainSession> session = co_await this->_trainSessionRepository.get(sessionId);
    if (!session.has_value())
    {
        throw NotExistException("this session not exists");
    }

    if (session->finishedAt.has_value())
    {
        throw services::ValidationException("this session already finished");
    }

    if (session->userId != userId)
    {
        throw AccessDeniedException("access denied for this user");
    }

    std::optional<models::TrainCard> currentCard = co_await this->_trainCardRepository.getCurrentCard(session->id);
    if (currentCard.has_value())
    {
        co_return currentCard.value();
    }

    int lastPositionCard = 0;

    const models::LanguageCode::Code &sourceLanguage = session->settings.sourceLanguage;
    const models::LanguageCode::Code &targetLanguage = session->settings.targetLanguage;

    std::vector<models::TrainCard> lastCards = co_await this->_trainCardRepository.getLastCards(session->id, TrainService::COUNT_LAST_CARD);

    std::vector<models::id> lastWordIds;
    for (const models::TrainCard &card : lastCards)
    {
        lastWordIds.push_back(card.targetWordId);
        lastPositionCard = card.position;
    }

    std::vector<models::Translate> translates = co_await this->_translateRepository.getTranslates(
        userId,
        sourceLanguage,
        targetLanguage,
        lastWordIds,
        TrainService::COUNT_TRANSLATE);

    if (translates.empty())
    {
        throw NotExistException("no translates for session");
    }

    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_int_distribution<> distrib(0, static_cast<int>(translates.size() - 1));

    int randomIndex = distrib(gen);

    const models::Translate &selectedTranslate = translates[randomIndex];

    auto wordAOpt = co_await this->_wordRepository.get(selectedTranslate.firstWordId);
    auto wordBOpt = co_await this->_wordRepository.get(selectedTranslate.secondWordId);

    if (!wordAOpt.has_value() || !wordBOpt.has_value())
    {
        throw NotExistException("words for translate not found");
    }

    models::Word wordA = wordAOpt.value();
    models::Word wordB = wordBOpt.value();

    models::Word sourceWord = wordA;
    models::Word targetWord = wordB;
    if (models::LanguageCode::toCode(wordA.languageCode) != sourceLanguage)
    {
        sourceWord = wordB;
        targetWord = wordA;
    }

    models::TrainCard card;
    card.createdAt = std::chrono::system_clock::now();
    card.position = lastPositionCard + 1;
    card.targetWordId = targetWord.id;
    card.trainCardModeId = 1;
    card.trainSessionId = session->id;

    nlohmann::json cardParams;
    cardParams["source_word_content"] = sourceWord.content;
    cardParams["source_word_id"] = sourceWord.id;
    cardParams["source_word_language_code"] = sourceWord.languageCode;

    card.params = cardParams.dump();

    models::id createdId = co_await this->_trainCardRepository.create(card);
    card.id = createdId;

    co_return card;
}

drogon::Task<bool> TrainService::answerTask(const std::string &answer, models::id cardId)
{
    auto cardOpt = co_await this->_trainCardRepository.get(cardId);
    if (!cardOpt.has_value())
    {
        throw NotExistException("card not found");
    }

    models::TrainCard card = cardOpt.value();

    auto targetWordOpt = co_await this->_wordRepository.get(card.targetWordId);
    if (!targetWordOpt.has_value())
    {
        throw NotExistException("target word not found");
    }

    std::string expected = targetWordOpt->content;

    std::string answerClean = this->_wordCleaner.clean(answer);
    std::string expectedClean = this->_wordCleaner.clean(expected);

    bool isCorrect = answerClean == expectedClean;

    models::UserAnswer ua;
    ua.trainCardId = card.id;
    ua.content = answer;
    ua.isCorrect = isCorrect;

    co_await this->_trainUserAnswerRepository.create(ua);

    co_return isCorrect;
}