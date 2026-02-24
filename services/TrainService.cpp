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
    repositories::TrainUserAnswerRepository trainUserAnswerRepository)
    : _trainSessionRepository(std::move(trainSessionRepository)),
      _trainCardRepository(std::move(trainCardRepository)),
      _wordRepository(std::move(wordRepository)),
      _translateRepository(std::move(translateRepository)),
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

    static services::train::CardGeneratorFactory cardGeneratorFactory;
    models::TrainCard card = co_await cardGeneratorFactory.generate(session.value(), services::train::CardMode::SOURCE_TO_TARGET_INPUT);

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
    services::train::CardMode cardMode = static_cast<services::train::CardMode>(card.trainCardModeId);

    static services::train::CardCheckerFactory cardCheckerFactory;
    bool isCorrect = co_await cardCheckerFactory.check(card, answer, cardMode);

    models::UserAnswer ua;
    ua.trainCardId = card.id;
    ua.content = answer;
    ua.isCorrect = isCorrect;

    co_await this->_trainUserAnswerRepository.create(ua);

    co_return isCorrect;
}