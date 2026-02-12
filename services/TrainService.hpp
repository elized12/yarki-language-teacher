#pragma once

#include <random>

#include "repositories/TrainSessionRepository.hpp"
#include "repositories/TrainCardRepository.hpp"
#include "repositories/WordRepository.hpp"
#include "repositories/TrainUserAnswerRepository.hpp"
#include "services/NotExistException.hpp"
#include "services/ValidataionException.hpp"
#include "services/AccessDeniedException.hpp"
#include "repositories/TranslateRepository.hpp"
#include "services/WordCleaner.hpp"

namespace services
{

    class TrainService
    {
    private:
        repositories::TrainSessionRepository _trainSessionRepository;
        repositories::TrainCardRepository _trainCardRepository;
        repositories::WordRepository _wordRepository;
        repositories::TranslateRepository _translateRepository;
        services::WordCleaner _wordCleaner;
        repositories::TrainUserAnswerRepository _trainUserAnswerRepository;

        const static int COUNT_LAST_CARD = 100;
        const static int COUNT_TRANSLATE = 10000;

    public:
        TrainService(
            repositories::TrainSessionRepository trainSessionRepository,
            repositories::TrainCardRepository trainCardRepository,
            repositories::WordRepository wordRepository,
            repositories::TranslateRepository translateRepository,
            repositories::TrainUserAnswerRepository trainUserAnswerRepository,
            services::WordCleaner wordCleaner);

    public:
        drogon::Task<std::string> startSession(models::id userId, const models::TrainSettings &settings);
        drogon::Task<bool> finishSession(models::id userId, const std::string &id);

        drogon::Task<models::TrainCard> getTask(models::id userId, const std::string &sessionId);
        drogon::Task<bool> answerTask(const std::string &answer, models::id cardId);
    };

}