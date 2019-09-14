#include <iostream>
#include <Util.hpp>
#include <steam/steam_api.h>

using namespace std;


// https://github.com/akiver/boiler-writter/tree/master/boiler-writter
// https://bitbucket.org/ACB/boiler/src/86898e45757bd1cdf38385c4ebe7f0e430e1d30c/boiler/main.cpp?at=master&fileviewer=file-view-default

int main(int argc, char** argv) {

    if (csgotools::IsCSGORunning()) {
        std::cout << "CSGO Already running" << std::endl;
        return 0;
    }

    if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid)) {
        std::cout << "Restart required" << std::endl;
    }

    if (!SteamAPI_Init()) {
        std::cout << "Steam is not running or logged" << std::endl;
    }

    if (SteamUser()) {
        std::cout << SteamUser()->BLoggedOn() << std::endl;
    }
    



    SteamAPI_Shutdown();

    return 0;
}