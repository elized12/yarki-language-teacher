#include "repositories/JwtTokenRepository.hpp"

#include <iostream>

using namespace repositories;

JwtTokenRepository::JwtTokenRepository(drogon::orm::DbClientPtr db) : _db(db)
{
}

drogon::Task<models::id> JwtTokenRepository::create(const models::JwtToken& token)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
        INSERT INTO token (user_id, hashed_token, issued_at, expired_at)
        VALUES ($1, $2, $3, $4) RETURNING id;
    )",
            static_cast<int>(token.userId),
            token.hashedToken,
            models::timePointToString(token.issuedAt),
            models::timePointToString(token.expiredAt)
    );

    if (result.empty())
    {
        throw ResultEmptyException("not returned id");
    }

    co_return result[0]["id"].as<models::id>();
}

drogon::Task<bool> JwtTokenRepository::remove(models::id id)
{
    drogon::orm::Result result =
            co_await this->_db->execSqlCoro(R"(DELETE FROM token WHERE id = $1)", static_cast<int>(id));

    co_return result.affectedRows() != 0;
}

drogon::Task<bool> JwtTokenRepository::remove(const std::string& hashedToken)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(DELETE FROM token WHERE hashed_token = $1)", hashedToken
    );

    co_return result.affectedRows() != 0;
}

drogon::Task<std::optional<models::JwtToken>> JwtTokenRepository::get(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            SELECT id, user_id, hashed_token, issued_at, expired_at, revoked_at 
            FROM token WHERE id = $1
        )",
            static_cast<int>(id)
    );

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::JwtToken token;
    token.id = result[0]["id"].as<models::id>();
    token.userId = result[0]["user_id"].as<models::id>();
    token.hashedToken = result[0]["hashed_token"].as<std::string>();
    token.expiredAt = models::stringToTimePoint(result[0]["expired_at"].as<std::string>());
    token.issuedAt = models::stringToTimePoint(result[0]["issued_at"].as<std::string>());
    if (!result[0]["revoked_at"].isNull())
    {
        token.revokedAt = models::stringToTimePoint(result[0]["revoked_at"].as<std::string>());
    }
    else
    {
        token.revokedAt = std::nullopt;
    }

    co_return token;
}

drogon::Task<std::optional<models::JwtToken>>
JwtTokenRepository::getByToken(const std::string& hashedToken)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            SELECT id, user_id, hashed_token, issued_at, expired_at, revoked_at 
            FROM token WHERE hashed_token = $1
        )",
            hashedToken
    );

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::JwtToken token;
    token.id = result[0]["id"].as<models::id>();
    token.userId = result[0]["user_id"].as<models::id>();
    token.hashedToken = result[0]["hashed_token"].as<std::string>();
    token.expiredAt = models::stringToTimePoint(result[0]["expired_at"].as<std::string>());
    token.issuedAt = models::stringToTimePoint(result[0]["issued_at"].as<std::string>());
    if (!result[0]["revoked_at"].isNull())
    {
        token.revokedAt = models::stringToTimePoint(result[0]["revoked_at"].as<std::string>());
    }
    else
    {
        token.revokedAt = std::nullopt;
    }

    co_return token;
}

drogon::Task<bool>
JwtTokenRepository::revoke(const std::string& hashedToken, models::timePoint timeRevoke)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            UPDATE token SET revoked_at = $1 WHERE hashed_token = $2
        )",
            models::timePointToString(timeRevoke),
            hashedToken
    );

    co_return result.affectedRows() != 0;
}

drogon::Task<bool> JwtTokenRepository::revoke(models::id id, models::timePoint timeRevoke)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            UPDATE token SET revoked_at = $1 WHERE id = $2
        )",
            models::timePointToString(timeRevoke),
            static_cast<int>(id)
    );

    co_return result.affectedRows() != 0;
}

drogon::Task<bool>
JwtTokenRepository::revokeByUserId(models::id userId, models::timePoint timeRevoke)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            UPDATE token SET revoked_at = $1 WHERE user_id = $2
        )",
            models::timePointToString(timeRevoke),
            static_cast<int>(userId)
    );

    co_return result.affectedRows() != 0;
}