#ifndef CSGOTOOLS_SERVER_INFO_HPP_
#define CSGOTOOLS_SERVER_INFO_HPP_

#include <iostream>

#include <Steam.hpp>

namespace csgotools {
	struct ServerInfo {
		ServerInfo() = default;

		int32 protocol{};
		int32 server_count{}; // Num change level
		bool is_dedicated{};
		bool is_official_valve_server{};
		bool is_hltv{};
		bool is_replay{};
		bool is_redirecting_to_proxy_relay{};
		int32 c_os{}; // L = Linux, W = Windows
		uint32 map_crc{}; // cyclic redundancy check 
		uint32 client_crc{};
		uint32 string_table_crc{};
		int32 max_clients{};
		int32 max_classes{};
		int32 player_slot{};
		float tick_interval{};
		std::string game_dir{};
		std::string map_name{};
		std::string map_group_name{};
		std::string sky_name{};
		std::string host_name{};
		uint32 public_ip{};
		uint64 ugc_map_id{};
	};

	std::ostream& operator<<(std::ostream& out, const ServerInfo& server_info);


}

#endif

