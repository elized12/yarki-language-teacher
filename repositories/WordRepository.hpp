#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include <optional>

#include "models/LanguageCode.hpp"
#include "models/Word.hpp"
#include "repositories/ResultEmptyException.hpp"

namespace repositories
{

class WordRepository
{
  private:
    drogon::orm::DbClientPtr _db;

  public:
    WordRepository(drogon::orm::DbClientPtr db);

  public:
    drogon::Task<std::optional<models::Word>> get(models::id id);
    drogon::Task<std::optional<models::Word>> getByContent(const std::string& content);
    drogon::Task<std::vector<models::Word>> getByUser(
            models::id userId, const models::LanguageCode::Code& code, ssize_t limit, ssize_t offset
    );

    drogon::Task<ssize_t> getCountWord(models::id userId, const models::LanguageCode::Code& code);

    drogon::Task<models::id> create(const models::Word& word);
    drogon::Task<bool> remove(models::id id);
    drogon::Task<bool> update(const models::Word& user);

    drogon::Task<std::vector<models::Word>> getTranslates(models::id wordId, models::id userId);
};

} // namespace repositories