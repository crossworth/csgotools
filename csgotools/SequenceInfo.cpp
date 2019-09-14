#include "SequenceInfo.hpp"

using namespace csgotools;

SequenceInfo SequenceInfo::CreateFromMemory(MemoryBitStream& memory) {
    SequenceInfo sequence;
    sequence.seq_nr_in_ = memory.ReadInt32();
    sequence.seq_nr_out_ = memory.ReadInt32();
    return sequence;
}

SequenceInfo::operator std::string() const {
    std::stringstream out;
    out << "------------SEQUENCE INFO------------" << std::endl;
    out << "SeqNrIn" << seq_nr_in_ << std::endl;
    out << "SeqNrOut" << seq_nr_out_ << std::endl;
    out << "------------SEQUENCE INFO------------" << std::endl;
    return out.str();
}

std::ostream& csgotools::operator<<(std::ostream& out, const SequenceInfo& sequence_info) {
    out << static_cast<std::string>(sequence_info);
    return out;
}
