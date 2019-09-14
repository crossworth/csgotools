#pragma once

#include <sstream>

#include "Steam.hpp"
#include "MemoryBitStream.hpp"

namespace csgotools {

    class SequenceInfo {
        int32 seq_nr_in_{};
        int32 seq_nr_out_{};

    public:
        SequenceInfo() = default;

        int32 SeqNrIn() const { return seq_nr_in_; }
        int32 SeqNrOut() const { return seq_nr_out_; }
        void SeqNrIn(int32 in) { seq_nr_in_ = in; }
        void SeqNrOut(int32 out) { seq_nr_out_ = out; }

        static SequenceInfo CreateFromMemory(MemoryBitStream& memory);

        operator std::string() const;
    };

    std::ostream& operator<<(std::ostream& out, const SequenceInfo& sequence_info);

}
