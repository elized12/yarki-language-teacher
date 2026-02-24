#include "repositories/TrainCardRepository.hpp"

using namespace repositories;

TrainCardRepository::TrainCardRepository(drogon::orm::DbClientPtr db) : _db(db)
{
}

drogon::Task<models::id> TrainCardRepository::create(const models::TrainCard &card)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            INSERT INTO train_card (train_session_id, train_card_mode_id, position, other_params, source_word_id)
            VALUES ($1, $2, $3, $4::jsonb, $5) RETURNING id;
        )",
        card.trainSessionId,
        static_cast<int>(card.trainCardModeId),
        static_cast<int>(card.position),
        card.params,
        static_cast<int>(card.sourceWordId));

    if (result.empty())
    {
        throw ResultEmptyException("not returned id");
    }

    co_return result[0]["id"].as<models::id>();
}

drogon::Task<bool> TrainCardRepository::remove(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            DELETE FROM train_card WHERE id = $1
        )",
        static_cast<int>(id));
    co_return result.affectedRows() != 0;
}

drogon::Task<bool> TrainCardRepository::update(const models::TrainCard &card)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            UPDATE train_card SET
                train_session_id = $1,
                position = $2,
                other_params = $3::jsonb,
                train_card_mode_id = $4,
                source_word_id = $5
            WHERE id = $6
        )",
        card.trainSessionId,
        card.position,
        card.params,
        card.trainCardModeId,
        static_cast<int>(card.sourceWordId),
        static_cast<int>(card.id));
    co_return result.affectedRows() != 0;
}

drogon::Task<std::optional<models::TrainCard>> TrainCardRepository::get(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            SELECT 
                id, 
                train_session_id,
                train_card_mode_id,
                created_at,
                position,
                other_params,
                source_word_id
            FROM train_card 
                WHERE id = $1)",
        static_cast<int>(id));
    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::TrainCard card;
    card.createdAt = models::stringToTimePoint(result[0]["created_at"].as<std::string>());
    card.id = result[0]["id"].as<models::id>();
    card.position = result[0]["position"].as<int>();
    card.trainCardModeId = result[0]["train_card_mode_id"].as<models::id>();
    card.trainSessionId = result[0]["train_session_id"].as<std::string>();
    card.params = result[0]["other_params"].as<std::string>();
    card.sourceWordId = result[0]["source_word_id"].as<models::id>();

    co_return card;
}

drogon::Task<std::optional<models::TrainCard>> TrainCardRepository::getCurrentCard(const std::string &sessionId)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            SELECT 
                train_card.id as trainCardId,
                train_card.train_session_id as trainCardSessionId,
                train_card.train_card_mode_id as trainCardModeId,
                train_card.created_at as trainCardCreatedAt,
                train_card.position as trainCardPosition,
                train_card.other_params as trainCardParams,
                train_card.source_word_id as trainCardSourceWordId
            FROM train_card
            LEFT JOIN train_user_answer ON train_user_answer.train_card_id=train_card.id
            WHERE (train_card.train_session_id = $1) AND (train_user_answer.id IS NULL)
            ORDER BY train_card.position ASC
            LIMIT 1
        )",
        sessionId);

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::TrainCard card;
    card.id = result[0]["trainCardId"].as<models::id>();
    card.trainSessionId = result[0]["trainCardSessionId"].as<std::string>();
    card.position = result[0]["trainCardPosition"].as<int>();
    card.trainCardModeId = result[0]["trainCardModeId"].as<int>();
    card.params = result[0]["trainCardParams"].as<std::string>();
    card.createdAt = models::stringToTimePoint(result[0]["trainCardCreatedAt"].as<std::string>());
    card.sourceWordId = result[0]["trainCardSourceWordId"].as<models::id>();

    co_return card;
}

drogon::Task<std::optional<int>> TrainCardRepository::getLastPosition(const std::string &sessionId)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            SELECT 
                MAX(COALESCE(train_card.position, 0)) as trainCardPosition
            FROM train_card
                WHERE (train_card.train_session_id = $1)
            LIMIT 1
        )",
        sessionId);

    if (result.empty())
    {
        co_return std::nullopt;
    }

    co_return result[0]["trainCardPosition"].as<int>();
}

drogon::Task<std::vector<models::TrainCard>> TrainCardRepository::getLastCards(const std::string &sessionId, ssize_t count)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            SELECT 
                id, 
                train_session_id,
                train_card_mode_id,
                created_at,
                position,
                other_params,
                source_word_id
            FROM train_card 
            WHERE train_session_id = $1
            ORDER BY position DESC
            LIMIT $2
        )",
        sessionId,
        static_cast<long long>(count));

    std::vector<models::TrainCard> cards;

    for (const auto &row : result)
    {
        models::TrainCard card;
        card.id = row["id"].as<models::id>();
        card.trainSessionId = row["train_session_id"].as<std::string>();
        card.trainCardModeId = row["train_card_mode_id"].as<models::id>();
        card.createdAt = models::stringToTimePoint(row["created_at"].as<std::string>());
        card.position = row["position"].as<int>();
        card.params = row["other_params"].as<std::string>();
        card.sourceWordId = row["source_word_id"].as<models::id>();

        cards.push_back(card);
    }

    co_return cards;
}
