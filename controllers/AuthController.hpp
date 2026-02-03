#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>

#include "services/AuthService.hpp"

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
  private:
    services::AuthService _authService;

  public:
    AuthController();

  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::signUp, "/sign-up", HttpMethod::Post);
    ADD_METHOD_TO(AuthController::signIn, "/sign-in", HttpMethod::Post);
    ADD_METHOD_TO(AuthController::refresh, "/refresh", HttpMethod::Post);
    METHOD_LIST_END

  public:
    drogon::Task<HttpResponsePtr> signUp(HttpRequestPtr request);
    drogon::Task<HttpResponsePtr> signIn(HttpRequestPtr request);
    drogon::Task<HttpResponsePtr> refresh(HttpRequestPtr request);
};
