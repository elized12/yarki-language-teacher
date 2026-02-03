#include "controllers/AuthController.hpp"

#include <iostream>

drogon::Task<HttpResponsePtr> AuthController::signUp(HttpRequestPtr request)
{
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (!requestBody || !requestBody->isMember("email") || !requestBody->isMember("nickname") ||
        !requestBody->isMember("password"))
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = "Неверный формат запроса";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }

    dto::UserRegistration userData;
    userData.email = requestBody->get("email", "").asString();
    userData.nickname = requestBody->get("nickname", "").asString();
    userData.password = requestBody->get("password", "").asString();

    try
    {
        models::id userId = co_await this->_authService.registerUser(userData);

        Json::Value responseBody;
        responseBody["status"] = true;
        responseBody["message"] = "Регистрация успешна!";

        auto response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(k201Created);

        co_return response;
    }
    catch (const services::auth::UserAlreadyExistException& ex)
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = "Пользователь с такой почтой уже существует!";

        auto response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(k409Conflict);

        co_return response;
    }
    catch (const services::ValidationException& ex)
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = std::string("Ошибка валидации: ") + ex.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(k400BadRequest);

        co_return response;
    }
    catch (const std::exception& ex)
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = "Ошибка сервера";

        auto response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(k500InternalServerError);

        co_return response;
    }
}

drogon::Task<HttpResponsePtr> AuthController::signIn(HttpRequestPtr request)
{
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (!requestBody || !requestBody->isMember("email") || !requestBody->isMember("password"))
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = "Неверный формат запроса";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }

    dto::UserLogin userLogin;
    userLogin.email = requestBody->get("email", "").asString();
    userLogin.password = requestBody->get("password", "").asString();

    try
    {
        std::pair<std::string, std::string> tokens =
                co_await this->_authService.loginUser(userLogin);

        Json::Value responseBody;
        responseBody["status"] = true;
        responseBody["message"] = "Успешно";
        responseBody["access_token"] = tokens.first;
        responseBody["refresh_token"] = tokens.second;

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k200OK);

        co_return response;
    }
    catch (const services::ValidationException& ex)
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = std::string("Ошибка валидации: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }
    catch (const std::exception& ex)
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = std::string("Ошибка сервера: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);

        co_return response;
    }
}

drogon::Task<HttpResponsePtr> AuthController::refresh(HttpRequestPtr request)
{
    std::shared_ptr<Json::Value> requestBody = request->getJsonObject();
    if (!requestBody || !requestBody->isMember("refresh_token"))
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = "Неверный формат запроса";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }

    std::string refreshToken = requestBody->get("refresh_token", "").asString();

    try
    {
        std::string newToken = co_await this->_authService.refresh(refreshToken);

        Json::Value responseBody;
        responseBody["status"] = true;
        responseBody["message"] = "Успешно обновлен access_token";
        responseBody["access_token"] = newToken;

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k200OK);

        co_return response;
    }
    catch (const services::ValidationException& ex)
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = std::string("Ошибка валидации: ") + ex.what();

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);

        co_return response;
    }
    catch (const std::exception& ex)
    {
        Json::Value responseBody;
        responseBody["status"] = false;
        responseBody["message"] = "Ошибка запроса";

        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpJsonResponse(responseBody);
        response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);

        co_return response;
    }
}

AuthController::AuthController()
    : _authService(
              services::AuthService(
                      repositories::UserRepository(drogon::app().getDbClient()),
                      services::JwtService(
                              drogon::app().getCustomConfig()["secret_key"].asString(),
                              std::chrono::hours(
                                      drogon::app()
                                              .getCustomConfig()["refresh_token_validity_duraction"]
                                              .asInt()
                              ),
                              std::chrono::minutes(
                                      drogon::app()
                                              .getCustomConfig()["access_token_validity_duraction"]
                                              .asInt()
                              ),
                              repositories::JwtTokenRepository(drogon::app().getDbClient())
                      ),
                      dto::Validator::getInstance()
              )
      )
{
}
