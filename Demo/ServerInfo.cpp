#include "ServerInfo.hpp"

using namespace csgosdk;

std::ostream& csgosdk::operator<<(std::ostream& out, const ServerInfo& server_info) {
	out << "------------SERVER INFO------------" << std::endl;
	out << "protocol: " << server_info.protocol << std::endl;
	out << "server_count (num change level): " << server_info.server_count << std::endl;
	out << "is_dedicated: " << ((server_info.is_dedicated) ? "true" : "false") << std::endl;
	out << "is_official_valve_server: " << ((server_info.is_official_valve_server) ? "true" : "false") << std::endl;
	out << "is_hltv: " << ((server_info.is_hltv) ? "true" : "false") << std::endl;
	out << "is_replay: " << ((server_info.is_replay) ? "true" : "false") << std::endl;
	out << "is_redirecting_to_proxy_relay: " << ((server_info.is_redirecting_to_proxy_relay) ? "true" : "false") << std::endl;
	out << "c_os: " << server_info.c_os << std::endl;
	out << "map_crc: " << server_info.map_crc << std::endl;
	out << "client_crc: " << server_info.client_crc << std::endl;
	out << "string_table_crc: " << server_info.string_table_crc << std::endl;
	out << "max_clients: " << server_info.max_clients << std::endl;
	out << "max_classes: " << server_info.max_classes << std::endl;
	out << "player_slot: " << server_info.player_slot << std::endl;
	out << "tick_interval: " << server_info.tick_interval << std::endl;
	out << "game_dir: " << server_info.game_dir << std::endl;
	out << "map_name: " << server_info.map_name << std::endl;
	out << "map_group_name: " << server_info.map_group_name << std::endl;
	out << "sky_name: " << server_info.sky_name << std::endl;
	out << "host_name: " << server_info.host_name << std::endl;
	out << "public_ip: " << server_info.public_ip << std::endl;
	out << "ugc_map_id: " << server_info.ugc_map_id << std::endl;
	out << "------------SERVER INFO------------" << std::endl;
	return out;
}