#include "csgotools.hpp"

using namespace std;
using namespace csgotools;


int main() {
    Demo d("1.dem");
    d.ParseAllTicks();

    auto players = d.Participants();

    for (auto& p : players) {
//        cout << p << endl;
    }

    auto server_info = d.ServerInfo();
//    cout << d.ServerInfo() << endl;
//    cout << d.Header() << endl;

    auto con_vars = d.ConVars();

    for (auto& v : con_vars) {
//        cout << v.second << endl;
    }


    cout << d.GameEventList();

    return 0;
}





