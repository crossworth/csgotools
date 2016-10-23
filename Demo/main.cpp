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
    cout << demo_info.GetDateUTC() << endl;
    cout << demo_info.GetDateGMT() << endl;

	return 0;
}