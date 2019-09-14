#include "GameEventDescriptor.hpp"

using namespace csgotools;

GameEventDescriptor::operator std::string() const {
    std::string out;

    out += "EventID: " + std::to_string(event_id) + "\n";
    out += "Name: " + name + "\n";

    for (auto& key : keys) {
        out += "Type: " + std::to_string(key.type) + "\n";
        out += "Key: " + key.name + "\n";
    }

    return out;
}

std::ostream& csgotools::operator<<(std::ostream& out, GameEventDescriptor& game_event) {
    out << static_cast<std::string>(game_event);
    return out;
}