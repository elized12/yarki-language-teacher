#pragma once

#include <drogon/orm/DbClient.h>

#include "models/JwtToken.hpp"
#include "repositories/ResultEmptyException.hpp"

namespace repositories
{

class JwtTokenRepository
{
  private:
    drogon::orm::DbClientPtr _db;

  public:
    JwtTokenRepository(drogon::orm::DbClientPtr db);

  public:
    drogon::Task<models::id> create(const models::JwtToken& token);
    drogon::Task<bool> remove(models::id id);
    drogon::Task<bool> remove(const std::string& hashedToken);
    drogon::Task<std::optional<models::JwtToken>> get(models::id);
    drogon::Task<std::optional<models::JwtToken>> getByToken(const std::string& hashedToken);
    drogon::Task<bool> revoke(const std::string& hashedToken, models::timePoint timeRevoke);
    drogon::Task<bool> revoke(models::id id, models::timePoint timeRevoke);
    drogon::Task<bool> revokeByUserId(models::id userId, models::timePoint timeRevoke);
};

} // namespace repositories