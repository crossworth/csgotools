#include "ServerInfo.hpp"
#include "netmessages_public.pb.h"

using namespace csgotools;

ServerInfo::operator std::string() const {
    std::stringstream out;
    out << "------------SERVER INFO------------" << std::endl;
    out << "protocol: " << protocol_ << std::endl;
    out << "server_count (num change level): " << server_count_ << std::endl;
    out << "is_dedicated: " << ((is_dedicated_) ? "true" : "false") << std::endl;
    out << "is_official_valve_server: " << ((is_official_valve_server_) ? "true" : "false") << std::endl;
    out << "is_hltv: " << ((is_hltv_) ? "true" : "false") << std::endl;
    out << "is_replay: " << ((is_replay_) ? "true" : "false") << std::endl;
    out << "is_redirecting_to_proxy_relay: " << ((is_redirecting_to_proxy_relay_) ? "true" : "false") << std::endl;
    out << "c_os: " << c_os_ << std::endl;
    out << "map_crc: " << map_crc_ << std::endl;
    out << "client_crc: " << client_crc_ << std::endl;
    out << "string_table_crc: " << string_table_crc_ << std::endl;
    out << "max_clients: " << max_clients_ << std::endl;
    out << "max_classes: " << max_classes_ << std::endl;
    out << "player_slot: " << player_slot_ << std::endl;
    out << "tick_interval: " << tick_interval_ << std::endl;
    out << "game_dir: " << game_dir_ << std::endl;
    out << "map_name: " << map_name_ << std::endl;
    out << "map_group_name: " << map_group_name_ << std::endl;
    out << "sky_name: " << sky_name_ << std::endl;
    out << "host_name: " << host_name_ << std::endl;
    out << "ugc_map_id: " << ugc_map_id_ << std::endl;
    out << "------------SERVER INFO------------" << std::endl;
    return out.str();
}

ServerInfo ServerInfo::CreateFromMemory(MemoryBitStream& memory) {
    return CreateFromMemory(std::move(memory));
}

ServerInfo ServerInfo::CreateFromMemory(MemoryBitStream&& memory) {
    CSVCMsg_ServerInfo server_info;
    server_info.ParseFromArray(memory.GetBufferPointer(), memory.GetSize());
    return ServerInfo(server_info.protocol(),
                      server_info.server_count(),
                      server_info.is_dedicated(),
                      server_info.is_official_valve_server(),
                      server_info.is_hltv(),
                      server_info.is_replay(),
                      server_info.is_redirecting_to_proxy_relay(),
                      server_info.c_os(),
                      server_info.map_crc(),
                      server_info.client_crc(),
                      server_info.string_table_crc(),
                      server_info.max_clients(),
                      server_info.max_classes(),
                      server_info.player_slot(),
                      server_info.tick_interval(),
                      server_info.game_dir(),
                      server_info.map_name(),
                      server_info.map_group_name(),
                      server_info.sky_name(),
                      server_info.host_name(),
                      server_info.ugc_map_id());
}

std::ostream& csgotools::operator<<(std::ostream& out, ServerInfo& server_info) {
    out << static_cast<std::string>(server_info);
    return out;
}
