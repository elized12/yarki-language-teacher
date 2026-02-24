#pragma once

#include "services/train/checkers/IChecker.hpp"
#include "repositories/TranslateRepository.hpp"

namespace services::train::checkers
{

    class SourceToTargetInput : public IChecker
    {
    private:
    public:
        SourceToTargetInput();

    public:
        drogon::Task<bool> check(const models::TrainCard &card, const std::string &answer) override;
    };

}