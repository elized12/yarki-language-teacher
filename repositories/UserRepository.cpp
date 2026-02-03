#include "repositories/UserRepository.hpp"

using namespace repositories;

UserRepository::UserRepository(drogon::orm::DbClientPtr db) : _db(db)
{
}

drogon::Task<models::id> UserRepository::create(const models::User& user)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            INSERT INTO "user" (email, hashed_password, nickname)
             VALUES ($1, $2, $3) RETURNING id;
            )",
            user.email,
            user.hashedPassword,
            user.nickname
    );

    if (result.empty())
    {
        throw ResultEmptyException("not returned id");
    }

    co_return result[0]["id"].as<models::id>();
}

drogon::Task<std::optional<models::User>> UserRepository::get(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(SELECT id, nickname, hashed_password, email FROM "user" WHERE id = $1)", id
    );
    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::User user;
    user.id = result[0]["id"].as<models::id>();
    user.email = result[0]["email"].as<std::string>();
    user.nickname = result[0]["nickname"].as<std::string>();
    user.hashedPassword = result[0]["hashed_password"].as<std::string>();

    co_return user;
}

drogon::Task<std::optional<models::User>>
UserRepository::getByCredentials(const std::string& email, const std::string& hashedPassword)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(SELECT id, nickname, hashed_password, email FROM "user" WHERE email = $1 AND hashed_password = $2)",
            email,
            hashedPassword
    );
    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::User user;
    user.id = result[0]["id"].as<models::id>();
    user.email = result[0]["email"].as<std::string>();
    user.nickname = result[0]["nickname"].as<std::string>();
    user.hashedPassword = result[0]["hashed_password"].as<std::string>();

    co_return user;
}

drogon::Task<bool> UserRepository::remove(models::id id)
{
    drogon::orm::Result result =
            co_await this->_db->execSqlCoro(R"(DELETE FROM "user" WHERE id = $1)", id);
    co_return result.affectedRows() != 0;
}

drogon::Task<bool> UserRepository::update(const models::User& user)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(UPDATE "user" SET email = $1, hashed_password = $2, nickname = $3 WHERE id = $4)",
            user.email,
            user.hashedPassword,
            user.nickname,
            user.id
    );
    co_return result.affectedRows() != 0;
}
