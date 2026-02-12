#include "repositories/TrainUserAnswerRepository.hpp"

using namespace repositories;

TrainUserAnswerRepository::TrainUserAnswerRepository(drogon::orm::DbClientPtr db) : _db(db)
{
}

drogon::Task<std::string> TrainUserAnswerRepository::create(const models::UserAnswer &answer)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            INSERT INTO train_user_answer (train_card_id, content, is_correct, created_at)
            VALUES ($1, $2, $3, $4) RETURNING id;
        )",
        static_cast<int>(answer.trainCardId),
        answer.content,
        answer.isCorrect,
        models::timePointToString(answer.createdAt));

    if (result.empty())
    {
        throw ResultEmptyException("not returned id");
    }

    co_return std::to_string(result[0]["id"].as<models::id>());
}

drogon::Task<bool> TrainUserAnswerRepository::remove(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            DELETE FROM train_user_answer WHERE id = $1
        )",
        static_cast<int>(id));
    co_return result.affectedRows() != 0;
}

drogon::Task<bool> TrainUserAnswerRepository::update(const models::UserAnswer &answer)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            UPDATE train_user_answer SET
                train_card_id = $1,
                content = $2,
                is_correct = $3
            WHERE id = $4
        )",
        static_cast<int>(answer.trainCardId),
        answer.content,
        answer.isCorrect,
        static_cast<int>(answer.id));
    co_return result.affectedRows() != 0;
}

drogon::Task<std::optional<models::UserAnswer>> TrainUserAnswerRepository::get(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            SELECT 
                id, 
                train_card_id,
                content,
                is_correct,
                created_at
            FROM train_user_answer 
            WHERE id = $1
        )",
        static_cast<int>(id));

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::UserAnswer answer;
    answer.id = result[0]["id"].as<models::id>();
    answer.trainCardId = result[0]["train_card_id"].as<models::id>();
    answer.content = result[0]["content"].as<std::string>();
    answer.isCorrect = result[0]["is_correct"].as<bool>();
    answer.createdAt = models::stringToTimePoint(result[0]["created_at"].as<std::string>());

    co_return answer;
}

drogon::Task<std::vector<models::UserAnswer>> TrainUserAnswerRepository::getByTrainCardId(models::id trainCardId)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            SELECT 
                id, 
                train_card_id,
                content,
                is_correct,
                created_at
            FROM train_user_answer 
            WHERE train_card_id = $1
            ORDER BY created_at ASC
        )",
        static_cast<int>(trainCardId));

    std::vector<models::UserAnswer> answers;

    for (const auto &row : result)
    {
        models::UserAnswer answer;
        answer.id = row["id"].as<models::id>();
        answer.trainCardId = row["train_card_id"].as<models::id>();
        answer.content = row["content"].as<std::string>();
        answer.isCorrect = row["is_correct"].as<bool>();
        answer.createdAt = models::stringToTimePoint(row["created_at"].as<std::string>());

        answers.push_back(answer);
    }

    co_return answers;
}
