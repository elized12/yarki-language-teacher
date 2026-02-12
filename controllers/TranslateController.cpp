#include "controllers/TranslateController.hpp"

drogon::Task<HttpResponsePtr> TranslateController::addTranslate(HttpRequestPtr request)
{
    const std::string accessToken = request->getHeader("Authorization");
    models::id userId = std::stoull(this->_authService.getPayload(accessToken)["userId"].get<std::string>());

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (!requestBody || !requestBody->isMember("firstWord") ||
        !requestBody->isMember("secondWord") || !requestBody->isMember("firstCode") ||
        !requestBody->isMember("secondCode"))
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = "Неверный формат запроса";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }

    try
    {
        dto::TranslateCreation translate;
        translate.userId = userId;
        translate.word.code = requestBody->get("firstCode", "").asString();
        translate.word.content = requestBody->get("firstWord", "").asString();
        translate.translate.code = requestBody->get("secondCode", "").asString();
        translate.translate.content = requestBody->get("secondWord", "").asString();

        models::id translateId = co_await this->_translateService.addTranslate(translate);

        std::optional<std::pair<models::Word, models::Word>> translation =
            co_await this->_translateService.get(translateId);

        Json::Value answerBody;
        answerBody["status"] = true;
        answerBody["message"] = "Перевод успешно создан";
        answerBody["firstWord"]["code"] = translation->first.languageCode;
        answerBody["firstWord"]["content"] = translation->first.content;
        answerBody["firstWord"]["id"] = translation->first.id;
        answerBody["secondWord"]["code"] = translation->second.languageCode;
        answerBody["secondWord"]["content"] = translation->second.content;
        answerBody["secondWord"]["id"] = translation->second.id;

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k201Created);

        co_return response;
    }
    catch (const services::TranslateAlreadyExistException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка валидации:") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k409Conflict);

        co_return response;
    }
    catch (const services::ValidationException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка валидации:") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }
    catch (const std::exception &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка сервера:") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);

        co_return response;
    }
}

drogon::Task<HttpResponsePtr> TranslateController::removeTranslate(HttpRequestPtr request)
{
    const std::string accessToken = request->getHeader("Authorization");
    models::id userId = std::stoull(this->_authService.getPayload(accessToken)["userId"].get<std::string>());

    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (!requestBody || !requestBody->isMember("firstWordId") ||
        !requestBody->isMember("secondWordId"))
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = "Неверный формат запроса";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }

    models::id firstWordId = std::stoull(requestBody->get("firstWordId", "").asString());
    models::id secondWordId = std::stoull(requestBody->get("secondWordId", "").asString());

    try
    {
        dto::RemoveTranslate remove;
        remove.firstWordId = firstWordId;
        remove.secondWordId = secondWordId;
        remove.userId = userId;

        bool isDeleted = co_await this->_translateService.removeTranslate(remove);
        if (!isDeleted)
        {
            Json::Value answerBody;
            answerBody["status"] = false;
            answerBody["message"] = "Перевод не найден";

            drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
            response->setStatusCode(drogon::HttpStatusCode::k404NotFound);

            co_return response;
        }

        Json::Value answerBody;
        answerBody["status"] = true;
        answerBody["message"] = "Перевод успешно удален";
        answerBody["translate"]["firstWordId"] = firstWordId;
        answerBody["translate"]["secondWordId"] = secondWordId;

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k200OK);

        co_return response;
    }
    catch (const std::exception &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = "Ошибка сервера";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);

        co_return response;
    }
}

drogon::Task<HttpResponsePtr>
TranslateController::getTranslates(HttpRequestPtr request, models::id wordId)
{
    const std::string accessToken = request->getHeader("Authorization");
    models::id userId = std::stoull(this->_authService.getPayload(accessToken)["userId"].get<std::string>());

    try
    {
        std::vector<models::Word> translates =
            co_await this->_translateService.getTranslates(wordId, userId);

        nlohmann::json answerBody;
        answerBody["status"] = true;
        answerBody["message"] = "Успешно получены переводы для слова";

        for (const models::Word &word : translates)
        {
            answerBody["words"].push_back(
                {{"word", word.content},
                 {"id", std::to_string(word.id)},
                 {"code", word.languageCode}});
        }

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse(
            drogon::HttpStatusCode::k200OK,
            drogon::ContentType::CT_APPLICATION_JSON);
        response->setBody(answerBody.dump());

        co_return response;
    }
    catch (const services::ValidationException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка валидации: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }
    catch (const std::exception &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = "Ошибка сервера";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);

        co_return response;
    }
}

TranslateController::TranslateController()
    : _translateService(
          services::TranslateService(
              repositories::WordRepository(drogon::app().getDbClient()),
              repositories::TranslateRepository(drogon::app().getDbClient()),
              services::WordCleaner())),
      _authService(
          services::AuthService(
              repositories::UserRepository(drogon::app().getDbClient()),
              services::JwtService(
                  drogon::app().getCustomConfig()["secret_key"].asString(),
                  std::chrono::hours(
                      drogon::app()
                          .getCustomConfig()["refresh_token_validity_duraction"]
                          .asInt()),
                  std::chrono::minutes(
                      drogon::app()
                          .getCustomConfig()["access_token_validity_duraction"]
                          .asInt()),
                  repositories::JwtTokenRepository(drogon::app().getDbClient())),
              dto::Validator::getInstance()))
{
}
