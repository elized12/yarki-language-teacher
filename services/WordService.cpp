#include "services/WordService.hpp"

using namespace services;

WordService::WordService(repositories::WordRepository wordRepository)
    : _wordRepository(std::move(wordRepository))
{
}

drogon::Task<std::vector<models::Word>>
WordService::get(models::id userId, const std::string& code, ssize_t limit, ssize_t offset)
{
    if (!models::LanguageCode::isLangCodeExist(code))
    {
        throw services::ValidationException("Такого языка не существует");
    }

    co_return co_await this->_wordRepository.getByUser(
            userId, models::LanguageCode::toCode(code), limit, offset
    );
}

drogon::Task<ssize_t> WordService::getCountWord(models::id userId, const std::string& code)
{
    if (!models::LanguageCode::isLangCodeExist(code))
    {
        throw services::ValidationException("Такого языка не существует");
    }

    co_return co_await this->_wordRepository.getCountWord(
            userId, models::LanguageCode::toCode(code)
    );
}
