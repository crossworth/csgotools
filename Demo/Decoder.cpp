#include "Decoder.hpp"

using namespace csgotools;

Prop* Decoder::Prop(MemoryBitStream& data, FlattenedPropEntry& flattened_prop_entry,
			uint32 class_id, int field_index) {

	const CSVCMsg_SendTable::sendprop_t* send_prop = flattened_prop_entry.prop;
	csgotools::Prop* result = new csgotools::Prop();

	SendPropType type = static_cast<SendPropType>(send_prop->type());
	if (type != SendPropType::DPT_ARRAY &&
		type != SendPropType::DPT_DATA_TABLE) {
		result->SetType(type);
	}

	switch (type) {
		case SendPropType::DPT_INT:
		{
			result->value.int_value = Decoder::Int(data, send_prop);
		} break;
		case SendPropType::DPT_FLOAT:
		{
			result->value.float_value = Decoder::Float(data, send_prop);
		} break;
		case SendPropType::DPT_VECTOR:
		{
			result->value.vector_value = Decoder::Vector(data, send_prop);
		} break;
		case SendPropType::DPT_VECTOR_XY:
		{
			result->value.vector_value = Decoder::VectorXY(data, send_prop);
		} break;
		case SendPropType::DPT_STRING:
		{
			result->string_value = Decoder::String(data, send_prop);
			result->value.string_value = &result->string_value[0];
		} break;
		case SendPropType::DPT_ARRAY:
		{
			result = Decoder::Array(data, flattened_prop_entry, send_prop->num_elements(), class_id, field_index);
		} break;
		case SendPropType::DPT_DATA_TABLE:
		{} break;
		case SendPropType::DPT_INT64:
		{
			result->value.int64_value = Decoder::Int64(data, send_prop);
		} break;
		default:
		{} break;
	}

	return result;
}

int32 Decoder::Int(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop) {
	int32 flags = prop->flags();

	if (flags & Decoder::kSPropVarInt) {
		if (flags & Decoder::kSPropUnsigned) {
			return data.ReadVarInt32();
		} else {
			return data.ReadSignedVarInt32();
		}
	} else {
		if (flags & Decoder::kSPropUnsigned) {
			return data.ReadUBitLong(prop->num_bits());
		} else {
			return data.ReadSBitLong(prop->num_bits());
		}
	}
}

bool Decoder::SpecialFloat(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop, float& value) {
	int32 flags = prop->flags();

	if (flags & Decoder::kSPropCoord) {
		value = data.ReadBitCoord();
		return true;
	} else if (flags & Decoder::kSPropCoordMP) {
		value = data.ReadBitCoordMP(MemoryBitStream::BitCoordType::CW_NONE);
		return true;
	} else if (flags & Decoder::kSPropCoordMPLowPrecision) {
		value = data.ReadBitCoordMP(MemoryBitStream::BitCoordType::CW_LOW_PRECISION);
		return true;
	} else if (flags & Decoder::kSPropCoordMPIntegral) {
		value = data.ReadBitCoordMP(MemoryBitStream::BitCoordType::CW_INTEGRAL);
		return true;
	} else if (flags & Decoder::kSPropNoScale) {
		value = data.ReadBitFloat();
		return true;
	} else if (flags & Decoder::kSPropNormal) {
		value = data.ReadBitNormal();
		return true;
	} else if (flags & Decoder::kSPropCellCoord) {
		value = data.ReadBitCellCoord(prop->num_bits(), MemoryBitStream::BitCoordType::CW_NONE);
		return true;
	} else if (flags & Decoder::kSPropCellCoordLowPrecision) {
		value = data.ReadBitCellCoord(prop->num_bits(), MemoryBitStream::BitCoordType::CW_LOW_PRECISION);
		return true;
	} else if (flags & Decoder::kSPropCellCoordIntegral) {
		value = data.ReadBitCellCoord(prop->num_bits(), MemoryBitStream::BitCoordType::CW_INTEGRAL);
		return true;
	}

	return false;
}

float Decoder::Float(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop) {
	float result = 0.0f;
	uint32 interp;

	if (Decoder::SpecialFloat(data, prop, result)) {
		return result;
	}

	interp = data.ReadUBitLong(prop->num_bits());
	result = static_cast<float>(interp) / ((1 << prop->num_bits()) - 1);
	result = prop->low_value() + (prop->high_value() - prop->low_value()) * result;
	return result;
}

csgotools::Vector Decoder::Vector(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop) {
	csgotools::Vector vect;
	vect.x = Decoder::Float(data, prop);
	vect.y = Decoder::Float(data, prop);

	if ((prop->flags() & Decoder::kSPropNormal) == 0) {
		vect.z = Decoder::Float(data, prop);
	} else {
		int sign_bit = data.ReadBit();

		float v0v0v1v1 = vect.x * vect.x + vect.y * vect.y;

		if (v0v0v1v1 < 1.0f) {
			vect.z = sqrtf(1.0f - v0v0v1v1);
		} else {
			vect.z = 0.0f;
		}

		if (sign_bit) {
			vect.z *= -1.0f;
		}
	}
	return vect;
}

csgotools::Vector Decoder::VectorXY(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop) {
	csgotools::Vector vect;
	vect.x = Decoder::Float(data, prop);
	vect.y = Decoder::Float(data, prop);
	return vect;
}

std::string Decoder::String(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop) {
	int32 len = data.ReadUBitLong(Decoder::kDTMaxStringBits);
	char* data_string = new char[len + 1];

	if (len >= Decoder::kDTMaxStringBuffer) {
		std::cerr << "Decoder::String: String too big" << std::endl;
		len = Decoder::kDTMaxStringBuffer - 1;
	}

	data.ReadBytes(data_string, len);
	data_string[len] = 0;
	std::string result(data_string);
	delete[] data_string;
	return result;
}

int64 Decoder::Int64(MemoryBitStream& data, const CSVCMsg_SendTable::sendprop_t* prop) {
	if (prop->flags() & Decoder::kSPropVarInt) {
		if (prop->flags() & Decoder::kSPropUnsigned) {
			return static_cast<int64>(data.ReadVarInt64());
		} else {
			return data.ReadSignedVarInt64();
		}
	} else {
		uint32 high_int = 0;
		uint32 low_int = 0;
		bool neg = false;

		if (!(prop->flags() & Decoder::kSPropUnsigned)) {
			neg = data.ReadBit() != 0;
			low_int = data.ReadUBitLong(32);
			high_int = data.ReadUBitLong(prop->num_bits() - 32 - 1);
		} else {
			low_int = data.ReadUBitLong(32);
			high_int = data.ReadUBitLong(prop->num_bits() - 32);
		}
	

		int64 temp;
		uint32* p_int = reinterpret_cast<uint32*>(&temp);
		*p_int++ = low_int;
		*p_int = high_int;

		if (neg) {
			temp = -temp;
		}

		return temp;
	}
}

csgotools::Prop* Decoder::Array(MemoryBitStream& data, FlattenedPropEntry& flattened_prop_entry, int32 num_elements, uint32 class_id, int32 field_index) {
	int32 max_elements = num_elements;
	int32 num_bits = 1;

	while((max_elements >>= 1) != 0) {
		num_bits++;
	}

	int32 elements = data.ReadUBitLong(num_bits);

	csgotools::Prop* result = nullptr;
	result = new csgotools::Prop[num_elements];

	for (int32 i = 0; i < elements; ++i) {
		FlattenedPropEntry temp(flattened_prop_entry.array_element_prop, 0);
		csgotools::Prop* element_result = Decoder::Prop(data, temp, class_id, field_index);
		result[i] = *element_result;
		delete element_result;
		result[i].num_elements = num_elements - i;
	}

	return result;
}