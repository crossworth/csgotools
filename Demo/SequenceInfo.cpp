#include "SequenceInfo.hpp"

using namespace csgosdk;

SequenceInfo SequenceInfo::CreateFromMemory(MemoryBitStream & memory) {
	SequenceInfo sequence;
	sequence.seq_nr_in_ = memory.ReadInt32();
	sequence.seq_nr_out_ = memory.ReadInt32();
	return sequence;
}
