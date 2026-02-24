#pragma once

#include <nlohmann/json.hpp>

#include <random>

#include "services/train/generators/IGenerator.hpp"
#include "services/NotExistException.hpp"
#include "models/TrainCard.hpp"
#include "models/TrainSession.hpp"
#include "repositories/TrainCardRepository.hpp"
#include "repositories/TranslateRepository.hpp"
#include "repositories/WordRepository.hpp"
#include "services/train/CardMode.hpp"

namespace services::train::generators
{

    class SourceToTargetInput : public IGenerator
    {
    private:
        repositories::TrainCardRepository _trainCardRepository;
        repositories::WordRepository _wordRepository;

    public:
        SourceToTargetInput(
            repositories::TrainCardRepository trainCardRepository,
            repositories::WordRepository wordRepository);

    public:
        drogon::Task<models::TrainCard>
        generate(const models::TrainSession &session) override;

    private:
        int generateRandomIndex(int size);
    };

}