#pragma once

#include <algorithm>
#include <regex>

#include "dto/RemoveTranslate.hpp"
#include "dto/TranslateCreation.hpp"
#include "models/LanguageCode.hpp"
#include "repositories/TranslateRepository.hpp"
#include "repositories/WordRepository.hpp"
#include "services/ValidataionException.hpp"
#include "utils/StringHelper.hpp"

namespace services
{

class TranslateService
{
  private:
    repositories::WordRepository _wordRepository;
    repositories::TranslateRepository _translateRepository;

  public:
    TranslateService(
            repositories::WordRepository wordRepository,
            repositories::TranslateRepository translateRepository
    );

  public:
    drogon::Task<models::id> addTranslate(dto::TranslateCreation translate);
    drogon::Task<void> removeTranslate(const dto::RemoveTranslate& translate);
    drogon::Task<void> updateTranslate(const models::Translate& translate);
    drogon::Task<std::vector<models::Word>> getTranslates(models::id wordId, models::id userId);
    drogon::Task<std::optional<std::pair<models::Word, models::Word>>> get(models::id translateId);
};

} // namespace services