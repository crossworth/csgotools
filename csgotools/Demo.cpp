#include "Demo.hpp"
#include "SendTable.hpp"

using namespace csgotools;

Demo::Demo(const std::string& file_name) {
    Open(file_name);
}

void Demo::Open(const std::string& file_name) {
    ResetState();
    file_name_ = file_name;

    demo_memory_ = std::move(static_cast<DemoMemoryBitStream>(MemoryBitStream::CreateFromFile(file_name)));
    if (demo_memory_.GetSize() > 0) {
        ParseHeader();
    }
}

Demo::Demo(const std::string& file_name, const DemoMemoryBitStream& memory) {
    Open(file_name, memory);
}

void Demo::Open(const std::string& file_name, const DemoMemoryBitStream& memory) {
    ResetState();
    file_name_ = file_name;

    if (memory.GetSize() <= 0) {
        std::cerr << "Demo::Open: Memory too small: " << memory.GetSize() << " bytes" << std::endl;
    } else {
        demo_memory_ = std::move(memory);
        ParseHeader();
    }
}

void Demo::ParseAllTicks() {
    while (!IsParsed() && ParseNextTick()) {}
}

bool Demo::ParseNextTick() {
    if (current_tick_ >= header_.PlaybackTicks()) {
        is_parsed_ = true;
        return false;
    }

    DemoCommand command = static_cast<DemoCommand>(demo_memory_.ReadByte());

    if (command < DemoCommand::SIGN_ON) {
        std::cerr << "Demo::ParseTick: Missing end tag in demo file" << std::endl;
        return false;
    }

    assert(command >= DemoCommand::SIGN_ON && command <= DemoCommand::LAST_CMD);

    current_tick_ = demo_memory_.ReadInt32();
    uint8 player_slot = demo_memory_.ReadByte();

    ++total_commands_;

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
            std::cerr << "Demo::ParseTick: Can't handle command: " << static_cast<int>(command) << std::endl;
        } break;
    }

    return true;
}

void Demo::ReadDummyData() {
    int32 size = demo_memory_.ReadInt32();
    demo_memory_.SkipBytes(size);
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

void Demo::ParseSendTables() {
    int32 size = demo_memory_.ReadInt32();

    if (size < 0) {
        std::cerr << "Demo::ParseSendTables: Invalid size" << std::endl;
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

        uint16 size_server_classes = data.ReadInt16();

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
        std::cerr << "Demo::ParseStringTables: Invalid size" << std::endl;
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
    demo_memory_.SkipBytes(152);
    // sequence info
    demo_memory_.SkipBytes(8);
    // size
    int32 size = demo_memory_.ReadInt32();
    // data
    demo_memory_.SkipBytes(size);
}

void Demo::ParseStringTable(const std::string&& table_name, MemoryBitStream& data) {
    uint16 num_strings = data.ReadInt16();

    for (uint16 i = 0; i < num_strings; ++i) {
        std::string string_name = data.ReadString(100);

        bool has_data = data.ReadBit();

        if (has_data) {
            uint16 size = data.ReadInt16();

            if (table_name == "userinfo") {
                DemoMemoryBitStream user_info_data;
                // NOTE(Pedro): Valve default implementation adds 4 to the user data
                // I'm guessing that this must be for some odd case
                user_info_data.Alloc(size + 4);
                data.ReadBytes(user_info_data.GetBufferPointer(), size);
                
                auto player = Player::CreateFromDemoUserInfo(user_info_data);
                players_[player.SteamID().SteamID64()] = std::make_shared<Player>(player);
            } else {
                data.SkipBytes(size);
            }
            
            // userinfo, modelprecache, instancebaseline
            // std::cout << string_name << std::endl;
        }
    } // for num_strings

    bool has_client_data = data.ReadBit();

    if (has_client_data) {
        std::cout << "Demo with Client data" << std::endl; // Only to find client data

        uint16 num_strings_client = data.ReadInt16();

        for (uint16 i = 0; i < num_strings; ++i) {
            std::string string_name = data.ReadString(100);

            bool has_data = data.ReadBit();

            if (has_data) {
                uint16 size = data.ReadInt16();
                // NOTE(Pedro): Find a demo with client data
                char* data_buffer = new char[size];
                data.ReadBytes(data_buffer, size);
                delete[] data_buffer;
            }
        }
    } // if has_client_data
}

std::ostream& csgotools::operator<<(std::ostream& out, const Demo& demo) {
    out << demo.Header();
    // TODO(Pedro): Make this complete
    return out;
}
