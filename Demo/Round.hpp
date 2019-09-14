#ifndef CSGOTOOLS_ROUND_HPP_
#define CSGOTOOLS_ROUND_HPP_

#include <unordered_map>
#include <ctime>

#include <Steam.hpp>

#include "Player.hpp"

namespace csgotools {

    class Round {
    public:
        Round(uint32 round_number, uint32 ct_score = 0, uint32 t_score = 0) : round_number_(round_number),
        ct_score_(ct_score), t_score_(t_score) {}

        void SetDuration(const std::time_t& duration) { duration_ = duration; }
        std::time_t GetDuration() const { return duration_; }

        std::unordered_map<uint64, Player>& GetCTPlayers() { return ct_players_; }
        std::unordered_map<uint64, Player>& GetTPlayers() { return t_players_; }

        std::unordered_map<uint64, Player> GetAllPlayers() const {
            auto result = ct_players_;
            for (auto p = t_players_.begin(); p != t_players_.end(); ++p) {
                result[p->first] = p->second;
            }
            return result; 
        }

    private:
        uint32 round_number_{0};
        uint32 ct_score_{0};
        uint32 t_score_{0};
        std::unordered_map<uint64, Player> ct_players_;
        std::unordered_map<uint64, Player> t_players_;
        std::time_t duration_;
    };

}

#endif