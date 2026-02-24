#pragma once

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
        static const int COUNT_LAST_CARD = 5000;
        static const int COUNT_TRANSLATE = 10000;

        repositories::TrainCardRepository _trainCardRepository;
        repositories::TranslateRepository _translateRepository;
        repositories::WordRepository _wordRepository;

    public:
        SourceToTargetInput(
            repositories::TrainCardRepository trainCardRepository,
            repositories::TranslateRepository translateRepository,
            repositories::WordRepository wordRepository);

    public:
        drogon::Task<models::TrainCard>
        generate(
            const std::vector<models::TrainCard> &lastCards,
            const std::vector<models::Translate> &translates,
            const models::TrainSession &session) override;

    private:
        std::vector<models::id> getLastWordIds(const std::vector<models::TrainCard> &lastCards);
        int generateRandomIndex(int size);
    };

}