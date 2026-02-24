#include <drogon/drogon.h>

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    const char *pathConfigJson = getenv("CONFIG_JSON_PATH");
    if (pathConfigJson == nullptr)
    {
        std::cerr << "environment variable CONFIG_JSON_PATH not found" << std::endl;
        return 1;
    }

    drogon::app().loadConfigFile(pathConfigJson);

    drogon::app().registerSyncAdvice([](const drogon::HttpRequestPtr &request)
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

    drogon::app().run();

    return 0;
}
