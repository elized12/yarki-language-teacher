#include <drogon/drogon.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
    const char* pathConfigJson = getenv("CONFIG_JSON_PATH");

    drogon::app().loadConfigFile(pathConfigJson);
    drogon::app().run();

    return 0;
}
