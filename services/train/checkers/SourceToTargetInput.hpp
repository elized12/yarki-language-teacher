#pragma once

#include "services/train/checkers/IChecker.hpp"
#include "repositories/TranslateRepository.hpp"
#include "repositories/WordRepository.hpp"
#include "repositories/TrainSessionRepository.hpp"
#include "services/WordCleaner.hpp"
#include "services/NotExistException.hpp"

namespace services::train::checkers
{

    class SourceToTargetInput : public IChecker
    {
    private:
        repositories::TranslateRepository _translateRepository;
        services::WordCleaner _wordCleaner;
        repositories::TrainSessionRepository _trainSessionRepository;

    public:
        SourceToTargetInput(
            repositories::TranslateRepository translateRepository,
            repositories::TrainSessionRepository trainSessionRepository,
            services::WordCleaner wordCleaner);

    public:
        drogon::Task<bool> check(const models::TrainCard &card, const std::string &answer) override;
    };

}