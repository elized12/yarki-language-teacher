#pragma once

#include <drogon/HttpController.h>
#include <nlohmann/json.hpp>

#include "services/AuthService.hpp"
#include "services/TranslateService.hpp"

using namespace drogon;

class TranslateController : public drogon::HttpController<TranslateController>
{
private:
  services::TranslateService _translateService;
  services::AuthService _authService;

public:
  TranslateController();

public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TranslateController::addTranslate, "/translate", HttpMethod::Post, "AuthFilter");
  ADD_METHOD_TO(
      TranslateController::removeTranslate, "/translate", HttpMethod::Delete, "AuthFilter");
  ADD_METHOD_TO(
      TranslateController::getTranslates, "/translate/{1}", HttpMethod::Get, "AuthFilter");
  METHOD_LIST_END

public:
  drogon::Task<HttpResponsePtr> addTranslate(HttpRequestPtr request);
  drogon::Task<HttpResponsePtr> removeTranslate(HttpRequestPtr request);
  drogon::Task<HttpResponsePtr> getTranslates(HttpRequestPtr request, models::id wordId);
};
