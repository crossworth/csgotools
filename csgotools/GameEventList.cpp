#include "GameEventList.hpp"
#include "netmessages_public.pb.h"

using namespace csgotools;

GameEventList GameEventList::CreateFromMemory(MemoryBitStream& memory) {
    return CreateFromMemory(std::move(memory));
}

GameEventList GameEventList::CreateFromMemory(MemoryBitStream&& memory) {
    CSVCMsg_GameEventList game_event_list;
    game_event_list.ParseFromArray(memory.GetBufferPointer(), memory.GetSize());

    GameEventList result;

    for (int32 i = 0; i < game_event_list.descriptors_size(); ++i) {
        const auto& descriptor = game_event_list.descriptors(i);

        GameEventDescriptor game_event_descriptor;
        game_event_descriptor.event_id = descriptor.eventid();
        game_event_descriptor.name = descriptor.name();

        for (int32 j = 0; j < descriptor.keys_size(); ++j) {
            const auto& key_descriptor = descriptor.keys(j);
            GameEventDescriptorKey key;
            key.type = key_descriptor.type();
            key.name = key_descriptor.name();
            game_event_descriptor.keys.push_back(std::move(key));
        }

        result.game_event_descriptor_[game_event_descriptor.event_id] = std::move(game_event_descriptor);
    }

    return result;
}

GameEventList::operator std::string() const {
    std::string out;

    for (auto& descriptor : game_event_descriptor_) {
        out += static_cast<std::string>(descriptor.second) + "\n";
    }

    return out;
}

std::ostream& csgotools::operator<<(std::ostream& out, GameEventList& game_event_list) {
    out << static_cast<std::string>(game_event_list);
    return out;
}