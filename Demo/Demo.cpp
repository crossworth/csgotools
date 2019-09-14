#include "Demo.hpp"

using namespace csgotools;

Demo::Demo(const std::string& demo_name) {
	Open(demo_name);
}

Demo::Demo(const std::string& demo_name, const MemoryBitStream& memory) {
	OpenFromMemory(demo_name, memory);
}

bool Demo::Open(const std::string& demo_name) {
	demo_file_name_ = GetLastPartFromPathString(demo_name);
	is_demo_parsed_ = false;
	parse_type_ = ParseType::ALL;
	in_game_tick_ = 0;
	num_commands_ = 0;

	std::string string_buffer;

	std::ifstream file;
	file.open(demo_name.c_str(), std::ios::in | std::ios::binary);

	if (file.is_open()) {
		size_t file_size = 0;
		file.seekg(0, std::ios::end);
		file_size = static_cast<size_t>(file.tellg());
		file.seekg(0, std::ios::beg);

		if (file_size <= 0) {
			std::cerr << "Demo::Open: File too small: " << file_size << " bytes" << std::endl;
			file.close();
			is_demo_valid_ = false;
			return false;
		}

		string_buffer.resize(file_size);
		file.read(&string_buffer[0], file_size);
		file.close();
		
		memory_ = MemoryBitStream(string_buffer.c_str(), file_size);

		ParseHeader();
	} else {
		std::cerr << "Demo::Open: Could not open file: " << demo_name << std::endl;
		is_demo_valid_ = false;
		return false;
	}

	is_demo_valid_ = true;
	return true;
}

bool Demo::OpenFromMemory(const std::string& demo_name, const MemoryBitStream& memory) {
	demo_file_name_ = GetLastPartFromPathString(demo_name);
	is_demo_parsed_ = false;
	memory_ = memory;
	memory_.SetPosition(0);
	parse_type_ = ParseType::ALL;
	in_game_tick_ = 0;
	num_commands_ = 0;
	
	if (memory_.GetSize() > 0) {
		ParseHeader();
		return true;
	} else {
		return false;
	}	
}

bool Demo::ParseTick() {
	if (in_game_tick_ >= header_.GetPlayBackTicks()) {
		is_demo_parsed_ = true;
		return false;
	}

	return ParseTickInternal();
}

void Demo::ParseAllTicks() {
	while (!is_demo_parsed_ && ParseTick()) {}
}

void Demo::ReadDummyData() {
	int32 size = memory_.ReadInt32();
	memory_.SkipBytes(size);
}

bool Demo::ParseHeader() {
	is_demo_valid_ = false;

	if (memory_.GetSize() <= 0) {
		return false;
	}

	header_ = DemoHeader::CreateFromMemoryBitStream(memory_);

	if (header_.GetFileStamp() == DemoHeader::kDemoHeaderID && header_.GetProtocol() == DemoHeader::kDemoProtocol) {
		is_demo_valid_ = true;
	}

	return true;
}

bool Demo::ParseDataTables() {
	int32 data_size = memory_.ReadInt32();
	sm_assert(data_size > 0);

	if (parse_type_ & ParseType::DATA_TABLES) {
		MemoryBitStream data;
		data.Alloc(data_size);
		memory_.ReadBytes(data.GetBufferPointer(), data_size);

		while (true) {
			CSVCMsg_SendTable msg;
			data.ReadVarInt32(); // type
			int32 message_size = data.ReadVarInt32();

			if (message_size < 0 || message_size > kNetMaxPaypload) {
				std::cerr << "Demo::ParseDataTables: message_size < 0 or message_size > Demo::kNetMaxPaypload (" << message_size << ")" << std::endl;
				return false;
			}

			if (message_size > Demo::kDemoRecordBufferSize) {
				std::cerr << "Demo::ParseDataTables: message_size > Demo::kDemoRecordBufferSize (" << message_size << ")" << std::endl;
				return false;
			}

			char* message_buffer = new char[message_size];
			data.ReadBytes(message_buffer, message_size);
			msg.ParseFromArray(message_buffer, message_size);
			delete[] message_buffer;

			if (msg.is_end()) {
				break;
			}
			
			send_tables_[msg.net_table_name()] = std::move(msg);
		} 


		uint16 num_server_classes = data.ReadInt16();

		for (uint16 i = 0; i < num_server_classes; i++) {
			ServerClass entry;
			entry.SetClassID(data.ReadInt16());

			if (entry.GetClassID() > num_server_classes) {
				std::cerr << "Demo::ParseDataTables: entry.GetClassID() > num_server_classes" << std::endl;
				return false;
			}

			entry.SetName(data.ReadString(256));
			entry.SetDTName(data.ReadString(256));
			server_classes_[entry.GetClassID()] = std::move(entry);
		}

		for (uint16 i = 0; i < num_server_classes; i++) {
			FlattenDataTables(i);
		}

		int num_class_tmp = num_server_classes;
		server_class_bits_ = 0;
		while (num_class_tmp >>= 1) { 
			++server_class_bits_; 
		}
		++server_class_bits_;

	} else {
		memory_.SkipBytes(data_size);
	}

	return true;
}

void Demo::FlattenDataTables(uint32 index) {
	CSVCMsg_SendTable& table = send_tables_[server_classes_[index].GetDTName()];

	std::set<std::string> exclude_entries;

	GatherExcludes(table, exclude_entries);
	GatherProps(table, exclude_entries, index);

	std::vector<FlattenedPropEntry>& flattened_props = server_classes_[index].GetFlattenedPropsEntry();
	std::vector<uint32> priorities;
	priorities.push_back(64);
	
	uint32 i = 0;
	for (auto& prop : flattened_props) {
		uint32 priority = prop.prop->priority();

		bool found = false;

		for (uint32 j = 0, size_j = priorities.size(); j < size_j; ++j) {
			if (priorities[j] == priority) {
				found = true;
				break;
			}
		}

		if (!found) {
			priorities.push_back(priority);
		}
	}

	std::sort(priorities.begin(), priorities.end());

	// sort flattened_props by priority
	uint32 start = 0;
	for (uint32 i = 0, size = priorities.size(); i < size; ++i) {
		uint32 priority = priorities[i];

		while (true) {
			uint32 current_prop = start;
			while (current_prop < flattened_props.size()) {
				const FlattenedPropEntry& key = flattened_props.at(current_prop);

				if (key.prop->priority() == priority || (priority == 64 && (Demo::kSPropChangesOften & key.prop->flags()))) {
					if (start != current_prop) {
						FlattenedPropEntry temp = flattened_props.at(start);
						flattened_props.at(start) = flattened_props.at(current_prop);
						flattened_props.at(current_prop) = temp;
					}
					++start;
					break;
				}
				++current_prop;
			}

			if (current_prop == flattened_props.size()) {
				break;
			}
		}
	}
}

void  Demo::GatherExcludes(CSVCMsg_SendTable& table, std::set<std::string>& excludes) {
	for (int32 i = 0, size = table.props_size(); i < size; ++i) {
		const CSVCMsg_SendTable::sendprop_t& send_prop = table.props(i);
		
		if (send_prop.flags() & Demo::kSPropExclude) {
			excludes.insert(std::string(send_prop.dt_name() + send_prop.var_name()));
		} else if (static_cast<SendPropType>(send_prop.type()) == SendPropType::DPT_DATA_TABLE) {
			std::string dt_name = send_prop.dt_name();
			if (send_tables_.find(dt_name) != send_tables_.end()) {
				CSVCMsg_SendTable* sub_send_table = &send_tables_[dt_name];
				GatherExcludes(*sub_send_table, excludes);
			}
		}
	}
}

void Demo::GatherPropsInternal(CSVCMsg_SendTable& table, std::set<std::string>& excludes,
							   uint32 class_num, std::vector<FlattenedPropEntry>& temp_flattened_props) {
	
	for (int32 i = 0, size = table.props_size(); i < size; ++i) {
		const CSVCMsg_SendTable::sendprop_t& send_prop = table.props(i);

		std::string excluded_name = table.net_table_name() + send_prop.var_name();

		if ((Demo::kSPropInsideArray & send_prop.flags()) || (Demo::kSPropExclude & send_prop.flags())) {
			continue;
		} else if (excludes.count(excluded_name)) {
			continue;
		} else if (static_cast<SendPropType>(send_prop.type()) == SendPropType::DPT_DATA_TABLE) {
			std::string dt_name = send_prop.dt_name();
			if (send_tables_.find(dt_name) != send_tables_.end()) {
				CSVCMsg_SendTable* sub_send_table = &send_tables_[dt_name];
				if (send_prop.flags() & Demo::kSPropCollapsible) {
					GatherPropsInternal(*sub_send_table, excludes, class_num, temp_flattened_props);
				} else {
					GatherProps(*sub_send_table, excludes, class_num);
				}
			}
		} else {
			if (static_cast<SendPropType>(send_prop.type()) == SendPropType::DPT_ARRAY) {
				temp_flattened_props.push_back(FlattenedPropEntry(&send_prop, &(table.props(i - 1))));
			} else {
				temp_flattened_props.push_back(FlattenedPropEntry(&send_prop, 0));
			}
		}
	}
}

void Demo::GatherProps(CSVCMsg_SendTable& table, std::set<std::string>& excludes, uint32 class_num) {
	std::vector<FlattenedPropEntry> temp_flattened_props;

	GatherPropsInternal(table, excludes, class_num, temp_flattened_props);

	std::vector<FlattenedPropEntry>& flattened_props = server_classes_[class_num].GetFlattenedPropsEntry();
	for (auto& entry : temp_flattened_props) {
		flattened_props.push_back(entry);
	}
}

bool Demo::ParseStringTables() {
	int32 data_size = memory_.ReadInt32();
	sm_assert(data_size > 0);

	if (parse_type_ & ParseType::STRING_TABLES) {
		MemoryBitStream data;
		data.Alloc(data_size);
		memory_.ReadBytes(data.GetBufferPointer(), data_size);

		uint8 num_tables = data.ReadByte();

		for (uint8 i = 0; i < num_tables; ++i) {
			std::string table_name = data.ReadString(100);

			ParseStringTable(table_name, data);
		}

	} else {
		memory_.SkipBytes(data_size);
	}

	return true;
}

bool Demo::ParsePacket() {
	command_info_ = CommandInfo::CreateFromMemory(memory_);
	SequenceInfo sequence_info = SequenceInfo::CreateFromMemory(memory_);
	int32 data_size = memory_.ReadInt32();
	
	if (parse_type_ & ParseType::PACKET) {
		MemoryBitStream data;
		data.Alloc(data_size);
		memory_.ReadBytes(data.GetBufferPointer(), data_size);

		while (!data.IsEnd()) {
			int32 command = data.ReadVarInt32();
			int32 size = data.ReadVarInt32();

			// if theres nothing to parse, we just skip
			if (!((parse_type_ & ParseType::NET_TICKS) ||
				(parse_type_ & ParseType::PACKET_ENTITIES) ||
				(parse_type_ & ParseType::SERVER_INFO) ||
				(parse_type_ & ParseType::STRING_TABLES) ||
				(parse_type_ & ParseType::SET_CON_VAR) ||
				(parse_type_ & ParseType::USER_MESSAGE))) {
				data.SkipBytes(size);
				continue;	
			}

			char* data_buffer = new char[size];
			data.ReadBytes(data_buffer, size);

			switch (command) {
				case net_NOP:{} break;
				case net_Tick: 
				{
					if (parse_type_ & ParseType::NET_TICKS) {
						CNETMsg_Tick net_tick;
						net_tick.ParseFromArray(data_buffer, size);
						ParseNetTick(std::move(net_tick));	
					}
				} break;
				case svc_PacketEntities: 
				{
					if (parse_type_ & ParseType::PACKET_ENTITIES) {
						CSVCMsg_PacketEntities packet_entities;
						packet_entities.ParseFromArray(data_buffer, size);
						ParsePacketEntities(std::move(packet_entities));
					}
				} break;
				case svc_ServerInfo:
				{
					if (parse_type_ & ParseType::SERVER_INFO) {
						CSVCMsg_ServerInfo server_info;
						server_info.ParseFromArray(data_buffer, size);
						ParseServerInfo(std::move(server_info));
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
						CSVCMsg_GameEvent game_event;
						game_event.ParseFromArray(data_buffer, size);
						ParseGameEvent(std::move(game_event));
					}
				} break;
				case svc_GameEventList:
				{
					if (parse_type_ & ParseType::GAME_EVENTS) {
						CSVCMsg_GameEventList game_event_list;
						game_event_list.ParseFromArray(data_buffer, size);
						ParseGameEventList(std::move(game_event_list));
						if (use_default_skip_list_) {
							CreateSkipList();
						}
					}
				} break;
				case net_SetConVar:
				{
					if (parse_type_ & ParseType::SET_CON_VAR) {
						CNETMsg_SetConVar set_con_var;
						set_con_var.ParseFromArray(data_buffer, size);
						ParseSetConVar(std::move(set_con_var));
					}
				} break;
				case svc_UserMessage:
				{
					if (parse_type_ & ParseType::USER_MESSAGE) {
						CSVCMsg_UserMessage user_message;
						user_message.ParseFromArray(data_buffer, size);
						ParseUserMessage(std::move(user_message));
					}
				} break;
				default:
				{} break;
			}

			delete[] data_buffer;
		}

	} else {
		memory_.SkipBytes(data_size);
	}

	return true;
}

bool Demo::ParseTickInternal() {
	DemoCommand command = static_cast<DemoCommand>(memory_.ReadByte());

	if (command < DemoCommand::SIGN_ON) {
		std::cerr << "Demo::ParseTickInternal: Missing end tag in demo file" << std::endl;
		return false;
	}

	sm_assert(command >= DemoCommand::SIGN_ON && command <= DemoCommand::LAST_CMD);

	in_game_tick_ = memory_.ReadInt32();
	char player_slot = memory_.ReadByte();

	++num_commands_;
	
	switch (command) {
		case DemoCommand::SYNC_TICK:
		{} break;
		case DemoCommand::STOP:
		{
			std::cout << "CMD STOP" << std::endl;
			return false;
		} break;
		case DemoCommand::CONSOLE_CMD:
		{
			ReadDummyData();
		} break;
		case DemoCommand::DATA_TABLES:
		{
			ProfileBlockStrMS("DataTables");
			ParseDataTables();
		} break;
		case DemoCommand::STRING_TABLES:
		{
			ProfileBlockStrMS("StringTables");
			ParseStringTables();
		} break;
		case DemoCommand::USER_CMD:
		{
			ProfileBlockMS();
			int32 outgoing_sequence = memory_.ReadInt32();
			ReadDummyData();
		} break;
		case DemoCommand::SIGN_ON:
		case DemoCommand::PACKET:
		{
			ParsePacket();
		} break;
		default:
		{
			std::cerr << "Demo::ParseTickInternal: Can't handle command: " << static_cast<int>(command) << std::endl;
		} break;
	}

	return true;
}

bool Demo::ParseStringTable(const std::string& table_name, MemoryBitStream& data) {
	uint16 num_strings = data.ReadInt16();

	if (table_name == "modelprecache") {

	}

	for (uint16 i = 0; i < num_strings; ++i) {
		std::string string_name = data.ReadString(100);

		bool has_data = data.ReadBit();
		
		if (has_data) {
			uint16 data_size = data.ReadInt16();

			if (table_name == "userinfo") {
				MemoryBitStream data_user_info;
				// NOTE(Pedro): Valve default implementation adds 4 to the user data
				// Im guessing that this must be for some odd case
				data_user_info.Alloc(data_size + 4); 
				data.ReadBytes(data_user_info.GetBufferPointer(), data_size);
				ParseUserInfo(std::move(data_user_info));
			} else if (table_name == "modelprecache") {
				data.SkipBytes(data_size);
			} else if (table_name == "instancebaseline") {
				data.SkipBytes(data_size);
			} else {
				data.SkipBytes(data_size);
			}
			
		}
	} // for num_strings

	bool has_client_data = data.ReadBit();

	if (has_client_data) {
		// TODO(Pedro) check if there is a demo with client data
		std::cout << "Demo with Client data" << std::endl;

		uint16 num_strings_client = data.ReadInt16();
		
		for (uint16 i = 0; i < num_strings; ++i) {
			std::string string_name = data.ReadString(100);

			bool has_data = data.ReadBit();

			if (has_data) {
				uint16 data_size = data.ReadInt16();
				// NOTE(Pedro): Check this
				char* data_buffer = new char[data_size];
				data.ReadBytes(data_buffer, data_size);
				delete[] data_buffer;
			}
		}
	}

	return true;
}

bool Demo::ParseUserInfo(MemoryBitStream&& data) {
	players_.emplace_back(Player::CreateFromMemoryBitStream(data));
	return true;
}

bool Demo::ParseNetTick(CNETMsg_Tick&& net_tick) {
	net_ticks_.emplace_back(net_tick.tick(),
							net_tick.host_computationtime(),
							net_tick.host_computationtime_std_deviation(),
							net_tick.host_framestarttime_std_deviation());
	return true;
}

std::vector<NetTick> Demo::GetNetTickList() const {
	return net_ticks_;
}

bool Demo::ParseServerInfo(CSVCMsg_ServerInfo&& server_info) {
	server_info_.protocol = server_info.protocol();
	server_info_.server_count = server_info.server_count();
	server_info_.is_dedicated = server_info.is_dedicated();
	server_info_.is_official_valve_server = server_info.is_official_valve_server();
	server_info_.is_hltv = server_info.is_hltv();
	server_info_.is_replay = server_info.is_replay();
	server_info_.is_redirecting_to_proxy_relay = server_info.is_redirecting_to_proxy_relay();
	server_info_.c_os = server_info.c_os();
	server_info_.map_crc = server_info.map_crc();
	server_info_.client_crc = server_info.client_crc();
	server_info_.string_table_crc = server_info.string_table_crc();
	server_info_.max_clients = server_info.max_clients();
	server_info_.max_classes = server_info.max_classes();
	server_info_.player_slot = server_info.player_slot();
	server_info_.tick_interval = server_info.tick_interval();
	server_info_.game_dir = server_info.game_dir();
	server_info_.map_name = server_info.map_name();
	server_info_.map_group_name = server_info.map_group_name();
	server_info_.sky_name = server_info.sky_name();
	server_info_.host_name = server_info.host_name();
	server_info_.ugc_map_id = server_info.ugc_map_id();
	return true;
}

bool Demo::ParseGameEvent(CSVCMsg_GameEvent&& game_event) {
	int32 event_id = game_event.eventid();
	std::string event_name = game_event_descriptor_[event_id].name;

	// handle player connect/disconnect
	if (event_name == "player_connect" || event_name == "player_disconnect") {

	}

	
	// if parse_warm_up_ is set to false we skip the event
	if (parse_warm_up_ == false &&
		event_id != 40 ) { // 40 -> round_announce_match_start, warmup ended
		return false;
	} 

	// Skip the events on the skip list
	if (skip_game_events_.find(event_name) != skip_game_events_.end()) {
		return false;
	}



	// process our stuff first
	switch (event_id) {
		case 40: // round_announce_match_start
		{
			warm_up_ended_ = true;
		} break;

		default:
		{} break; // Unhandle game event
	}	

	

	// check for any callback
	auto game_event_cb = game_events_callbacks_.find(event_name);
	if (game_event_cb != game_events_callbacks_.end()) {
		GameEvent game_event_data{};
		game_event_cb->second(game_event_data);
	}

	return true;	
}

bool Demo::ParseGameEventList(CSVCMsg_GameEventList&& game_event_list) {
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

		game_event_descriptor_[game_event_descriptor.event_id] = std::move(game_event_descriptor);
	}

	return true;
}

int32 Demo::GameEventNameToEventID(const std::string& event_name) {
	for (auto& event : game_event_descriptor_) {
		if (event.second.name == event_name) {
			return event.second.event_id;
		}
	}

	return -1;
}

void Demo::CreateSkipList() {
	AddGameEventToSkipList("player_footstep"); // player_footstep
	AddGameEventToSkipList("weapon_fire"); // weapon_fire
	AddGameEventToSkipList("bomb_beep"); // bomb_beep
	AddGameEventToSkipList("player_blind"); // player_blind 
	AddGameEventToSkipList("player_jump"); // player_jump
	AddGameEventToSkipList("player_hurt");  // player_hurt
	AddGameEventToSkipList("weapon_reload"); // weapon_reload
	AddGameEventToSkipList("weapon_zoom"); // weapon_zoom
	AddGameEventToSkipList("hltv_status"); // hltv_status
	AddGameEventToSkipList("hltv_chase"); // hltv_chase
}

void Demo::SetGameEventCallback(const std::string& event_name, GameEventCallback callback) {
	game_events_callbacks_[event_name] = std::move(callback);
}

void Demo::RemoveGameEventCallback(const std::string& event_name) {
	auto game_event = game_events_callbacks_.find(event_name);
	if (game_event != game_events_callbacks_.end()) {
		game_events_callbacks_.erase(game_event);
	}
}

void Demo::SwapTeamSide() {
	Team tmp = team_ct_;
	team_ct_ = team_t_;
	team_t_ = tmp;
}

bool Demo::ParseSetConVar(CNETMsg_SetConVar&& set_con_var) {
	uint32 num_con_vars = set_con_var.convars().cvars_size();

	for (uint32 i = 0; i < num_con_vars; ++i) {
		const CMsg_CVars_CVar& con_var = set_con_var.convars().cvars(i);
		con_var_[con_var.name()] = con_var.value();
	}

	return true;
}

bool Demo::ParseUserMessage(CSVCMsg_UserMessage&& user_message) {
	int32 type = user_message.msg_type();
	int32 size = user_message.msg_data().size();

	char* data_buffer = new char[size];

	switch (type) {
		case CS_UM_VGUIMenu:
		case CS_UM_Geiger:
		case CS_UM_Train:
		case CS_UM_HudText:
		{
			//std::cout << "CS_UM_HudText: " << std::endl;
		} break;
		case CS_UM_SayText:
		{
			

		} break;
		case CS_UM_SayText2:
		{
			
			
		} break;
		case CS_UM_TextMsg:
		{	
			
		} break;
		case CS_UM_HudMsg:
		case CS_UM_ResetHud:
		case CS_UM_GameTitle:
		case CS_UM_Shake:
		case CS_UM_Fade:
		case CS_UM_Rumble:
		case CS_UM_CloseCaption:
		case CS_UM_CloseCaptionDirect:
		case CS_UM_SendAudio:
		{
		//	std::cout << "CS_UM_SendAudio: " << std::endl;
		} break;
		case CS_UM_RawAudio:
		{
		//	std::cout << "CS_UM_RawAudio: " << std::endl;
		} break;
		case CS_UM_VoiceMask:
		case CS_UM_RequestState:
		case CS_UM_Damage:
		case CS_UM_RadioText:
		{
			//std::cout << "CS_UM_RadioText: " << std::endl;
		} break;
		case CS_UM_HintText:
		case CS_UM_KeyHintText:
		case CS_UM_ProcessSpottedEntityUpdate:
		case CS_UM_ReloadEffect:
		case CS_UM_AdjustMoney:
		case CS_UM_UpdateTeamMoney:
		case CS_UM_StopSpectatorMode:
		case CS_UM_KillCam:
		case CS_UM_DesiredTimescale:
		case CS_UM_CurrentTimescale:
		case CS_UM_AchievementEvent:
		case CS_UM_MatchEndConditions:
		case CS_UM_DisconnectToLobby:
		case CS_UM_DisplayInventory:
		case CS_UM_WarmupHasEnded:
		{
			//std::cout << "Warmup ended" << std::endl;
		} break;
		case CS_UM_ClientInfo:
		case CS_UM_CallVoteFailed:
		case CS_UM_VoteStart:
		case CS_UM_VotePass:
		case CS_UM_VoteFailed:
		case CS_UM_VoteSetup:
		case CS_UM_SendLastKillerDamageToClient:
		case CS_UM_ItemPickup:
		case CS_UM_ShowMenu:
		case CS_UM_BarTime:
		case CS_UM_AmmoDenied:
		case CS_UM_MarkAchievement:
		case CS_UM_ItemDrop:
		case CS_UM_GlowPropTurnOff:
		{

		} break;
		default:
		{

		} break;
	}

	delete[] data_buffer;

	return true;
}


bool Demo::ParsePacketEntities(CSVCMsg_PacketEntities&& packet_entities) {
	MemoryBitStream data(&packet_entities.entity_data()[0],
						packet_entities.entity_data().size());

	int32 current_entry = -1;
	UpdateType update_type = UpdateType::PRESERVE_ENT;
	bool is_delta = packet_entities.is_delta();
	int32 entry_index = packet_entities.updated_entries();

	while (update_type < UpdateType::FINISHED) {
		--entry_index;

		bool is_entity = (entry_index >= 0) != 0;

		if (is_entity) {
			current_entry = current_entry + 1 + data.ReadUBitVar();

			update_type = UpdateType::DELTA_ENT;

			if (data.ReadBit() == false) {
				if (data.ReadBit() != false) {
					update_type = UpdateType::ENTER_PVS;
				}
			} else {
				update_type = UpdateType::LEAVE_PVS;
				if (data.ReadBit() != false) {
					// do nothing, we need to read the bit anyway
				}
			}
		}

		if (!is_entity || current_entry > Demo::kEntitySentinel) {
			update_type = UpdateType::FINISHED;
		}

		// VPS -> PONTIALLY VISIBLE SYSTEM
		switch (update_type) {
			case UpdateType::ENTER_PVS:
			{	
				// server class
				uint32 class_id = data.ReadUBitLong(server_class_bits_);
				// entity serial (never used)
				uint32 serial_num = data.ReadUBitLong(kNumNetworkedSerialNumberBits);

				sm_assert(class_id < 255);

				AddEntity(current_entry, class_id, serial_num, data);
			} break;
			case UpdateType::DELTA_ENT:
			{
				auto entity = entities_.find(current_entry);
				if (entity != entities_.end()) {
					UpdateEntity(entity->second, data);
				} else {
					std::cerr << "Demo::ParsePacketEntities: Could not get the enity" << std::endl;
					sm_assert(0);
				}
			} break;
			case UpdateType::PRESERVE_ENT:
			{
				if (!is_delta) {
					std::cerr << "Demo::ParsePacketEntities: PRESERVE_ENT on full update" << std::endl;
					sm_assert(0);
				} else {
					if (current_entry >= Demo::kMaxEdicts) {
						std::cerr << "Demo::ParsePacketEntities: PRESERVE_ENT current_entry >= Demo::kMaxEdicts" << std::endl;
						sm_assert(0);
					}
				}
			} break;
			case UpdateType::LEAVE_PVS:
			{
				if (!is_delta) {
					std::cerr << "Demo::ParsePacketEntities: LEAVE_PVS on full update" << std::endl;
					sm_assert(0);
				} else {
					RemoveEntity(current_entry);
				}
			} break;
			default:
			{} break;
		}
	}

	return true;
}

void Demo::AddEntity(uint32 id, uint32 class_id, uint32 serial_num, MemoryBitStream& data) {
	auto entity = entities_.find(id);

	if (entity != entities_.end()) {
		entity->second.class_id = class_id;
		entity->second.serial_number = serial_num;
		ParseEntity(entity->second, std::move(data));
	} else {
		entities_[id] = EntityEntry{id, class_id, serial_num};
		ParseEntity(entities_[id], std::move(data));
	}
}

void Demo::ParseEntity(EntityEntry& entity, MemoryBitStream& data) {
	bool new_way = data.ReadBit();
	int32 index = -1;

	CSVCMsg_SendTable* table = GetTableByClassID(entity.class_id);

	if (!table) {
		std::cerr << "Demo::ParseEntity: Error getting the table " << std::endl;
		return;
	}

	std::vector<int32> field_indexs;

	while ((index = ReadFieldIndex(data, new_way, index)) != -1 ) {
		if (index != -1) {
			field_indexs.push_back(index);
		}
	}

	for (uint32 i = 0, size = field_indexs.size(); i < size; ++i) {
		FlattenedPropEntry* send_prop = &server_classes_[entity.class_id].GetFlattenedPropsEntry()[field_indexs[i]];

		if (send_prop) {
			Prop* prop = Decoder::Prop(data, *send_prop, entity.class_id, field_indexs[i]);
			PropEntry prop_entry(*prop, send_prop);
	
		} else {
			std::cerr << "Demo::ParseEntity: Error getting the send prop pointer" << std::endl;
			return;
		}
	}

}

void Demo::UpdateEntity(EntityEntry& entity, MemoryBitStream& data) {
	ParseEntity(entity, std::move(data));	
}

void Demo::RemoveEntity(uint32 id) {
	auto entity = entities_.find(id);
	if (entity != entities_.end()) {
		entities_.erase(entity);
	}
}

int32 Demo::ReadFieldIndex(MemoryBitStream& data, bool new_way, uint32 index) {
	if (new_way) {
		if (data.ReadBit()) {
			return index + 1;
		}
	}

	int32 result = 0;
	if (new_way && data.ReadBit()) {
		result = data.ReadUBitLong(3);
	} else {
		result = data.ReadUBitLong(7);

		switch (result & (32 | 64)) {
			case 32:
			{
				result = (result &~96) | (data.ReadUBitLong(2) << 5);
				sm_assert(result >= 32);
			} break;
			case 64:
			{
				result = (result &~96) | (data.ReadUBitLong(4) << 5);
				sm_assert(result >= 128);
			} break;
			case 96:
			{
				result = (result &~96) | (data.ReadUBitLong(7) << 5);
				sm_assert(result >= 512);
			} break;
		}
	}

	if (result == 0xFFF) { // end marker is 4095 for CSGO
		return -1;
	}

	return index + 1 + result;
}

CSVCMsg_SendTable* Demo::GetTableByClassID(uint32 class_id) {
	for (uint32 i = 0, size = server_classes_.size(); i < size; ++i) {
		if (server_classes_.at(i).GetClassID() == class_id) {
			return &(send_tables_[server_classes_.at(i).GetDTName()]);
		}
	}

	return 0;
}