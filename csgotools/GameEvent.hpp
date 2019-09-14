#pragma once
#include <vector>

#include "GameEventKey.hpp"
#include "GameEventList.hpp"
#include "MemoryBitStream.hpp"


namespace csgotools {

    class GameEvent {
        std::vector<GameEventKey> keys_;
        int32 event_id_{0};
        std::string event_name_;
    public:
        std::vector<GameEventKey> Keys() const { return keys_; }
        int32 EventID() const { return event_id_; }
        std::string EventName() const { return event_name_; }

        explicit operator std::string() const;

        static GameEvent CreateFromMemory(MemoryBitStream& memory, GameEventList& event_list);
        static GameEvent CreateFromMemory(MemoryBitStream&& memory, GameEventList& event_list);
    };

    std::ostream& operator<<(std::ostream& out, GameEvent& game_event);
    

}
