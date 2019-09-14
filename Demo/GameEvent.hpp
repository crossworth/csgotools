#ifndef CSGOTOOLS_GAME_EVENT_HPP_
#define CSGOTOOLS_GAME_EVENT_HPP_

#include <iostream>
#include <vector>

#include <Steam.hpp>

namespace csgotools {


	struct GameEventDescriptorKey {
		int32 type{};
		std::string name;
	};

	struct GameEventDescriptor {
		int32 event_id{};
		std::string name;
		std::vector<GameEventDescriptorKey> keys;
	};

	struct GameEventData {
		union Data {
			std::string string_value;
			float float_value;
			int32 long_value;
			int32 short_value;
			int32 byte_value;
			bool bool_value;
			uint64 uint64_value;
			int8* wstring_value;

			~Data() {
				if (wstring_value) {
					delete[] wstring_value;
				}
			}
		};

		Data value;
		int32 type;
	};

	struct GameEvent {
		int32 event_id;
		std::string name;

		std::vector<GameEventData> data;
	};

}
#endif

