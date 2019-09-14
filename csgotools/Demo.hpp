#pragma once

#include <unordered_map>
#include <memory>

#include "ErrorHandling.hpp"
#include "Steam.hpp"
#include "DemoMemoryBitStream.hpp"
#include "DemoHeader.hpp"
#include "SendTable.hpp"
#include "Player.hpp"
#include "SendTable.hpp"
#include "CommandInfo.hpp"
#include "SequenceInfo.hpp"
#include "NetTick.hpp"
#include "ServerInfo.hpp"
#include "ConVar.hpp"
#include "GameEvent.hpp"
#include "GameEventList.hpp"
#include "GameEventSkipList.hpp"

namespace csgotools {

    using PlayerPtr = std::shared_ptr<Player>;

    enum ParseType : uint16 {
        ALL = 0xFFFF,
        PACKET = 0x8000,
        PACKET_ENTITIES = 0x8001,
        GAME_EVENTS = 0x8002,
        NET_TICKS = 0x8004,
        SERVER_INFO = 0x8008,
        SET_CON_VAR = 0x8016,
        USER_MESSAGE = 0x8032,
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
        explicit Demo(const std::string& file_name, const MemoryBitStream& memory);
        
        void Open(const std::string& file_name);
        void Open(const std::string& file_name, const MemoryBitStream& memory);
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
        bool UseDefaultEventSkipList() const { return use_default_event_skip_list_; }
        uint32 CurrentTick() const { return current_tick_; }
        uint32 TotalTicks() const { return static_cast<uint32>(header_.PlaybackTicks()); }
        uint32 TotalCommands() const { return total_commands_; }
        void ParseWarmUp(bool parse_warm_up) { parse_warm_up_ = parse_warm_up; }
        void UseDefaultEventSkipList(bool use_default_event_skip_list) { use_default_event_skip_list_ = use_default_event_skip_list; }

        std::vector<Player> Participants() const;
        std::vector<NetTick> NetTicks() const { return net_ticks_; }
        NetTick LastNetTick() const { return net_ticks_.back(); }
        csgotools::ServerInfo ServerInfo() const { return server_info_; }
        csgotools::ConVar ConVar(const std::string& name);
        std::unordered_map<std::string, csgotools::ConVar> ConVars() const { return con_vars_; }
        csgotools::GameEventList GameEventList() const { return game_event_list_; }
        csgotools::GameEventSkipList GameEventSkipList() const { return game_event_skip_list_; }

    private:

        void ResetState();
        DemoCommand ParseCommand();
        void ReadDummyData();
        void ParseHeader();
        void ParseSendTables();
        void ParseStringTables();
        void ParsePacket();
        void ParsePacketMessages(DemoMemoryBitStream&& data);
        void ParseStringTable(const std::string&& table_name, DemoMemoryBitStream& data);
        void ParseString(const std::string& table_name, DemoMemoryBitStream& data);
        void ParseClientData(const std::string& table_name, uint32 num_strings, DemoMemoryBitStream& data);
        
        void CreateDefaultSkipList();
        void DispatchGameEvent(GameEvent &&game_event);

        bool is_valid_{false};
        bool is_parsed_{false};
        bool parse_warm_up_{ true };
        bool is_warm_up_ended_{false};
        bool use_default_event_skip_list_{true};
        std::string file_name_;
        csgotools::ParseType parse_type_{ParseType::ALL};
        DemoMemoryBitStream demo_memory_;
        DemoHeader header_;

        int32 current_tick_{0};
        int32 total_commands_{0};
        std::unordered_map<std::string, SendTable> send_tables_;
        std::vector<ServerClass> server_classes_;
        std::vector<NetTick> net_ticks_;
        csgotools::ServerInfo server_info_;
        std::unordered_map<std::string, csgotools::ConVar> con_vars_;
        csgotools::GameEventList game_event_list_;
        csgotools::GameEventSkipList game_event_skip_list_;

        std::unordered_map<uint64, PlayerPtr> players_;
    };

    std::ostream& operator<<(std::ostream& out, const Demo& demo);
}
