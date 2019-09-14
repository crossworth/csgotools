#include "GameEvent.hpp"
#include "netmessages_public.pb.h"

using namespace csgotools;

GameEvent GameEvent::CreateFromMemory(MemoryBitStream& memory, GameEventList& event_list) {
    return CreateFromMemory(std::move(memory), event_list);
}

GameEvent GameEvent::CreateFromMemory(MemoryBitStream&& memory, GameEventList& event_list) {
    CSVCMsg_GameEvent game_event;
    game_event.ParseFromArray(memory.GetBufferPointer(), memory.GetSize());

    GameEvent result;

    result.event_id_ = game_event.eventid();
    result.event_name_ = event_list[result.event_id_].name;

    for (int32 i = 0; i < game_event.keys_size(); ++i) {
        const auto& key = game_event.keys(i);
        
        result.keys_.emplace_back(GameEventKey{key.type(), key.val_string(),
                                  key.val_float(), key.val_long(), key.val_short(),
                                  key.val_byte(), key.val_bool(), key.val_uint64(),
                                  key.val_wstring()});
    }

    return result;
}

GameEvent::operator std::string() const {
    std::string out;

    out += "EventID: " + std::to_string(event_id_) + "\n";
    out += "Event Name: " + event_name_ + "\n";

    for (auto& key : keys_) {
        out += static_cast<std::string>(key) + "\n";
    }

    return out;
}

std::ostream& csgotools::operator<<(std::ostream& out, GameEvent& game_event) {
    out << static_cast<std::string>(game_event);
    return out;
}




