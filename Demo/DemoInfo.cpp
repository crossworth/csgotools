#include "DemoInfo.hpp"

using namespace csgotools;

bool DemoInfo::Open(const std::string& file_name) {
    std::string string_buffer;

    std::ifstream file;
    file.open(file_name.c_str(), std::ios::in | std::ios::binary);

    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        file_size_ = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        if (file_size_ <= 0) {
            std::cerr << "DemoInfo::Open: File too small: " << file_size_ << " bytes" << std::endl;
            file.close();
            return false;
        }

        string_buffer.resize(file_size_);
        file.read(&string_buffer[0], file_size_);
        file.close();

        memory_ = MemoryBitStream(string_buffer.c_str(), file_size_);
        ParseDemoInfo();
    } else {
        std::cerr << "DemoInfo::Open: Could not open the file: " << file_name << std::endl;
        return false;
    }

    return true;
}

bool DemoInfo::ParseDemoInfo() {
    CDataGCCStrike15_v2_MatchInfo data;

    data.ParseFromArray(memory_.GetBufferPointer(), file_size_);
    if (data.has_matchid() && data.has_matchtime()) {
        match_date_ = data.matchtime();
        match_id_ = data.matchid();
        decryptdata_key_pub_ = data.watchablematchinfo().cl_decryptdata_key_pub();
        number_spectators_ = data.watchablematchinfo().tv_spectators();
        
        for (int32 i = 0; i < data.roundstatsall_size(); ++i) {
            auto round_stats = data.roundstatsall(i);
            CT_score_ = round_stats.team_scores(0);
            T_score_ = round_stats.team_scores(1);
           
            if (round_stats.has_match_result()) {
                // figuerout the results types
                // 0 TIE
                // 1 CT-T Win
                // 2 T - Win
                if (round_stats.match_result() == 0) {
                    result_ = MatchResult::TIE;
                }

                if (round_stats.match_result() == 1) {
                    result_ = MatchResult::CT_WIN;
                }

                if (round_stats.match_result() == 2) {
                    result_ = MatchResult::T_WIN;
                }
            }

            Round round(i + 1, CT_score_, T_score_);
            round.SetDuration(round_stats.match_duration() - match_duration_);

            auto& ct_players = round.GetCTPlayers();
            auto& t_players = round.GetTPlayers();
            
            // NOTE(Pedro): Here we make assumptions that the CDataGCCStrike15_v2_MatchInfo contains valid 
            // information and has everthing that we need
            // TODO(Pedro): Check a bot game to see if this can cause any trouble
            for (int32 j = 0; j < round_stats.reservation().account_ids_size(); ++j) {
                auto* players = &t_players;

                if (j < 5) {
                    players = &ct_players;
                }

                SteamID steam_id(round_stats.reservation().account_ids(j));
                (*players)[steam_id.GetSteamID64()] = Player();
                (*players)[steam_id.GetSteamID64()].SetSteamID(steam_id);
                
                if (j < round_stats.kills_size()) {
                    (*players)[steam_id.GetSteamID64()].SetKills(round_stats.kills(j));
                }

                if (j < round_stats.assists_size()) {
                    (*players)[steam_id.GetSteamID64()].SetAssists(round_stats.assists(j));
                }

                if (j < round_stats.deaths_size()) {
                    (*players)[steam_id.GetSteamID64()].SetDeaths(round_stats.deaths(j));
                }

                if (j < round_stats.scores_size()) {
                    (*players)[steam_id.GetSteamID64()].SetScore(round_stats.scores(j));
                }

                if (j < round_stats.scores_size()) {
                    (*players)[steam_id.GetSteamID64()].SetScore(round_stats.scores(j));
                }
                
                if (j < round_stats.mvps_size()) {
                    (*players)[steam_id.GetSteamID64()].SetMVPs(round_stats.mvps(j));
                }

                if (j < round_stats.enemy_headshots_size()) {
                    (*players)[steam_id.GetSteamID64()].SetNumberHeadshots(round_stats.enemy_headshots(j));
                }
            }

            rounds_.push_back(std::move(round));
            match_duration_ = round_stats.match_duration();
        }

        is_valid_ = true;
        return true;
    } else {
        std::cerr << "DemoInfo::ParseDemoInfo: Could not read the match id or the match time" << std::endl;
        is_valid_ = false;
    }

    return false;
}

std::string DemoInfo::GetDateGMT() const {
    if (!is_valid_) {
        return "";
    }

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&match_date_), "%c %Z");
    return ss.str();
}

std::string DemoInfo::GetDateUTC() const {
    if (!is_valid_) {
        return "";
    }

    std::stringstream ss;
    ss << std::put_time(std::localtime(&match_date_), "%c %Z");
    return ss.str();
}