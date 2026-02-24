#include "services/train/checkers/SourceToTargetInput.hpp"

using namespace services::train::checkers;

SourceToTargetInput::SourceToTargetInput()
{
}

drogon::Task<bool> SourceToTargetInput::check(const models::TrainCard &card, const std::string &answer)
{
    auto sourceWordId = co_await this->_wordRepository.get(card.sourceWordId);
    if (!sourceWordId.has_value())
    {
        throw NotExistException("source word not found");
    }

    std::string expected = sourceWordId->content;

    std::string answerClean = this->_wordCleaner.clean(answer);
    std::string expectedClean = this->_wordCleaner.clean(expected);

    bool isCorrect = answerClean == expectedClean;

        co_return isCorrect;
}