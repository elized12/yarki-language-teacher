#include "repositories/WordRepository.hpp"

using namespace repositories;

WordRepository::WordRepository(drogon::orm::DbClientPtr db) : _db(db)
{
}

drogon::Task<std::optional<models::Word>> WordRepository::get(models::id id)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            SELECT word.id, word.content, language.code FROM word
            LEFT JOIN "language" ON language.id = word.language_id  
            WHERE word.id = $1
        )",
            static_cast<int>(id)
    );

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::Word word;
    word.content = result[0]["word.content"].as<std::string>();
    word.id = result[0]["word.id"].as<models::id>();
    word.languageCode = result[0]["language.code"].as<std::string>();

    co_return word;
}

drogon::Task<models::id> WordRepository::create(const models::Word& word)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            INSERT INTO word (language_id, content)
            SELECT id, $2 FROM "language" WHERE code = $1
            RETURNING id;
            )",
            word.languageCode,
            word.content
    );

    if (result.empty())
    {
        throw ResultEmptyException("not returned id");
    }

    co_return result[0]["id"].as<models::id>();
}

drogon::Task<std::optional<models::Word>> WordRepository::getByContent(const std::string& content)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            SELECT word.id, word.content, language.code FROM word
            LEFT JOIN "language" ON language.id = word.language_id  
            WHERE word.content = $1
        )",
            content
    );

    if (result.empty())
    {
        co_return std::nullopt;
    }

    models::Word word;
    word.content = result[0]["word.content"].as<std::string>();
    word.id = result[0]["word.id"].as<models::id>();
    word.languageCode = result[0]["language.code"].as<std::string>();

    co_return word;
}

drogon::Task<bool> WordRepository::remove(models::id id)
{
    drogon::orm::Result result =
            co_await this->_db->execSqlCoro(R"(DELETE FROM word WHERE id = $1)", static_cast<int>(id));
    co_return result.affectedRows() != 0;
}

drogon::Task<bool> WordRepository::update(const models::Word& word)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            UPDATE word SET language_id = (SELECT id FROM "language" WHERE code = $2), 
            content = $3 WHERE id = $1
            )",
            static_cast<int>(word.id),
            word.languageCode,
            word.content
    );
    co_return result.affectedRows() != 0;
}

drogon::Task<std::vector<models::Word>>
WordRepository::getTranslates(models::id wordId, models::id userId)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            SELECT translation.word_a_id,
                translation.word_b_id, translation.user_id, translation.created_at,
                wa.content, wa.language_id, wb.content, wb.language_id
            FROM translation
            INNER JOIN word wa ON wa.id = translation.word_a_id
            INNER JOIN word wb ON wb.id = translation.word_b_id
            WHERE (translation.word_a_id = $1 OR translation.word_b_id = $1) AND translation.user_id = $2
        )",
            wordId,
            userId
    );

    if (result.empty())
    {
        co_return {};
    }

    std::vector<models::Word> words;
    for (auto& row : result)
    {
        models::id wordAId = std::stoull(row["translation.word_a_id"].as<std::string>());
        models::id wordBId = std::stoull(row["translation.word_b_id"].as<std::string>());

        models::Word word;

        if (wordAId != wordId)
        {
            word.content = row["wa.content"].as<std::string>();
            word.languageCode = models::LanguageCode::toString(
                    static_cast<models::LanguageCode::Code>(
                            std::stoi(row["wa.language_id"].as<std::string>())
                    )
            );
        }
        else
        {
            word.content = row["wb.content"].as<std::string>();
            word.languageCode = models::LanguageCode::toString(
                    static_cast<models::LanguageCode::Code>(
                            std::stoi(row["wb.language_id"].as<std::string>())
                    )
            );
        }

        words.push_back(std::move(word));
    }

    co_return words;
}

drogon::Task<std::vector<models::Word>> WordRepository::getByUser(
        models::id userId, const models::LanguageCode::Code& code, ssize_t limit, ssize_t offset
)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            SELECT translation.word_a_id,
                translation.word_b_id, translation.user_id, translation.created_at,
                wa.content, wa.language_id, wb.content, wb.language_id, wa.id, wb.id
            FROM translation
            INNER JOIN word wa ON wa.id = translation.word_a_id
            INNER JOIN word wb ON wb.id = translation.word_b_id
            WHERE (wa.language_id = $2 OR wb.language_id = $2) AND translation.user_id = $1
            ORDER BY translation.created_at
            LIMIT $3
            OFFSET $4
        )",
            userId,
            static_cast<int>(code),
            limit,
            offset
    );

    if (result.empty())
    {
        co_return {};
    }

    std::vector<models::Word> words;
    for (const auto& row : result)
    {
        models::LanguageCode::Code codeWordA = static_cast<models::LanguageCode::Code>(
                std::stoull(row["wa.language_id"].as<std::string>())
        );

        models::LanguageCode::Code codeWordB = static_cast<models::LanguageCode::Code>(
                std::stoull(row["wb.language_id"].as<std::string>())
        );

        if (codeWordA == code)
        {
            models::Word word;
            word.content = row["wa.content"].as<std::string>();
            word.languageCode = models::LanguageCode::toString(
                    static_cast<models::LanguageCode::Code>(
                            std::stoull(row["wa.language_id"].as<std::string>())
                    )
            );
            word.id = std::stoull(row["wa.id"].as<std::string>());

            words.push_back(std::move(word));
        }
        else
        {
            models::Word word;
            word.content = row["wb.content"].as<std::string>();
            word.languageCode = models::LanguageCode::toString(
                    static_cast<models::LanguageCode::Code>(
                            std::stoull(row["wb.language_id"].as<std::string>())
                    )
            );

            word.id = std::stoull(row["wb.id"].as<std::string>());

            words.push_back(std::move(word));
        }
    }

    co_return words;
}

drogon::Task<ssize_t>
WordRepository::getCountWord(models::id userId, const models::LanguageCode::Code& code)
{
    drogon::orm::Result result = co_await this->_db->execSqlCoro(
            R"(
            select count(distinct case when (wa.language_id = 1) then wa.id when(wb.language_id = 1) then wb.id end) as count from "translation" t
                inner join word wa on t.word_a_id = wa.id
                inner join word wb on t.word_b_id = wb.id 
                where (wa.language_id = 1 or wb.language_id = 1) and user_id = 1
        )",
            static_cast<int>(code),
            userId
    );

    if (result.empty())
    {
        throw repositories::ResultEmptyException("returning empty result");
    }

    co_return std::stoull(result[0]["count"].as<std::string>());
}
