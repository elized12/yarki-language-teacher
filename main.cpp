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
    drogon::app().registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr &request, const drogon::HttpResponsePtr &response)
        {
            response->addHeader("Access-Control-Allow-Origin", "*");
            response->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            response->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        });

    drogon::app().registerPreRoutingAdvice([](const drogon::HttpRequestPtr &request,
                                              drogon::FilterCallback &&stop,
                                              drogon::FilterChainCallback &&pass)
                                           {
        if (request->method() != drogon::HttpMethod::Options)
        {
            pass();
            return;
        }

        auto response = drogon::HttpResponse::newHttpResponse();
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        stop(response); });

    drogon::app().run();

    return 0;
}
