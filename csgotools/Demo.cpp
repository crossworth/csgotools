#include "Demo.hpp"
#include "netmessages_public.pb.h"

using namespace csgotools;

Demo::Demo(const std::string& file_name) {
    Open(file_name);
}

Demo::Demo(const std::string& file_name, const MemoryBitStream& memory) {
    Open(file_name, memory);
}

void Demo::Open(const std::string& file_name) {
    ResetState();
    file_name_ = file_name;

    demo_memory_ = std::move(static_cast<DemoMemoryBitStream>(MemoryBitStream::CreateFromFile(file_name)));
    if (demo_memory_.GetSize() > 0) {
        ParseHeader();
    }
}

void Demo::Open(const std::string& file_name, const MemoryBitStream& memory) {
    ResetState();
    file_name_ = file_name;

    if (memory.GetSize() <= 0) {
        CSGOTOOLS_ERROR("Demo::Open: Memory too small: " + std::to_string(memory.GetSize()) + " bytes");
    } else {
        demo_memory_ = std::move(static_cast<DemoMemoryBitStream>(memory));
        ParseHeader();
    }
}

void Demo::ResetState() {
    file_name_.clear();
    is_parsed_ = false;
    is_valid_ = false;
    current_tick_ = 0;
    total_commands_ = 0;
    demo_memory_.Clear();
    header_.Clear();
}

void Demo::ParseHeader() {
    is_valid_ = false;

    if (demo_memory_.GetSize() <= 0) {
        return;
    }

    header_ = std::move(DemoHeader::CreateFromMemoryBitStream(demo_memory_));
    is_valid_ = header_.IsValid();
}

void Demo::ParseAllTicks() {
    while (!IsParsed() && ParseNextTick()) {}
}

bool Demo::ParseNextTick() {
    if (!IsValid()) {
        return false;
    }

    if (current_tick_ >= header_.PlaybackTicks()) {
        is_parsed_ = true;
        return false;
    }

    DemoCommand command = ParseCommand();

    switch (command) {
        case DemoCommand::SYNC_TICK:
        {} break;
        case DemoCommand::STOP:
        {
            is_parsed_ = true;
            return false;
        } break;
        case DemoCommand::CONSOLE_CMD:
        {
            ReadDummyData();
        } break;
        case DemoCommand::DATA_TABLES:
        {
            ParseSendTables();
        } break;
        case DemoCommand::STRING_TABLES:
        {
            ParseStringTables();
        } break;
        case DemoCommand::USER_CMD:
        {
            int32 outgoing_sequence = demo_memory_.ReadInt32();
            ReadDummyData();
        } break;
        case DemoCommand::SIGN_ON:
        case DemoCommand::PACKET:
        {
            ParsePacket();
        } break;
        default:
        {
            CSGOTOOLS_ERROR("Demo::ParseTick: Can't handle command: " +  std::to_string(static_cast<int>(command)));
        } break;
    }

    return true;
}

Demo::DemoCommand Demo::ParseCommand() {
    DemoCommand command = static_cast<DemoCommand>(demo_memory_.ReadByte());

    if (command < DemoCommand::SIGN_ON || command > DemoCommand::LAST_CMD) {
        CSGOTOOLS_ERROR("Demo::ParseTick: Missing end tag in demo file");
        return DemoCommand::STOP;
    }

    current_tick_ = demo_memory_.ReadInt32();
    uint8 player_slot = demo_memory_.ReadByte();

    ++total_commands_;
    return command;
}

void Demo::ReadDummyData() {
    int32 size = demo_memory_.ReadInt32();
    demo_memory_.SkipBytes(size);
}

void Demo::ParseSendTables() {
    int32 size = demo_memory_.ReadInt32();

    if (size < 0) {
        CSGOTOOLS_ERROR("Demo::ParseSendTables: Invalid size");
        return;
    }

    if (parse_type_ & ParseType::DATA_TABLES) {
        DemoMemoryBitStream data;
        data.Alloc(size);
        demo_memory_.ReadBytes(data.GetBufferPointer(), size);

        while (true) {
            auto table = SendTable::CreateFromMemory(data);

            if (table.IsEnd()) {
                break;
            }
            send_tables_[table.NetTableName()] = std::move(table);
        }

        // Read server classes
        uint16 size_server_classes = data.ReadInt16();

        server_classes_.resize(size_server_classes); // Just to improve speed

        for (uint16 i = 0; i < size_server_classes; ++i) {
            server_classes_.emplace_back(data.ReadInt16(), data.ReadString(256), data.ReadString(256));
        }

        // Flatten data table
        for (auto& server_class : server_classes_) {
            send_tables_[server_class.DTName()].MakeFlat(server_class, send_tables_);
        }

        // calculate server class bits
        while (size_server_classes >>= 1) {
            ++ServerClass::Bits;
        }
        ++ServerClass::Bits;

    } else {
        demo_memory_.SkipBytes(size);
    }
}

void Demo::ParseStringTables() {
    int32 size = demo_memory_.ReadInt32();

    if (size < 0) {
        CSGOTOOLS_ERROR("Demo::ParseStringTables: Invalid size");
        return;
    }

    if (parse_type_ & ParseType::STRING_TABLES) {
        DemoMemoryBitStream data;
        data.Alloc(size);
        demo_memory_.ReadBytes(data.GetBufferPointer(), size);

        uint8 num_tables = data.ReadByte();

        for (uint8 i = 0; i < num_tables; ++i) {
            std::string table_name = data.ReadString(100);
            ParseStringTable(std::move(table_name), data);
        }
    } else {
        demo_memory_.SkipBytes(size);
    }
}

void Demo::ParsePacket() {
    // command info
    CommandInfo command = CommandInfo::CreateFromMemory(demo_memory_);
    SequenceInfo sequence_info = SequenceInfo::CreateFromMemory(demo_memory_);

    int32 size = demo_memory_.ReadInt32();
    
    if (parse_type_ & ParseType::PACKET && 
        (parse_type_ << 6) > ParseType::PACKET) { // Check if any packet subtype is set
        DemoMemoryBitStream data;
        data.Alloc(size);
        demo_memory_.ReadBytes(data.GetBufferPointer(), size);
        ParsePacketMessages(std::move(data));
    } else {
        demo_memory_.SkipBytes(size);
    }
}

void Demo::ParsePacketMessages(DemoMemoryBitStream&& data) {
    while(!data.IsEnd()) {
        int32 command = data.ReadVarInt32();
        int32 size = data.ReadVarInt32();

        char* data_buffer = new char[size];
        data.ReadBytes(data_buffer, size);

        switch (command) {
            case net_NOP: {} break;
            case net_Tick:
            {
                if (parse_type_ & ParseType::NET_TICKS) {
                    net_ticks_.push_back(std::move(
                        NetTick::CreateFromMemory(MemoryBitStream::CreateFromCharPointer(data_buffer, size))
                    ));
                }
            } break;
            case svc_PacketEntities:
            {
                /*if (parse_type_ & ParseType::PACKET_ENTITIES) {
                    CSVCMsg_PacketEntities packet_entities;
                    packet_entities.ParseFromArray(data_buffer, size);
                    ParsePacketEntities(std::move(packet_entities));
                }*/
            } break;
            case svc_ServerInfo:
            {
                if (parse_type_ & ParseType::SERVER_INFO) {
                    server_info_ = std::move(
                        ServerInfo::CreateFromMemory(MemoryBitStream::CreateFromCharPointer(data_buffer, size))
                    );
                }

            } break;
            case svc_CreateStringTable:
            {
                if (parse_type_ & ParseType::STRING_TABLES) {

                }
            } break;
            case svc_UpdateStringTable:
            {
                if (parse_type_ & ParseType::STRING_TABLES) {

                }
            } break;
            case svc_GameEvent:
            {
                if (parse_type_ & ParseType::GAME_EVENTS) {
                    GameEvent game_event = GameEvent::CreateFromMemory(
                        MemoryBitStream::CreateFromCharPointer(data_buffer, size), game_event_list_
                    );

                    DispatchGameEvent(std::move(game_event));
                }
            } break;
            case svc_GameEventList:
            {
                if (parse_type_ & ParseType::GAME_EVENTS) {
                    game_event_list_ = GameEventList::CreateFromMemory(
                        MemoryBitStream::CreateFromCharPointer(data_buffer, size)
                    );

                    if (use_default_event_skip_list_) {
                        CreateDefaultSkipList();
                    }
                }
            } break;
            case net_SetConVar:
            {
                if (parse_type_ & ParseType::SET_CON_VAR) {
                    auto con_vars = ConVar::CreateFromMemory(
                        MemoryBitStream::CreateFromCharPointer(data_buffer, size)
                    );
                    con_vars_.insert(con_vars.begin(), con_vars.end());
                }
            } break;
            case svc_UserMessage:
            {
                /*if (parse_type_ & ParseType::USER_MESSAGE) {
                    CSVCMsg_UserMessage user_message;
                    user_message.ParseFromArray(data_buffer, size);
                    ParseUserMessage(std::move(user_message));
                }*/
            } break;
            default:
            {} break;
        }

        delete[] data_buffer;
    }
}

void Demo::ParseStringTable(const std::string&& table_name, DemoMemoryBitStream& data) {
    uint16 num_strings = data.ReadInt16();

    for (uint16 i = 0; i < num_strings; ++i) {
        ParseString(table_name, data);
    } 

    bool has_client_data = data.ReadBit();

    if (has_client_data) {
        ParseClientData(table_name, num_strings, data);
    }
}

void Demo::ParseString(const std::string& table_name, DemoMemoryBitStream& data) {
    int32 max_string_size = 100;
    std::string string_name = data.ReadString(max_string_size);

    bool has_data = data.ReadBit();

    if (has_data) {
        uint16 size = data.ReadInt16();

        if (table_name == "userinfo") {
            DemoMemoryBitStream user_info_data;
            // NOTE(Pedro): Valve default implementation adds 4 to the user data
            // I'm guessing that this must be for some odd case
            int32 valve_odd_padding = 4;
            user_info_data.Alloc(size + valve_odd_padding);
            data.ReadBytes(user_info_data.GetBufferPointer(), size);

            auto player = Player::CreateFromDemoUserInfo(user_info_data);
            players_[player.SteamID().SteamID64()] = std::make_shared<Player>(player);
        } else {
            data.SkipBytes(size);
        }

        // TODO(Pedro): Parse modelprecache, instancebaseline ?
    }
}

// ReSharper disable CppMemberFunctionMayBeStatic 
// ReSharper disable CppMemberFunctionMayBeConst
void Demo::ParseClientData(const std::string& table_name, uint32 num_strings, DemoMemoryBitStream& data) {
    int32 max_string_size = 100;
    std::cout << "Demo with Client data" << std::endl; // Only to find client data

    uint16 num_strings_client = data.ReadInt16();

    for (uint16 i = 0; i < num_strings; ++i) {
        std::string string_name = data.ReadString(max_string_size);

        bool has_data = data.ReadBit();

        if (has_data) {
            uint16 size = data.ReadInt16();
            // NOTE(Pedro): Find a demo with client data
            char* data_buffer = new char[size];
            data.ReadBytes(data_buffer, size);
            delete[] data_buffer;
        }
    }
}
// ReSharper restore CppMemberFunctionMayBeStatic
// ReSharper restore CppMemberFunctionMayBeConst

void Demo::CreateDefaultSkipList() {
    game_event_skip_list_.AddEvent("player_footstep");
    game_event_skip_list_.AddEvent("weapon_fire");
    game_event_skip_list_.AddEvent("bomb_beep");
    game_event_skip_list_.AddEvent("player_blind");
    game_event_skip_list_.AddEvent("player_jump");
    game_event_skip_list_.AddEvent("player_hurt");
    game_event_skip_list_.AddEvent("weapon_reload");
    game_event_skip_list_.AddEvent("weapon_zoom");
    game_event_skip_list_.AddEvent("hltv_status");
    game_event_skip_list_.AddEvent("hltv_chase");
}

void Demo::DispatchGameEvent(GameEvent&& game_event) {
    // check skip list
    // events callbacks and shit
    // parse warmup

}

std::vector<Player> Demo::Participants() const {
    std::vector<Player> result;

    std::for_each(players_.begin(), players_.end(), [&result] (const auto& player) {
        result.push_back(*player.second);
    });

    return result;
}

csgotools::ConVar Demo::ConVar(const std::string& name) {
    if (con_vars_.find(name) != con_vars_.end()) {
        return con_vars_[name];
    } else {
        return csgotools::ConVar("", "");
    }
}

std::ostream& csgotools::operator<<(std::ostream& out, const Demo& demo) {
    out << demo.Header();
    
    // Get CTTeam
    // Get TTeam


    return out;
}
