#ifndef CSGOTOOLS_DEMO_INFO_HPP_
#define CSGOTOOLS_DEMO_INFO_HPP_

#pragma warning(disable:4996) // disable warning std::localtime

#include<ctime>
#include <vector>
#include <iomanip>
#include <sstream>

#include <Steam.hpp>
#include <MemoryBitStream.hpp>

#include "cstrike15_gcmessages.pb.h"

#include "Round.hpp"

namespace csgotools {
	class DemoInfo {
	public:

        enum class MatchResult {
            TIE,
            CT_WIN,
            T_WIN,
            NONE
        };

        DemoInfo() = default;
        DemoInfo(const std::string& file_name) { Open(file_name); }

		bool Open(const std::string& file_name);
        std::time_t GetDateTimestamp() const { return match_date_; }
        std::string GetDateUTC() const;
        std::string GetDateGMT() const;
        uint64 GetMatchID() const { return match_id_; }
        std::time_t GetMatchDuration() const { return match_duration_; }
        int32 GetNumberSpectators() const { return number_spectators_; }

        int32 GetCTScore() const { return CT_score_; }
        int32 GetTScore() const { return T_score_; }
        MatchResult GetMatchResult() const { return result_; }

        Round GetRound(uint32 number) const { return rounds_[number]; }
        std::vector<Round> GetRounds() const { return rounds_; }
        uint32 GetNumberRounds() const { return rounds_.size(); }
        
    private:
        bool ParseDemoInfo();

        bool is_valid_{ false };
        size_t file_size_{};
        std::time_t match_date_{};
        std::time_t match_duration_{};
        int64 match_id_{ 0 };
        int32 number_spectators_{ 0 };
        int64 decryptdata_key_pub_{ 0 };
        int32 CT_score_{ 0 };
        int32 T_score_{ 0 };
        MatchResult result_{ MatchResult::NONE };
        std::vector<Round> rounds_;

        MemoryBitStream memory_;
        std::string demo_info_name_;
	};
}

#endif
