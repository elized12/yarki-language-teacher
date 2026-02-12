#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include "models/TrainCard.hpp"
#include "repositories/ResultEmptyException.hpp"

namespace repositories
{

    class TrainCardRepository
    {
    private:
        drogon::orm::DbClientPtr _db;

    public:
        TrainCardRepository(drogon::orm::DbClientPtr db);

    public:
        drogon::Task<models::id> create(const models::TrainCard &card);
        drogon::Task<bool> remove(models::id id);
        drogon::Task<bool> update(const models::TrainCard &card);
        drogon::Task<std::optional<models::TrainCard>> get(models::id id);
        drogon::Task<std::optional<models::TrainCard>> getCurrentCard(const std::string &sessionId);
        drogon::Task<std::optional<int>> getLastPosition(const std::string &sessionIds);
        drogon::Task<std::vector<models::TrainCard>> getLastCards(const std::string &sessionId, ssize_t count);
    };

}