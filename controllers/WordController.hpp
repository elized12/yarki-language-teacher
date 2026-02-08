#pragma once

#include <drogon/HttpController.h>
#include <nlohmann/json.hpp>

#include "models/Type.hpp"
#include "services/ValidataionException.hpp"
#include "services/WordService.hpp"

using namespace drogon;

class WordController : public drogon::HttpController<WordController>
{
private:
  static const ssize_t MAX_LIMIT = 5000;
  services::WordService _wordService;

public:
  WordController();

public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(
      WordController::getWords,
      "/words/{1:userId}/{2:languageCode}?offset={3:offset}&limit={4:limit}",
      HttpMethod::Get,
      "AuthFilter");
  METHOD_LIST_END

public:
  drogon::Task<HttpResponsePtr> getWords(
      HttpRequestPtr request,
      models::id userId,
      std::string &&languageCode,
      ssize_t offset,
      ssize_t limit);

  drogon::Task<HttpResponsePtr>
  getCount(HttpRequestPtr request, models::id userId, std::string &&code);
};
