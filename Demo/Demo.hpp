#ifndef CSGOTOOLS_DEMO_HPP_
#define CSGOTOOLS_DEMO_HPP_

#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <cassert>
#include <set>

#include <Steam.hpp>
#include <Util.hpp>
#include <MemoryBitStream.hpp>

#include "DemoHeader.hpp"
#include "Decoder.hpp"
#include "Prop.hpp"
#include "GameEvent.hpp"
#include "ServerClass.hpp"
#include "FlattenedPropEntry.hpp"
#include "SequenceInfo.hpp"
#include "CommandInfo.hpp"
#include "ServerInfo.hpp"
#include "NetTick.hpp"
#include "Team.hpp"
#include "Player.hpp"


#include "generated_proto\netmessages_public.pb.h"
#include "generated_proto\cstrike15_usermessages_public.pb.h"
#include "generated_proto\cstrike15_gcmessages.pb.h"
#include "generated_proto\steammessages.pb.h"


namespace csgotools {

	using GameEventCallback = std::function<void(GameEvent&)>;

	class Demo {
		static constexpr uint32 kNetMaxPaypload = 262140;
		static constexpr uint32 kDemoRecordBufferSize = 2097152;
		static constexpr uint32 kSPropExclude = (1 << 6);
		static constexpr uint32 kSPropInsideArray = (1 << 8);
		static constexpr uint32 kSPropCollapsible = (1 << 11);
		static constexpr uint32 kSPropChangesOften = (1 << 18);
		static constexpr uint32 kEntitySentinel = 9999;
		static constexpr uint32 kNumNetworkedSerialNumberBits = 10;
		static constexpr uint32 kMaxEdictsBits = 11;
		static constexpr uint32 kMaxEdicts = (1 << kMaxEdictsBits);


		enum class DemoCommand {
			SIGN_ON = 1, // Startup message
			PACKET, // Network packet
			SYNC_TICK, // Client sync to demo tick
			CONSOLE_CMD, // Console command
			USER_CMD, // User command
			DATA_TABLES, // Network data tables
			STOP, // End of ticks
			CUSTOM_DATA, // blob binary data
			STRING_TABLES,
			LAST_CMD = STRING_TABLES // Last command
		};


		enum class UpdateType {
			ENTER_PVS = 0, // Entity came back into pvs, create new entity if one doesn't exist
			LEAVE_PVS, // Entity left pvs
			DELTA_ENT, // There is a delta for this entity.
			PRESERVE_ENT, // Entity stays alive but no delta ( could be LOD, or just unchanged )
			FINISHED, // finished parsing entities successfully
			FAILED, // parsing error occured while reading entities
		};


		struct EntityEntry {
			uint32 id{};
			uint32 class_id{};
			uint32 serial_number{};

			std::vector<PropEntry> props;

			EntityEntry() = default;
			EntityEntry(uint32 id, uint32 class_id, uint32 serial_number = 0)
			: id(id), class_id(class_id), serial_number(serial_number) {}
		};

	public:
		// TODO(Pedro): Check if the PACKET is working as expected
		enum ParseType : uint8 {
			ALL = 0b11111111,
			PACKET = 0b10000000,
				PACKET_ENTITIES = 0b10000001,
				GAME_EVENTS = 0b10000010,
				NET_TICKS = 0b10000100,
				SERVER_INFO = 0b10001000,
				SET_CON_VAR = 0b10010000,
				USER_MESSAGE = 0b10100000,
			DATA_TABLES = 0b01000000,
			STRING_TABLES = 0b00100000
		};

		Demo() = default;
		Demo(const std::string& demo_name);
		Demo(const std::string& demo_name, const MemoryBitStream& memory);
		bool Open(const std::string& demo_name);
		bool OpenFromMemory(const std::string& demo_name, const MemoryBitStream& memory);

		bool ParseTick();
		void ParseAllTicks();
		int32 GetCurrentTick() const { return in_game_tick_; }
		int32 GetNumCommands() const { return num_commands_; }
		
		void SetParseWarmUp(bool parse_warm_up) { parse_warm_up_ = parse_warm_up; };
		bool IsParsingWarmUp() const { return parse_warm_up_; }
		bool WarmUpEnded() const { return warm_up_ended_; }

		ServerInfo GetServerInfo() const { return server_info_; }
		NetTick GetLastNetTick() const { return net_ticks_.back(); }
		std::vector<NetTick> GetNetTickList() const;
		
		template<typename T>
		T GetConVar(const std::string& con_var_name);
	
		//Team GetCTTeam();
		//Team GetTTeam();
		//Team GetSpectators();

		std::vector<Player> GetPlayers() const { return players_; }
		uint32 GetNumberPlayers() const { return players_.size(); };

		void SetParse(uint8 type) { parse_type_ = static_cast<ParseType>(type); }
		bool IsParsing(ParseType type) const { return (type & parse_type_) != 0; }

		std::string GetDemoFileName() const { return demo_file_name_; }
		bool IsDemoValid() const { return is_demo_valid_; }
		bool IsDemoParsed() const { return is_demo_parsed_; }
		DemoHeader GetDemoHeader() const { return header_; }


		void SetGameEventCallback(const std::string& event_name, GameEventCallback callback);
		void RemoveGameEventCallback(const std::string& event_name);

		int32 GameEventNameToEventID(const std::string& event_name);

		void AddGameEventToSkipList(int32 event_id) { skip_game_events_.insert(event_id); }
		void AddGameEventToSkipList(const std::string& event_name);
		void RemoveGameEventFromSkipList(int32 event_id) {skip_game_events_.erase(skip_game_events_.find(event_id)); }
		void RemoveGameEventFromSkipList(const std::string& event_name);

		void SetDefaultSkipList(bool enable) { use_default_skip_list_ = enable; }
		bool IsDefaultSkipListEnable() { return use_default_skip_list_; }

		//int32 GetCurrentRound();
	private:
		void CreateSkipList();


		void ReadDummyData();

		bool ParseHeader();
		bool ParseDataTables();
		bool ParseStringTables();
		bool ParsePacket();
		bool ParseTickInternal();
		
		bool ParseStringTable(const std::string& table_name, MemoryBitStream& data);
		bool ParseUserInfo(MemoryBitStream&& data);
		bool ParseNetTick(CNETMsg_Tick&& net_tick);
		bool ParseServerInfo(CSVCMsg_ServerInfo&& server_info);
		bool ParseGameEvent(CSVCMsg_GameEvent&& game_event);
		bool ParseGameEventList(CSVCMsg_GameEventList&& game_event_list);
		bool ParseSetConVar(CNETMsg_SetConVar&& set_con_var);
		bool ParseUserMessage(CSVCMsg_UserMessage&& user_message);
		bool ParsePacketEntities(CSVCMsg_PacketEntities&& packet_entities);

		void FlattenDataTables(uint32 index);
		void GatherExcludes(CSVCMsg_SendTable& table, std::set<std::string>& excludes);
		void GatherProps(CSVCMsg_SendTable& table, std::set<std::string>& excludes, uint32 class_num);
		void GatherPropsInternal(CSVCMsg_SendTable& table, std::set<std::string>& excludes, uint32 class_num,
								 std::vector<FlattenedPropEntry>& temp_flattened_props);
		

		void AddEntity(uint32 id, uint32 class_id, uint32 serial_num, MemoryBitStream& data);
		void UpdateEntity(EntityEntry& entity, MemoryBitStream& data);
		void RemoveEntity(uint32 id);
		void ParseEntity(EntityEntry& entity, MemoryBitStream& data);
		int32 ReadFieldIndex(MemoryBitStream& data, bool new_way, uint32 index);



		CSVCMsg_SendTable* GetTableByClassID(uint32 class_id);


		void SwapTeamSide();
		
		// TODO(Pedro) make classes for most of this
/*		
		
		bool ParseEntity();
	*/	
		MemoryBitStream memory_{};
		DemoHeader header_{};
		CommandInfo command_info_{};
		std::vector<NetTick> net_ticks_;
		ServerInfo server_info_{};
		std::unordered_map<int32, GameEventDescriptor> game_event_descriptor_;
		std::unordered_map<std::string, GameEventCallback> game_events_callbacks_;
		std::unordered_map<std::string, std::string> con_var_;
		std::unordered_map<uint32, EntityEntry> entities_;

		std::vector<Player> players_;
		std::unordered_map<std::string, CSVCMsg_SendTable> send_tables_;
		std::unordered_map<uint32, ServerClass> server_classes_;
		std::set<int32> skip_game_events_;

		Team team_ct_{};
		Team team_t_{};
		Team team_spectators_{};

		ParseType parse_type_{ParseType::ALL};

		std::string demo_file_name_;
		bool is_demo_valid_{false};
		bool is_demo_parsed_{false};
		
		int32 in_game_tick_{};
		int32 num_commands_{};
		int32 server_class_bits_{};
		bool parse_warm_up_{true};
		bool warm_up_ended_{false};
		bool use_default_skip_list_{true};
	};


	template<typename T>
	T Demo::GetConVar(const std::string& con_var_name) {
		T result;

		if (!is_demo_valid_) {
			std::cerr << "Demo::GetConVar: The demo is not valid" << std::endl;
			return T;
		}

		auto con_var = con_var_.find(con_var_name);

		if (con_var != con_var_.end()) {
			std::stringstream value(con_var->second);

			if (!(value >> result)) {
				std::cerr << "Demo::GetConVar: Could not convert to the type specified" << std::endl;
			}
		}

		return result;
	}

}

#endif
