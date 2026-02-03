#include "controllers/WordController.hpp"

drogon::Task<HttpResponsePtr> WordController::getWords(
        HttpRequestPtr request,
        models::id userId,
        std::string&& languageCode,
        ssize_t offset,
        ssize_t limit
)
{
    if (WordController::MAX_LIMIT < limit)
    {
        throw services::ValidationException("Лимит не может быть больше 5000");
    }

    try
    {
        std::vector<models::Word> words =
                co_await this->_wordService.get(userId, languageCode, limit, offset);

        nlohmann::json answerBody;
        answerBody["status"] = true;
        answerBody["message"] = "Успешно";
        answerBody["count"] = words.size();

        for (const models::Word& word : words)
        {
            answerBody["words"].push_back(
                    {{"word", word.content},
                     {"id", std::to_string(word.id)},
                     {"code", word.languageCode}}
            );
        }

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse(
                drogon::HttpStatusCode::k200OK, drogon::ContentType::CT_APPLICATION_JSON
        );
        response->setBody(answerBody.dump());

        co_return response;
    }
    catch (const services::ValidationException& ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка валидации: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }
    catch (const std::exception& ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = "ошибка сервера";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);

        co_return response;
    }
}

drogon::Task<HttpResponsePtr>
WordController::getCount(HttpRequestPtr request, models::id userId, std::string&& code)
{
    try
    {
        ssize_t count = co_await this->_wordService.getCountWord(userId, code);

        Json::Value answerBody;
        answerBody["status"] = true;
        answerBody["message"] = "Успешно";
        answerBody["count"] = count;

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k200OK);

        co_return response;
    }
    catch (const services::ValidationException& ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка валидации: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }
    catch (const std::exception& ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = "ошибка сервера";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);

        co_return response;
    }
}

WordController::WordController()
    : _wordService(services::WordService(repositories::WordRepository(drogon::app().getDbClient())))
{
}