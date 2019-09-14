#ifndef CSGOTOOLS_DECODER_HPP_
#define CSGOTOOLS_DECODER_HPP_

#include <MemoryBitStream.hpp>

#include "Prop.hpp"

#include "netmessages_public.pb.h"

namespace csgotools {

	struct Decoder {

		static constexpr int32 kSPropVarInt = (1 << 19);
		static constexpr int32 kSPropUnsigned = (1 << 0);
		static constexpr int32 kSPropCoord = (1 << 1);
		static constexpr int32 kSPropCoordMP = (1 << 12);
		static constexpr int32 kSPropCoordMPLowPrecision = (1 << 13);
		static constexpr int32 kSPropCoordMPIntegral = (1 << 14);
		static constexpr int32 kSPropNoScale = (1 << 2);
		static constexpr int32 kSPropNormal = (1 << 5);
		static constexpr int32 kSPropCellCoord = (1 << 15);
		static constexpr int32 kSPropCellCoordLowPrecision = (1 << 16);
		static constexpr int32 kSPropCellCoordIntegral = (1 << 17);
		static constexpr int32 kDTMaxStringBits = 9;
		static constexpr int32 kDTMaxStringBuffer = (1 << kDTMaxStringBits);


		Decoder() = delete;

		static csgotools::Prop* Prop(MemoryBitStream& data, FlattenedPropEntry& flattened_prop_entry, uint32 class_id, int32 field_index);
		static int32 Int(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop);
		static bool SpecialFloat(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop, float& value);
		static float Float(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop);
		static csgotools::Vector Vector(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop);
		static csgotools::Vector VectorXY(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop);
		static std::string String(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop);
		static int64 Int64(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop);
		static csgotools::Prop* Array(MemoryBitStream& data, FlattenedPropEntry& flattened_prop_entry, int32 num_elements, uint32 class_id, int32 field_index);	
	};

}

#endif
