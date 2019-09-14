#pragma once
#include <vector>

#include "Steam.hpp"
#include "GameEventDescriptorKey.hpp"

namespace csgotools {

    struct GameEventDescriptor {
        int32 event_id{ 0 };
        std::string name;
        std::vector<GameEventDescriptorKey> keys;

        explicit operator std::string() const;
    };

    std::ostream& operator<<(std::ostream& out, GameEventDescriptor& game_event);

}