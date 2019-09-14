#pragma once

#include <sstream>

#include "Steam.hpp"
#include "MemoryBitStream.hpp"

namespace csgotools {

    class ServerInfo {
        int32 protocol_{};
        int32 server_count_{}; // Num change level
        bool is_dedicated_{};
        bool is_official_valve_server_{};
        bool is_hltv_{};
        bool is_replay_{};
        bool is_redirecting_to_proxy_relay_{};
        int32 c_os_{}; // L = Linux, W = Windows
        uint32 map_crc_{}; // cyclic redundancy check 
        uint32 client_crc_{};
        uint32 string_table_crc_{};
        int32 max_clients_{};
        int32 max_classes_{};
        int32 player_slot_{};
        float tick_interval_{};
        std::string game_dir_{};
        std::string map_name_{};
        std::string map_group_name_{};
        std::string sky_name_{};
        std::string host_name_{};
        uint64 ugc_map_id_{};
    public:

        ServerInfo() {}

        ServerInfo(int32 protocol, int32 server_count, bool is_dedicated, bool is_official_valve_server,
                   bool is_hltv, bool is_replay, bool is_redirecting_to_proxy_relay,
                   int32 c_os, uint32 map_crc, uint32 client_crc, uint32 string_table_crc,
                   int32 max_clients, int32 max_classes, int32 player_slot, float tick_interval,
                   const std::string& game_dir, const std::string& map_name, const std::string& map_group_name,
                   const std::string& sky_name, const std::string& host_name, uint64 ugc_map_id)
            : protocol_(protocol),
              server_count_(server_count),
              is_dedicated_(is_dedicated),
              is_official_valve_server_(is_official_valve_server),
              is_hltv_(is_hltv),
              is_replay_(is_replay),
              is_redirecting_to_proxy_relay_(is_redirecting_to_proxy_relay),
              c_os_(c_os),
              map_crc_(map_crc),
              client_crc_(client_crc),
              string_table_crc_(string_table_crc),
              max_clients_(max_clients),
              max_classes_(max_classes),
              player_slot_(player_slot),
              tick_interval_(tick_interval),
              game_dir_(game_dir),
              map_name_(map_name),
              map_group_name_(map_group_name),
              sky_name_(sky_name),
              host_name_(host_name),
              ugc_map_id_(ugc_map_id) {}


        int32 Protocol() const { return protocol_; }
        int32 ServerCount() const { return server_count_; }
        bool IsDedicated() const { return is_dedicated_; }
        bool IsOfficialValveServer() const { return is_official_valve_server_; }
        bool IsHLTV() const { return is_hltv_; }
        bool IsReplay() const { return is_replay_; }
        bool IsRedirectingToProxyRelay() const { return is_redirecting_to_proxy_relay_; }
        int32 OS() const { return c_os_; }
        uint32 MapCRC() const { return map_crc_; }
        uint32 ClientCRC() const { return client_crc_; }
        uint32 StringTableCRC() const { return string_table_crc_; }
        int32 MaxClients() const { return max_clients_; }
        int32 MaxClasses() const { return max_classes_; }
        int32 PlayerSlot() const { return player_slot_; }
        float TickInterval() const { return tick_interval_; }
        std::string GameDir() const { return game_dir_; }
        std::string MapName() const { return map_name_; }
        std::string MapGroupName() const { return map_group_name_; }
        std::string SkyName() const { return sky_name_; }
        std::string HostName() const { return host_name_; }
        uint64 UGCMapID() const { return ugc_map_id_; }

        explicit operator std::string() const;

        static ServerInfo CreateFromMemory(MemoryBitStream& memory);
        static ServerInfo CreateFromMemory(MemoryBitStream&& memory);
    };

    std::ostream& operator<<(std::ostream& out, ServerInfo& server_info);

}
