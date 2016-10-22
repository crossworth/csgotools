#ifndef CSGOTOOLS_FLATTENED_PROP_ENTRY_HPP_
#define CSGOTOOLS_FLATTENED_PROP_ENTRY_HPP_

#include "generated_proto\netmessages_public.pb.h"

namespace csgotools {
	struct FlattenedPropEntry {
		FlattenedPropEntry(const CSVCMsg_SendTable::sendprop_t* prop,
			const CSVCMsg_SendTable::sendprop_t* array_element_prop) : prop(prop),
			array_element_prop(array_element_prop) {}

		const CSVCMsg_SendTable::sendprop_t* prop;
		const CSVCMsg_SendTable::sendprop_t* array_element_prop;
	};
}

#endif