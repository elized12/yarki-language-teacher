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
    drogon::app().run();

    return 0;
}
