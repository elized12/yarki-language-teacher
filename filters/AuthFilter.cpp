#include "AuthFilter.hpp"

using namespace drogon;

void AuthFilter::doFilter(
    const HttpRequestPtr &request, FilterCallback &&callback, FilterChainCallback &&next)
{
    const std::string headerAuth = request->getHeader("Authorization");
    if (headerAuth.empty())
    {
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
        response->setContentTypeCode(drogon::ContentType::CT_APPLICATION_JSON);
        response->addHeader("Access-Control-Allow-Origin", "*");

        nlohmann::json responseBody = {
            {"status", false}, {"message", "Отсутсвует заголовок Authorization"}};

        response->setBody(responseBody.dump());
        callback(response);
        return;
    }

    if (!this->_service.isValidAccessToken(headerAuth))
    {
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
        response->setContentTypeCode(drogon::ContentType::CT_APPLICATION_JSON);
        response->addHeader("Access-Control-Allow-Origin", "*");

        nlohmann::json responseBody = {{"status", false}, {"message", "Невалидный access_token"}};

        response->setBody(responseBody.dump());
        callback(response);
        return;
    }

    next();
}

AuthFilter::AuthFilter()
    : _service(
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
