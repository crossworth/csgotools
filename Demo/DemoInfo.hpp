#ifndef CSGOTOOLS_DEMO_INFO_HPP_
#define CSGOTOOLS_DEMO_INFO_HPP_

#pragma warning(disable:4996) // disable warning std::localtime

#include<ctime>
#include <iomanip>
#include <sstream>

#include <Steam.hpp>
#include <MemoryBitStream.hpp>

#include "cstrike15_gcmessages.pb.h"

// CDataGCCStrike15_v2_MatchInfo
// match730_reservationid_tvport_serverip.dem.info
// https://github.com/CSGO-Analysis/CSGO-Analyzer
namespace csgotools {
	class DemoInfo {
	public:
        DemoInfo() = default;
        DemoInfo(const std::string& file_name) { Open(file_name); }

		bool Open(const std::string& file_name);
        std::time_t GetDateTimestamp() const { return match_date_; }
        std::string GetDateUTC() const;
        std::string GetDateGMT() const;
        uint64 GetMatchID() const { return match_id_; }
        
    private:
        bool ParseDemoInfo();

        bool is_valid_{ false };
        size_t file_size_{};
        std::time_t match_date_{};
        int64 match_id_{ 0 };
        int32 number_spectators_{ 0 };
        int64 decryptdata_key_pub_{ 0 };

        MemoryBitStream memory_;
        std::string demo_info_name_;
	};
}

#endif
