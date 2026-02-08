#pragma once

#include <unicode/regex.h>
#include <unicode/unistr.h>

#include <algorithm>
#include <regex>

#include "dto/RemoveTranslate.hpp"
#include "dto/TranslateCreation.hpp"
#include "models/LanguageCode.hpp"
#include "repositories/TranslateRepository.hpp"
#include "repositories/WordRepository.hpp"
#include "services/ValidataionException.hpp"
#include "services/WordCleaner.hpp"
#include "services/TranslateAlreadyExistException.hpp"

namespace services
{

  class TranslateService
  {
  private:
    repositories::WordRepository _wordRepository;
    repositories::TranslateRepository _translateRepository;
    services::WordCleaner _wordCleaner;

  public:
    TranslateService(
        repositories::WordRepository wordRepository,
        repositories::TranslateRepository translateRepository,
        services::WordCleaner cleaner);

  public:
    drogon::Task<models::id> addTranslate(dto::TranslateCreation translate);
    drogon::Task<bool> removeTranslate(const dto::RemoveTranslate &translate);
    drogon::Task<void> updateTranslate(const models::Translate &translate);
    drogon::Task<std::vector<models::Word>> getTranslates(models::id wordId, models::id userId);
    drogon::Task<std::optional<std::pair<models::Word, models::Word>>> get(models::id translateId);
    drogon::Task<bool> existTranslate(models::id userId, models::id firstWordId, models::id secondWordId);
  };

} // namespace services