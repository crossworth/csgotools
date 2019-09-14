#pragma once

#include <unordered_map>

#include "MemoryBitStream.hpp"
#include "GameEventDescriptor.hpp"

namespace csgotools {

    class GameEventList {
        std::unordered_map<int32, GameEventDescriptor> game_event_descriptor_;
    public:
        GameEventDescriptor operator[](int32 element) { return game_event_descriptor_[element]; }
        explicit operator std::string() const;
        static GameEventList CreateFromMemory(MemoryBitStream& memory);
        static GameEventList CreateFromMemory(MemoryBitStream&& memory);
    };

    std::ostream& operator<<(std::ostream& out, GameEventList& game_event_list);

}
