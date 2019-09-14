#pragma once

#include <string>
#include <cstdint>
#include <algorithm>


#ifdef _DEBUG
#ifdef _MSC_VER
#define assert(expr)  do {if (!(bool)(expr)) { __debugbreak();}} while(0)
#elif defined(__MACH__)
#define assert(expr)  do {if (!(bool)(expr)) { __builtin_trap();}} while(0)
#else
#define assert(expr)  do {if (!(bool)(expr)) { (*(u32*)0) = 0xDeAdBeEf;}} while(0)
#endif
#else
#define assert(expr)
#endif


namespace csgotools {

    using uint64 = uint64_t;
    using int64 = int64_t;
    using uint32 = uint32_t;
    using int32 = int32_t;
    using uint16 = uint16_t;
    using int16 = int16_t;
    using uint8 = uint8_t;
    using int8 = int8_t;

    // For more info: https://developer.valvesoftware.com/wiki/SteamID

    class SteamID {
        uint64 steam_id64_{ 0 };
        static constexpr uint64 kBaseSteam64 = 76561197960265728;

        static bool Is64Bits(uint64 steam_id) {
            return steam_id >> 32 > 0;
        }

        static std::string ToUpper(const std::string& str) {
            std::string tmp{ str };
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
            return tmp;
        }
    public:
        SteamID() = default;

        explicit SteamID(uint64 steam_id64) {
            steam_id64_ = Is64Bits(steam_id64) ? steam_id64 : steam_id64_ = steam_id64 + SteamID::kBaseSteam64;
        }


        explicit SteamID(std::string steam_id) {
            steam_id64_ = 0;

            if (IsValidSteam3(steam_id)) {
                steam_id64_ = SteamID::SteamID3ToSteamID64(steam_id);
            } else if (IsValidSteam32(steam_id)) {
                steam_id64_ = SteamID::SteamID32ToSteamID64(steam_id);
            } else {
                // Check if its a valid steam64 as string
                for (char num : steam_id) {
                    if (num < '0' || num > '9') {
                        return; // Just return if not valid
                    }
                }

                steam_id64_ = std::stoll(steam_id);
            }
        }

        void SteamID64(uint64 steam_id64) {
            steam_id64_ = steam_id64;
        }

        uint64 SteamID64() const {
            return steam_id64_;
        }

        std::string SteamID32() const {
            return SteamID::SteamID64ToSteamID32(steam_id64_);
        }

        std::string SteamID3() const {
            return SteamID::SteamID64ToSteam3(steam_id64_);
        }

        explicit operator std::string() const {
            return "SteamID64: " + std::to_string(steam_id64_);
        }

        static bool IsSteam3Group(const std::string& steam_id3) {
            return steam_id3[0] == '[' || steam_id3[1] == 'G' ||
                steam_id3[2] == ':' || steam_id3[4] == ':' ||
                steam_id3[3] >= '0' || steam_id3[3] <= '9';
        }

        static bool IsSteam3Individual(const std::string& steam_id3) {
            return steam_id3[0] == '[' || steam_id3[1] == 'U' ||
                steam_id3[2] == ':' || steam_id3[4] == ':' ||
                steam_id3[3] >= '0' || steam_id3[3] <= '9';
        }

        static bool IsValidSteam3(const std::string& steam_id3) {
            std::string steam_id3_upper(ToUpper(steam_id3));

            if (!IsSteam3Group(steam_id3_upper) || !IsSteam3Individual(steam_id3_upper)) {
                return false;
            }

            std::string account_number = steam_id3_upper.substr(5, steam_id3_upper.length() - 6);

            for (char num : account_number) {
                if (num < '0' || num > '9') {
                    return false;
                }
            }

            return true;
        }

        static bool IsValidSteam32(const std::string& steam_id32) {
            std::string steam_id32_upper(ToUpper(steam_id32));

            if (steam_id32_upper[0] != 'S' || steam_id32_upper[1] != 'T' ||
                steam_id32_upper[2] != 'E' || steam_id32_upper[3] != 'A' ||
                steam_id32_upper[4] != 'M' || steam_id32_upper[5] != '_' ||
                steam_id32_upper[7] != ':' || steam_id32_upper[9] != ':') {
                return false;
            }

            if ((steam_id32_upper[6] < '0' || steam_id32_upper[6] > '9') ||
                (steam_id32_upper[8] < '0' || steam_id32_upper[8] > '9')) {
                return false;
            }

            std::string account_number = steam_id32_upper.substr(10);

            for (char num : account_number) {
                if (num < '0' || num > '9') {
                    return false;
                }
            }

            return true;
        }

        static uint64 SteamID32ToSteamID64(const std::string& steam_id32) {
            if (IsValidSteam32(steam_id32)) {
                uint64 account_number = std::stoll(steam_id32.substr(10));
                return (account_number * 2) + SteamID::kBaseSteam64;
            }

            return 0;
        }

        static uint64 SteamID3ToSteamID64(const std::string& steam_id3) {
            if (IsValidSteam3(steam_id3)) {
                return SteamID::kBaseSteam64 + std::stoll(steam_id3.substr(5, steam_id3.length() - 6));
            }

            return 0;
        }

        static std::string SteamID64ToSteam3(uint64 steam_id64, bool individual = true) {
            char type = (individual) ? 'U' : 'g';

            if (steam_id64 == 0) {
                return{ "[" + std::to_string(type) + ":1:0]" };
            }

            uint64 account_number = (steam_id64 - (SteamID::kBaseSteam64 + (steam_id64 % 2))) / 2;
            int id_number = (steam_id64 % 2 != 0) ? 1 : 0;
            return{ "[" + std::to_string(type) + ":1:" + std::to_string((account_number * 2) + id_number) + "]" };
        }

        static std::string SteamID64ToSteamID32(uint64 steam_id64) {
            if (steam_id64 == 0) {
                return{ "STEAM_1:0:0" };
            }

            uint64 account_number = (steam_id64 - (SteamID::kBaseSteam64 + (steam_id64 % 2))) / 2;
            int id_number = (steam_id64 % 2 != 0) ? 1 : 0;
            // NOTE(Pedro) When we parse the demo CSGO give us STEAM_1 as SteamID32	(Universe Public)
            // Apparently STEAM_0 was a bug/problem with some Source games
            // http://store.steampowered.com/news/2252
            return{ "STEAM_1:" + std::to_string(id_number) + ":" + std::to_string(account_number) };
        }
    };

    inline std::ostream& operator<<(std::ostream& out, const SteamID& steam) {
        out << static_cast<std::string>(steam);
        return out;
    }

}

