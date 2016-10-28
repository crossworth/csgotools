#include <iostream>

#include "Demo.hpp"
#include "DemoInfo.hpp"

using std::cout;
using std::endl;
using namespace csgotools;

int main(int argc, char** argv) {
	/*ProfileBlockStrMS("Complete demo");
	Demo demo;
	demo.Open("C:/1.dem");
	demo.SetParse(Demo::ParseType::ALL);

	demo.SetGameEventCallback("begin_new_match", [](GameEvent& evt) {
		std::cout << "BEGIN NEW MATCH FROM LAMBDA" << std::endl;
	});*/

	// demo.ParseUntilTick(x); -> Go to Tick bigger then _NOW_
	// demo.GoToTick(x); -> Go to any Tick, if less, re-read the demo or something

	//demo.ParseAllTicks();
	//cout << demo.IsDemoParsed() << endl;

	//cout << demo.GetServerInfo() << endl;

    DemoInfo demo_info;
    demo_info.Open("C:/1.dem.info");
    //cout << demo_info.GetDateUTC() << endl;
    //cout << demo_info.GetDateGMT() << endl;
    //cout << demo_info.GetCTScore() << endl;
    //cout << demo_info.GetTScore() << endl;
    cout << demo_info.GetMatchDuration() << endl;

    auto round = demo_info.GetRound(demo_info.GetNumberRounds() - 1);
    auto p = round.GetTPlayers();
    auto rounds = demo_info.GetRounds();

    for (auto& pl : p) {
       /* cout << pl.first << " " << pl.second.GetSteamID().GetSteamID3() << " K: " << pl.second.GetKills() << " A: " << pl.second.GetAssists()
            << " D: " << pl.second.GetDeaths() << " M: " << pl.second.GetMVPs() << " S: " << pl.second.GetScore() << endl;*/
    }



	return 0;
}