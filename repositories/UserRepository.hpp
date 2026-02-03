#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include <optional>

#include "models/User.hpp"
#include "repositories/ResultEmptyException.hpp"

namespace repositories
{

class UserRepository
{
  private:
    drogon::orm::DbClientPtr _db;

  public:
    UserRepository(drogon::orm::DbClientPtr db);

  public:
    drogon::Task<std::optional<models::User>> get(models::id id);
    drogon::Task<std::optional<models::User>>
    getByCredentials(const std::string& email, const std::string& hashedPassword);
    drogon::Task<models::id> create(const models::User& user);
    drogon::Task<bool> remove(models::id id);
    drogon::Task<bool> update(const models::User& user);
    drogon::Task<std::optional<models::User>> getByEmail(const std::string& email);
};

} // namespace repositories