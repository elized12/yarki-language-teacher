#include "controllers/AuthController.hpp"

void AuthController::signUp(
        const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback
)
{
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(k200OK);
    response->setContentTypeCode(ContentType::CT_APPLICATION_JSON);

    callback(response);
}

void AuthController::signIn(
        const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback
)
{
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(k200OK);
    response->setContentTypeCode(ContentType::CT_APPLICATION_JSON);

    callback(response);
}
