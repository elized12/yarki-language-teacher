#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include <optional>

#include "models/LanguageCode.hpp"
#include "models/Translate.hpp"
#include "repositories/ResultEmptyException.hpp"

namespace repositories
{

  class TranslateRepository
  {
  private:
    drogon::orm::DbClientPtr _db;

  public:
    TranslateRepository(drogon::orm::DbClientPtr db);

  public:
    drogon::Task<std::optional<models::Translate>> get(models::id id);
    drogon::Task<std::optional<models::Translate>> getByContent(
        const std::string &content, const models::LanguageCode::Code &code, models::id userId);
    drogon::Task<std::optional<models::Translate>> get(models::id userId, models::id firstWordId, models::id secondWordId);

    drogon::Task<models::id> create(const models::Translate &translate);
    drogon::Task<bool> remove(models::id firstWordId, models::id secondWordId, models::id userId);
    drogon::Task<bool> update(const models::Translate &user);
    drogon::Task<std::vector<models::Translate>> getTranslates(
        models::id userId,
        const models::LanguageCode::Code &sourceLanguage,
        const models::LanguageCode::Code &targetLanguage,
        const std::vector<models::id> &ignoredIds,
        ssize_t limit);

    drogon::Task<bool> isTranslateExist(
        models::id firstWordId,
        std::string secondContentWord,
        const models::LanguageCode::Code &firstWordCode,
        models::id userId);

    drogon::Task<ssize_t> getCount(
        const models::LanguageCode::Code &sourceLanguage,
        const models::LanguageCode::Code &targetLanguage,
        models::id userId);
  };

} // namespace repositories