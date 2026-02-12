#pragma once

#include <drogon/HttpController.h>
#include <nlohmann/json.hpp>

#include "dto/TrainSettingsConverter.hpp"
#include "services/AuthService.hpp"
#include "services/TrainService.hpp"
#include "utils/StringHelper.hpp"

using namespace drogon;

class TrainController : public drogon::HttpController<TrainController>
{
private:
  services::TrainService _trainService;
  services::AuthService _authService;

public:
  TrainController();

public:
  METHOD_LIST_BEGIN

  ADD_METHOD_TO(TrainController::startTrainSession, "/train/session", HttpMethod::Post, "AuthFilter");
  ADD_METHOD_TO(TrainController::finishTrainSession, "/train/session/finish", HttpMethod::Post, "AuthFilter");
  ADD_METHOD_TO(TrainController::getTask, "/train/session/{1:sessionId}/task", HttpMethod::Get, "AuthFilter");
  ADD_METHOD_TO(TrainController::sendAnswerTask, "/train/session/{1:cardId}/answer", HttpMethod::Post, "AuthFilter");

  METHOD_LIST_END

public:
  drogon::Task<HttpResponsePtr> startTrainSession(HttpRequestPtr request);
  drogon::Task<HttpResponsePtr> finishTrainSession(HttpRequestPtr request);

  drogon::Task<HttpResponsePtr> getTask(HttpRequestPtr request, std::string &&sessionId);
  drogon::Task<HttpResponsePtr> sendAnswerTask(HttpRequestPtr request, models::id cardId);
};
