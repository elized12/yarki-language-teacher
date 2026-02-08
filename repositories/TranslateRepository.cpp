#include "repositories/TranslateRepository.hpp"

using namespace repositories;

TranslateRepository::TranslateRepository(drogon::orm::DbClientPtr db) : _db(db)
{
}

drogon::Task<std::optional<models::Translate>> TranslateRepository::get(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
        SELECT word_a_id, word_b_id, user_id, created_at FROM translation WHERE id = $1
    )",
        static_cast<int>(id));

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::Translate translate;
    translate.firstWordId = result[0]["word_a_id"].as<models::id>();
    translate.secondWordId = result[0]["word_b_id"].as<models::id>();
    translate.createdAt = models::stringToTimePoint(result[0]["created_at"].as<std::string>());
    translate.userId = result[0]["user_id"].as<models::id>();

    co_return translate;
}

drogon::Task<std::optional<models::Translate>> TranslateRepository::getByContent(
    const std::string &content, const models::LanguageCode::Code &code, models::id userId)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
        SELECT translation.id as translationId,
        translation.word_a_id as wordAId,
        translation.word_b_id as wordBId,
        translation.user_id as userId,
        translation.created_at as createdAt 
        FROM translation
        INNER JOIN word wa ON wa.id = translation.word_a_id
        INNER JOIN word wb ON wb.id = translation.word_b_id
        WHERE ((wa.content = $1 and wa.language_id = $2) OR (wb.content = $1 and wb.language_id = $2)) AND translation.user_id = $3
    )",
        content,
        static_cast<int>(code),
        static_cast<int>(userId));

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::Translate translate;
    translate.id = result[0]["translationId"].as<models::id>();
    translate.firstWordId = result[0]["wordAId"].as<models::id>();
    translate.secondWordId = result[0]["wordBId"].as<models::id>();
    translate.createdAt =
        models::stringToTimePoint(result[0]["translation.createdAt"].as<std::string>());
    translate.userId = result[0]["userId"].as<models::id>();

    co_return translate;
}

drogon::Task<models::id> TranslateRepository::create(const models::Translate &translate)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
            INSERT INTO translation (word_a_id, word_b_id, user_id, created_at)
            VALUES ($1, $2, $3, $4) RETURNING id;
            )",
        static_cast<int>(translate.firstWordId),
        static_cast<int>(translate.secondWordId),
        static_cast<int>(translate.userId),
        models::timePointToString(translate.createdAt));

    if (result.empty())
    {
        throw ResultEmptyException("not returned id");
    }

    co_return result[0]["id"].as<models::id>();
}

drogon::Task<bool>
TranslateRepository::remove(models::id firstWordId, models::id secondWordId, models::id userId)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(DELETE FROM translation WHERE ((word_a_id = $1 AND word_b_id = $2) OR (word_a_id = $2 AND word_b_id = $1)) AND user_id = $3)",
        static_cast<int>(std::min(firstWordId, secondWordId)),
        static_cast<int>(std::max(firstWordId, secondWordId)),
        static_cast<int>(userId));
    co_return result.affectedRows() != 0;
}

drogon::Task<bool> TranslateRepository::update(const models::Translate &translate)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(UPDATE translation SET word_a_id = $1, word_b_id = $2, user_id = $3 WHERE id = $4)",
        static_cast<int>(std::min(translate.firstWordId, translate.secondWordId)),
        static_cast<int>(std::max(translate.firstWordId, translate.secondWordId)),
        static_cast<int>(translate.userId),
        static_cast<int>(translate.id));
    co_return result.affectedRows() != 0;
}

drogon::Task<std::optional<models::Translate>> TranslateRepository::get(models::id userId, models::id firstWordId, models::id secondWordId)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
        R"(
        SELECT word_a_id, word_b_id, user_id, created_at FROM translation 
        WHERE ((word_a_id = $1 AND word_b_id = $2) OR (word_a_id = $2 AND word_b_id = $1)) AND user_id = $3
    )",
        static_cast<int>(firstWordId),
        static_cast<int>(secondWordId),
        static_cast<int>(userId));

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::Translate translate;
    translate.firstWordId = result[0]["word_a_id"].as<models::id>();
    translate.secondWordId = result[0]["word_b_id"].as<models::id>();
    translate.createdAt = models::stringToTimePoint(result[0]["created_at"].as<std::string>());
    translate.userId = result[0]["user_id"].as<models::id>();

    co_return translate;
}
