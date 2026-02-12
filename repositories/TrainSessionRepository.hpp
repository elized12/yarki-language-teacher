#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include "dto/TrainSettingsConverter.hpp"
#include "models/TrainSession.hpp"
#include "repositories/ResultEmptyException.hpp"

namespace repositories
{

    class TrainSessionRepository
    {
    private:
        drogon::orm::DbClientPtr _db;
        dto::TrainSettingsConverter _converter;

    public:
        TrainSessionRepository(drogon::orm::DbClientPtr db, dto::TrainSettingsConverter converter);

    public:
        drogon::Task<std::string> create(const models::TrainSession &session);
        drogon::Task<bool> remove(const std::string &uuid);
        drogon::Task<bool> update(const models::TrainSession &session);
        drogon::Task<std::optional<models::TrainSession>> get(const std::string &uuid);
    };

}