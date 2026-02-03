#include <drogon/drogon.h>

int main(int argc, char* argv[])
{
    drogon::app().addListener("0.0.0.0", 5555).loadConfigFile("config.json");
    // Load config file
    // drogon::app().loadConfigFile("../config.json");
    // drogon::app().loadConfigFile("../config.yaml");
    // Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();

    return 0;
}
