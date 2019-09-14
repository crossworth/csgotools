#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <strsafe.h>

#include "Demo.hpp"
#include "DemoInfo.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace csgotools; 


//const string PLAYER_NAME_API = "http://pedrohenrique.ninja/steam_name.php?id=";
//
//string GetPlayerNameFromID(uint64 steam_id) {
//    string player_name;
//    std::stringstream ss;
//    ss << steam_id;
//
//    char dir_buffer[500];
//    GetModuleFileName(NULL, dir_buffer, 500);
//    string path(dir_buffer);
//
//    path = path.substr(0, path.find_last_of("\\") + 1);
//
//    string command_complete = path + "simple_get.bat http://pedrohenrique.ninja/steam_name.php?id=" + ss.str() +  " -reportfile none";
//    FILE* cmd = _popen(command_complete.c_str(), "r");
//   
//    if (cmd != NULL) {
//        char buffer[32]; // max steam name
//        fread(buffer, sizeof(char), 32, cmd);
//
//        char* c = &buffer[0];
//        while (*c && *c != '\n' && *c != '\r') {
//            player_name.push_back(*c);
//            c++;
//        }
//    }
//
//    return player_name;
//}



int main(int argc, char** argv) {



	ProfileBlockStrMS("Complete demo");
	Demo demo;
	demo.Open("C:/1.dem");
	demo.SetParse(Demo::ParseType::ALL);

	demo.SetGameEventCallback("begin_new_match", [](GameEvent& evt) {
		std::cout << "BEGIN NEW MATCH FROM LAMBDA" << std::endl;
	});

	// demo.ParseUntilTick(x); -> Go to Tick bigger then _NOW_
	// demo.GoToTick(x); -> Go to any Tick, if less, re-read the demo or something

	demo.ParseAllTicks();
	//cout << demo.IsDemoParsed() << endl;

	//cout << demo.GetServerInfo() << endl;

    //DemoInfo demo_info;
    //demo_info.Open("C:/1.dem.info");
    ////cout << demo_info.GetDateUTC() << endl;
    ////cout << demo_info.GetDateGMT() << endl;
    ////cout << demo_info.GetCTScore() << endl;
    ////cout << demo_info.GetTScore() << endl;
    //cout << demo_info.GetMatchDuration() << endl;

    //auto round = demo_info.GetRound(demo_info.GetNumberRounds() - 1);
    //auto p = round.GetTPlayers();
    //auto rounds = demo_info.GetRounds();


    // FIND 5k's
   // string csgo_replay_path = "C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive/csgo/replays/";
   // vector<string> demos;

   // WIN32_FIND_DATA ffd;
   // TCHAR szDir[MAX_PATH];
   // LARGE_INTEGER filesize;
   // HANDLE hFind = INVALID_HANDLE_VALUE;

   // string files_filter = csgo_replay_path + "*.dem.info";
   // StringCchCopy(szDir, MAX_PATH, files_filter.c_str());

   // hFind = FindFirstFile(szDir, &ffd);

   // if (hFind == INVALID_HANDLE_VALUE) {
   //     cout << "Invalid folder" << endl;
   // }

   // do {
   //     if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
   //         continue; // dir
   //     } else {
   //         filesize.LowPart = ffd.nFileSizeLow;
   //         filesize.HighPart = ffd.nFileSizeHigh;
   //         demos.push_back(string(ffd.cFileName));
   //     }
   // } while (FindNextFile(hFind, &ffd) != 0);

   // int number_kills = 5;

   // cout << "=============" << endl;
   // cout << "DEMO LOCATION" << endl;
   // cout << "PLAYER NAME" << endl;
   // cout << "STEAMID64" << endl;
   // cout << "ROUND" << endl;
   // cout << "ROUND DURATION" << endl;
   // cout << "=============" << endl;
   // cout << endl << endl;

   // for (auto demo : demos) {
   //     DemoInfo demo_info;
   //     demo_info.Open(csgo_replay_path + demo);
   //     auto rounds = demo_info.GetRounds();

   //     for (int i = 0; i < rounds.size(); ++i) {
   //         auto players_prev_round = (i > 0) ? rounds[i - 1].GetAllPlayers() : rounds[i].GetAllPlayers();
   //         auto players_this_round = rounds[i].GetAllPlayers();

   //         for (int c = 0; c < players_this_round.size(); c++) { // c++ <-- HA HA, VERY FUNNY DR. JONES!
   //             auto players_this_round_it = players_this_round.begin();
   //             std::advance(players_this_round_it, c);
   //             auto steam_id64 = players_this_round_it->second.GetSteamID().GetSteamID64();
   //             if (players_this_round[steam_id64].GetKills() - players_prev_round[steam_id64].GetKills() == number_kills) {
   //                 cout << csgo_replay_path + demo.substr(0, demo.size() - 5) << endl;
   //                 cout << GetPlayerNameFromID(steam_id64) << endl;
   //                 cout << steam_id64 << endl;
   //                 cout << i << endl;
   //                 cout << rounds[i].GetDuration() << endl;
   //                 cout << endl;
   //             }
   //         }
   //     }
   //}
   // // record http://www.ryadel.com/en/csgo-video-making-tutorial-from-demo-replay-files/


	return 0;
}