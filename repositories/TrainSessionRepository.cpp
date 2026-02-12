#include "repositories/TrainSessionRepository.hpp"

using namespace repositories;

TrainSessionRepository::TrainSessionRepository(
    drogon::orm::DbClientPtr db,
    dto::TrainSettingsConverter converter)
    : _db(db),
      _converter(converter)
{
}

drogon::Task<std::string> TrainSessionRepository::create(const models::TrainSession &session)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
        INSERT INTO train_session (user_id, settings) VALUES ($1, $2) RETURNING id;
    )",
        static_cast<int>(session.userId),
        this->_converter.convertSettingsToString(session.settings));

    if (result.empty())
    {
        throw ResultEmptyException("not returned id");
    }

    co_return result[0]["id"].as<std::string>();
}

drogon::Task<bool> TrainSessionRepository::remove(const std::string &uuid)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(R"(DELETE FROM train_session WHERE id = $1)", uuid);
    co_return result.affectedRows() != 0;
}

drogon::Task<bool> TrainSessionRepository::update(const models::TrainSession &session)
{
    std::optional<std::string> finishedAtStr;
    if (session.finishedAt.has_value())
    {
        finishedAtStr = models::timePointToString(session.finishedAt.value());
    }

    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(UPDATE train_session SET user_id = $1, settings = $2, finished_at = $3 WHERE id = $4)",
        static_cast<int>(session.userId),
        this->_converter.convertSettingsToString(session.settings),
        finishedAtStr,
        session.id);

    co_return result.affectedRows() != 0;
}

drogon::Task<std::optional<models::TrainSession>> TrainSessionRepository::get(const std::string &uuid)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            SELECT id, user_id, started_at, finished_at, settings FROM train_session WHERE id = $1
        )",
        uuid);

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::TrainSession session;
    session.id = result[0]["id"].as<std::string>();
    session.userId = result[0]["user_id"].as<models::id>();
    session.settings = this->_converter.convertStringToSettings(result[0]["settings"].as<std::string>());
    session.startedAt = models::stringToTimePoint(result[0]["started_at"].as<std::string>());

    if (result[0]["finished_at"].isNull())
    {
        session.finishedAt = std::nullopt;
    }
    else
    {
        session.finishedAt = models::stringToTimePoint(result[0]["finished_at"].as<std::string>());
    }

    co_return session;
}