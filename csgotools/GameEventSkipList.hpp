#pragma once
#include <set>

namespace csgotools {

    class GameEventSkipList {
        std::set<std::string> events_;
    public:
        void AddEvent(const std::string& event_name) { events_.insert(event_name); }
        void RemoveEvent(const std::string& event_name) { events_.erase(events_.find(event_name)); }

        operator std::string() const {
            std::string out;

            out += "GameEventSkipList\n";

            for (auto& event: events_) {
                out += event + "\n";
            }

            return out;
        }
    };

    inline std::ostream& operator<<(std::ostream& out, GameEventSkipList& game_event_skip_list) {
        out << static_cast<std::string>(game_event_skip_list);
        return out;
    }

}