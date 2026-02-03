#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::signUp, "/sign-up", HttpMethod::Post);
    ADD_METHOD_TO(AuthController::signIn, "/sign-in", HttpMethod::Post);
    METHOD_LIST_END

  public:
    void
    signUp(const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback);
    void
    signIn(const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback);
};
