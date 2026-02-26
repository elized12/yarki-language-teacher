#include <drogon/drogon.h>
#include <json/json.h>

#include <cstdlib>
#include <iostream>

#include "utils/AutoMigration.hpp"

int main(int argc, char *argv[])
{
    const char *pathConfigJson = getenv("CONFIG_JSON_PATH");
    if (pathConfigJson == nullptr)
    {
        LOG_ERROR << "environment variable \"CONFIG_JSON_PATH\" not found";
        std::cerr << "environment variable CONFIG_JSON_PATH not found" << std::endl;
        return 1;
    }

    drogon::app().loadConfigFile(pathConfigJson);
    drogon::app()
        .registerSyncAdvice([](const drogon::HttpRequestPtr &request)
                                -> drogon::HttpResponsePtr
                            {
        if (request->method() == drogon::HttpMethod::Options)
        {
            auto response = drogon::HttpResponse::newHttpResponse();

            const auto &origin = request->getHeader("Origin");
            if (!origin.empty())
            {
                response->addHeader("Access-Control-Allow-Origin", origin);
            }

            const auto &requestMethod =
                request->getHeader("Access-Control-Request-Method");
            if (!requestMethod.empty())
            {
                response->addHeader("Access-Control-Allow-Methods", requestMethod);
            }

            response->addHeader("Access-Control-Allow-Credentials", "true");

            const auto &requestHeaders =
                request->getHeader("Access-Control-Request-Headers");
            if (!requestHeaders.empty())
            {
                response->addHeader("Access-Control-Allow-Headers", requestHeaders);
            }

            return std::move(response);
        }
        return {}; });

    drogon::app().registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr &request,
           const drogon::HttpResponsePtr &response) -> void
        {
            const auto &origin = request->getHeader("Origin");
            if (!origin.empty())
            {
                response->addHeader("Access-Control-Allow-Origin", origin);
            }

            const auto &requestMethod =
                request->getHeader("Access-Control-Request-Method");
            if (!requestMethod.empty())
            {
                response->addHeader("Access-Control-Allow-Methods", requestMethod);
            }

            response->addHeader("Access-Control-Allow-Credentials", "true");

            const auto &requestHeaders =
                request->getHeader("Access-Control-Request-Headers");
            if (!requestHeaders.empty())
            {
                response->addHeader("Access-Control-Allow-Headers", requestHeaders);
            }
        });

    const Json::Value customConfig = drogon::app().getCustomConfig();
    if (!customConfig.isNull() && customConfig["migration"].isObject() && customConfig["migration"]["db"].isObject())
    {
        const Json::Value &configDb = customConfig["migration"]["db"];

        std::string host, port, dbname, user, password;
        if (configDb["host"].isNull())
        {
            std::cerr << "[ERROR] AutoMigration: \"host\" params not found" << std::endl;
            return 1;
        }

        host = configDb["host"].asString();

        if (configDb["port"].isNull())
        {
            std::cerr << "[ERROR] AutoMigration: \"port\" params not found" << std::endl;
            return 1;
        }

        port = configDb["port"].asString();

        if (configDb["dbname"].isNull())
        {
            std::cerr << "[ERROR] AutoMigration: \"dbname\" params not found" << std::endl;
            return 1;
        }

        dbname = configDb["dbname"].asString();

        if (configDb["user"].isNull())
        {
            std::cerr << "[ERROR] AutoMigration: \"user\" params not found" << std::endl;
            return 1;
        }

        user = configDb["user"].asString();

        if (configDb["password"].isNull())
        {
            std::cerr << "[ERROR] AutoMigration: \"password\" params not found" << std::endl;
            return 1;
        }

        password = configDb["password"].asString();

        utils::AutoMigration autoMigration(host, port, dbname, user, password);
        if (!autoMigration.performMigrations(drogon::app().getCustomConfig()))
        {
            return 1;
        }

        std::cout << "[INFO] Migrations success completed!" << std::endl;
    }

    drogon::app().run();

    return 0;
}
