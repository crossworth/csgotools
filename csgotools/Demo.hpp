#pragma once

#include <unordered_map>
#include <memory>

#include "Steam.hpp"
#include "DemoMemoryBitStream.hpp"
#include "DemoHeader.hpp"
#include "SendTable.hpp"
#include "Player.hpp"

namespace csgotools {

    using PlayerPtr = std::shared_ptr<Player>;

    enum ParseType : uint16 {
        ALL = 0xFFFF,
        PACKET = 0x8000,
        DATA_TABLES = 0x4000,
        STRING_TABLES = 0x2000
    };

    class Demo {

        enum class DemoCommand {
            SIGN_ON = 1,
            PACKET,
            SYNC_TICK,
            CONSOLE_CMD,
            USER_CMD,
            DATA_TABLES,
            STOP,
            CUSTOM_DATA,
            STRING_TABLES,
            LAST_CMD = STRING_TABLES
        };

        enum class UpdateType {
            ENTER_PVS,
            LEAVE_PVS,
            DELTA_ENT,
            PRESERVE_ENT,
            FINISHED,
            FAILED
        };        
    public:
        Demo() = default;
        explicit Demo(const std::string& file_name);
        explicit Demo(const std::string& file_name, const DemoMemoryBitStream& memory);
        
        void Open(const std::string& file_name);
        void Open(const std::string& file_name, const DemoMemoryBitStream& memory);
        bool ParseNextTick();
        void ParseAllTicks();


        std::string FileName() const { return file_name_; }
        csgotools::ParseType ParseType() const { return parse_type_; }
        void ParseType(csgotools::ParseType parse_type) { parse_type_ = parse_type; }
        bool IsParsed() const { return is_parsed_; }
        DemoHeader Header() const { return header_; }
        bool IsValid() const { return is_valid_; }
        void IsValid(bool is_valid) { is_valid_ = is_valid; }
        bool ParseWarmUp() const { return parse_warm_up_; }
        bool IsWarmUpEnded() const { return is_warm_up_ended_; }
        bool UseDefaultSkipList() const { return use_default_skip_list_; }
        uint32 CurrentTick() const { return current_tick_; }
        uint32 TotalTicks() const { return static_cast<uint32>(header_.PlaybackTicks()); }
        uint32 TotalCommands() const { return total_commands_; }
        void ParseWarmUp(bool parse_warm_up) { parse_warm_up_ = parse_warm_up; }
        void UseDefaultSkipList(bool use_default_skip_list) { use_default_skip_list_ = use_default_skip_list; }

    private:

        void ResetState();
        void ReadDummyData();
        void ParseHeader();
        void ParseSendTables();
        void ParseStringTables();
        void ParsePacket();
        void ParseStringTable(const std::string&& table_name, MemoryBitStream& data);

        bool is_valid_{false};
        bool is_parsed_{false};
        bool parse_warm_up_{ true };
        bool is_warm_up_ended_{false};
        bool use_default_skip_list_{true};
        std::string file_name_;
        csgotools::ParseType parse_type_{ParseType::ALL};
        DemoMemoryBitStream demo_memory_;
        DemoHeader header_;

        int32 current_tick_{0};
        int32 total_commands_{0};
        std::unordered_map<std::string, SendTable> send_tables_;
        std::vector<ServerClass> server_classes_;

        std::unordered_map<uint64, PlayerPtr> players_;

        int32 server_class_bits{0}; // TODO(Pedro) Put this on his own class
    };

    std::ostream& operator<<(std::ostream& out, const Demo& demo);
}
