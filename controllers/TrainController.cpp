#include "controllers/TrainController.hpp"

using namespace drogon;

TrainController::TrainController()
    : _trainService(
          services::TrainService(
              repositories::TrainSessionRepository(drogon::app().getDbClient(), dto::TrainSettingsConverter()),
              repositories::TrainCardRepository(drogon::app().getDbClient()),
              repositories::WordRepository(drogon::app().getDbClient()),
              repositories::TranslateRepository(drogon::app().getDbClient()),
              repositories::TrainUserAnswerRepository(drogon::app().getDbClient()))),
      _authService(services::AuthService(
          repositories::UserRepository(drogon::app().getDbClient()),
          services::JwtService(
              drogon::app().getCustomConfig()["secret_key"].asString(),
              std::chrono::hours(drogon::app().getCustomConfig()["refresh_token_validity_duraction"].asInt()),
              std::chrono::minutes(drogon::app().getCustomConfig()["access_token_validity_duraction"].asInt()),
              repositories::JwtTokenRepository(drogon::app().getDbClient())),
          dto::Validator::getInstance()))
{
}

drogon::Task<HttpResponsePtr> TrainController::startTrainSession(HttpRequestPtr request)
{
    try
    {
        const std::string accessToken = request->getHeader("Authorization");
        models::id userId = std::stoull(this->_authService.getPayload(accessToken)["userId"].get<std::string>());

        std::shared_ptr<Json::Value> body = request->getJsonObject();
        if (!body)
        {
            Json::Value answerBody;
            answerBody["status"] = false;
            answerBody["message"] = "Неверный формат запроса";

            drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
            response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
            co_return response;
        }

        if (!models::LanguageCode::isLangCodeExist(body->get("source_language", "").asString()) ||
            !models::LanguageCode::isLangCodeExist(body->get("target_language", "").asString()))
        {
            Json::Value answerBody;
            answerBody["status"] = false;
            answerBody["message"] = std::string("Ошибка валидации: неверно указаны языки");

            drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
            response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
            co_return response;
        }

        models::TrainSettings settings;
        settings.sourceLanguage = models::LanguageCode::toCode(body->get("source_language", "").asString());
        settings.targetLanguage = models::LanguageCode::toCode(body->get("target_language", "").asString());
        if (body->isMember("added_after_word") && !body->get("added_after_word", Json::Value()).isNull())
        {
            settings.addedAfterWord = models::stringToTimePoint(body->get("added_after_word", "").asString());
        }

        std::string sessionId = co_await this->_trainService.startSession(userId, settings);

        nlohmann::json answerBody;
        answerBody["status"] = true;
        answerBody["message"] = "Сессия создана";
        answerBody["sessionId"] = sessionId;

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse(drogon::HttpStatusCode::k201Created, drogon::ContentType::CT_APPLICATION_JSON);
        response->setBody(answerBody.dump());
        co_return response;
    }
    catch (const std::exception &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка сервера: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        co_return response;
    }
}

drogon::Task<HttpResponsePtr> TrainController::finishTrainSession(HttpRequestPtr request)
{
    try
    {
        const std::string accessToken = request->getHeader("Authorization");
        models::id userId = std::stoull(this->_authService.getPayload(accessToken)["userId"].get<std::string>());

        std::shared_ptr<Json::Value> body = request->getJsonObject();
        if (!body || !body->isMember("sessionId"))
        {
            Json::Value answerBody;
            answerBody["status"] = false;
            answerBody["message"] = "Неверный формат запроса";

            drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
            response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
            co_return response;
        }

        std::string sessionId = body->get("sessionId", "").asString();
        if (!::utils::string::isValidUuid(sessionId))
        {
            Json::Value answerBody;
            answerBody["status"] = false;
            answerBody["message"] = std::string("Такой сессии не сущесвует");

            drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
            response->setStatusCode(drogon::HttpStatusCode::k404NotFound);
            co_return response;
        }

        bool finished = co_await this->_trainService.finishSession(userId, sessionId);

        nlohmann::json answerBody;
        answerBody["status"] = finished;
        answerBody["message"] = finished ? "Сессия завершена" : "Не удалось завершить сессию";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse(drogon::HttpStatusCode::k200OK, drogon::ContentType::CT_APPLICATION_JSON);
        response->setBody(answerBody.dump());
        co_return response;
    }
    catch (const services::AccessDeniedException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("нету доступа");

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k403Forbidden);
        co_return response;
    }
    catch (const services::ValidationException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка валидации сессия уже закрыта:") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
        co_return response;
    }
    catch (const std::exception &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка сервера") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        co_return response;
    }
}

drogon::Task<HttpResponsePtr> TrainController::getTask(HttpRequestPtr request, std::string &&sessionId)
{
    try
    {
        const std::string accessToken = request->getHeader("Authorization");
        models::id userId = std::stoull(this->_authService.getPayload(accessToken)["userId"].get<std::string>());

        models::TrainCard card = co_await this->_trainService.getTask(userId, sessionId);

        nlohmann::json answerBody;
        answerBody["status"] = true;
        answerBody["message"] = "Успешно";
        answerBody["card"] = {
            {"id", std::to_string(card.id)},
            {"position", card.position},
            {"params", nlohmann::json::parse(card.params)},
            {"trainCardModeId", std::to_string(card.trainCardModeId)},
            {"sourceWordId", std::to_string(card.sourceWordId)},
            {"createdAt", models::timePointToString(card.createdAt)}};

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse(drogon::HttpStatusCode::k200OK, drogon::ContentType::CT_APPLICATION_JSON);
        response->setBody(answerBody.dump());
        co_return response;
    }
    catch (const services::AccessDeniedException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Нету доступа");

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k403Forbidden);
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
    catch (const services::NotExistException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Не найдено: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k404NotFound);
        co_return response;
    }
    catch (const std::exception &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка сервера") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        co_return response;
    }
}

drogon::Task<HttpResponsePtr> TrainController::sendAnswerTask(HttpRequestPtr request, models::id cardId)
{
    try
    {
        std::shared_ptr<Json::Value> body = request->getJsonObject();
        if (!body || !body->isMember("answer"))
        {
            Json::Value answerBody;
            answerBody["status"] = false;
            answerBody["message"] = "Неверный формат запроса";

            drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
            response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
            co_return response;
        }

        std::string answer = body->get("answer", "").asString();
        bool isCorrect = co_await this->_trainService.answerTask(answer, cardId);

        nlohmann::json answerBody;
        answerBody["status"] = true;
        answerBody["is_correct"] = isCorrect;

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse(drogon::HttpStatusCode::k200OK, drogon::ContentType::CT_APPLICATION_JSON);
        response->setBody(answerBody.dump());
        co_return response;
    }
    catch (const services::NotExistException &ex)
    {
        Json::Value answerBody;
        answerBody["status"] = false;
        answerBody["message"] = std::string("Ошибка валидации: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(answerBody);
        response->setStatusCode(drogon::HttpStatusCode::k404NotFound);
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