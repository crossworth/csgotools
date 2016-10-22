#ifndef CSGOTOOLS_SEQUENCE_INFO_HPP_
#define CSGOTOOLS_SEQUENEC_INFO_HPP_

#include <Steam.hpp>
#include <MemoryBitStream.hpp>

namespace csgotools {

	class SequenceInfo {
	public:
		SequenceInfo() = default;
		
		int32 GetSeqNrIn() const { return seq_nr_in_; }
		int32 GetSeqNrOut() const { return seq_nr_out_; }
		void SetSeqNrIn(int32 in) { seq_nr_in_ = in; }
		void SetSeqNrOut(int32 out) { seq_nr_out_ = out; }

		static SequenceInfo CreateFromMemory(MemoryBitStream& memory);
	private:
		int32 seq_nr_in_{};
		int32 seq_nr_out_{};
	};

}

#endif

