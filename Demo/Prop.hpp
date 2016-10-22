#ifndef CSGOTOOLS_PROP_HPP_
#define CSGOTOOLS_PROP_HPP_

#include <Steam.hpp>
#include <Vector.hpp>
#include "FlattenedPropEntry.hpp"

namespace csgotools {

	enum class SendPropType {
		DPT_INT = 0,
		DPT_FLOAT,
		DPT_VECTOR,
		DPT_VECTOR_XY, // Only encodes the XY of a vector, ignores Z
		DPT_STRING,
		DPT_ARRAY,	// An array of the base types (can't be of datatables).
		DPT_DATA_TABLE,
		DPT_INT64,
		DPT_NUM_SEND_PROPS_TYPE
	};

	struct Prop {
		union PropValue {
			int32 int_value;
			float float_value;
			char* string_value;
			int64 int64_value;
			Vector vector_value;
		};

		std::string string_value;

		SendPropType type;
		PropValue value{};
		int32 num_elements{};

		Prop() = default;
		Prop(SendPropType type) : type(type), num_elements(0) {}

		void SetType(SendPropType type) {
			this->type = type;
		}
	};

	struct PropEntry {
		Prop prop_value;
		FlattenedPropEntry* flattened_prop;

		PropEntry(Prop prop_value, FlattenedPropEntry* flattened_prop) 
		: prop_value(prop_value), flattened_prop(flattened_prop) {}
	};
}

#endif