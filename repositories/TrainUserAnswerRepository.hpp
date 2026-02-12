#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include "models/UserAnswer.hpp"
#include "repositories/ResultEmptyException.hpp"

namespace repositories
{

    class TrainUserAnswerRepository
    {
    private:
        drogon::orm::DbClientPtr _db;

    public:
        TrainUserAnswerRepository(drogon::orm::DbClientPtr db);

    public:
        drogon::Task<std::string> create(const models::UserAnswer &answer);
        drogon::Task<bool> remove(models::id id);
        drogon::Task<bool> update(const models::UserAnswer &answer);
        drogon::Task<std::optional<models::UserAnswer>> get(models::id id);
        drogon::Task<std::vector<models::UserAnswer>> getByTrainCardId(models::id trainCardId);
    };

}
