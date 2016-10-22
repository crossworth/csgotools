#ifndef CSGOTOOLS_STEAM_HPP_
#define CSGOTOOLS_STEAM_HPP_

#include <iostream>
#include <string>
#include <cstdint>
#include <algorithm>


namespace csgotools {
	
	using uint64 = uint64_t;
	using int64 = int64_t;
	using uint32 = uint32_t;
	using int32 = int32_t;
	using uint16 = uint16_t;
	using int16 = int16_t;
	using uint8 = uint8_t;
	using int8 = int8_t;
	
	// This can/should be buggy if used with groups
	// For more info: https://developer.valvesoftware.com/wiki/SteamID

	class SteamID {
	public:
		SteamID() = default;

		SteamID(uint64 steam_id64) {
			steam_id64_ = steam_id64;
		}

		SteamID(std::string steam_id) {
			steam_id64_ = 0;

			if (IsValidSteam32(steam_id)) {
				steam_id64_ = SteamID::SteamID32ToSteamID64(steam_id);
			} else if (IsValidSteam3(steam_id)) {
				std::cerr << "SteamID::SteamID: There is no way to convert SteamID3 to Steam64 or Steam32" << std::endl;
			} else {
				bool is_steam64 = true;

				for (char c : steam_id) {
					if (c < '0' || c > '9') {
						is_steam64 = false;
						break;
					}
				}

				if (is_steam64) {
					steam_id64_ = std::stoll(steam_id);
				}
			}
		}

		void SetSteamID64(uint64 steam_id64) {
			steam_id64_ = steam_id64;
		}

		uint64 GetSteamID64() const {
			return steam_id64_;
		}

		std::string GetSteamID32() const {
			return SteamID::SteamID64ToSteamID32(steam_id64_);
		}

		std::string GetSteamID3() const {
			return SteamID::SteamID64ToSteam3(steam_id64_);
		}

		static bool IsValidSteam3(const std::string& steam_id32) {
			std::string tmp(steam_id32);
			std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);

			// NOTE(Pedro) This is not fully implemented
			// since We should check if its a group
			// [g:1:4] 	<- It's a valid Steam3ID
			// http://steamcommunity.com/gid/[g:1:4]
			if (tmp[0] != '[' || tmp[1] != 'U' ||
				tmp[2] != ':' || tmp[4] != ':') {
				return false;
			}

			if ((tmp[3] < '0' || tmp[3] > '9')) {
				return false;
			}
			
			std::string account_number = tmp.substr(5, tmp.length() - 6);
			std::cout << account_number << std::endl;

			for (char c : account_number) {
				if (c < '0' || c > '9') {
					return false;
				}
			}

			return true;
		}

		static bool IsValidSteam32(const std::string& steam_id32) {
			std::string tmp(steam_id32);
			std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);

			if (tmp[0] != 'S' || tmp[1] != 'T' ||
				tmp[2] != 'E' || tmp[3] != 'A' ||
				tmp[4] != 'M' || tmp[5] != '_' ||
				tmp[7] != ':' || tmp[9] != ':') {
				return false; 
			}

			if ((tmp[6] < '0' || tmp[6] > '9') ||
				(tmp[8] < '0' || tmp[8] > '9')) {
				return false;
			}

			std::string account_number = tmp.substr(10);

			for (char c: account_number) {
				if (c < '0' || c > '9') {
					return false;
				}
			}

			return true;
		}

		static uint64 SteamID32ToSteamID64(const std::string& steam_id32) {
			if (IsValidSteam32(steam_id32)) {
				uint64 account_number = std::stoll(steam_id32.substr(10));
				return (account_number * 2) + 76561197960265728;
			}

			return 0;
		}

		static std::string SteamID64ToSteam3(uint64 steam_id64) {
			if (steam_id64 == 0) {
				return std::string("[U:1:0]");
			}

			uint64 account_number = (steam_id64 - (76561197960265728 + (steam_id64 % 2))) / 2;
			int id_number = (steam_id64 % 2 != 0) ? 1 : 0;
			return std::string("[U:1:" + std::to_string((account_number * 2) + id_number) + "]");
		}
		
		static std::string SteamID64ToSteamID32(uint64 steam_id64) {
			if (steam_id64 == 0) {
				return std::string("STEAM_1:0:0");
			}

			uint64 account_number = (steam_id64 - (76561197960265728 + (steam_id64 % 2))) / 2;
			int id_number = (steam_id64 % 2 != 0) ? 1 : 0;
			// NOTE(Pedro) When we parse the demo CSGO give us STEAM_1 as SteamID32	(Universe Public)
			// Apparently STEAM_0 was a bug/problem with some Source games
			// http://store.steampowered.com/news/2252
			return std::string("STEAM_1:" + std::to_string(id_number) + ":" + std::to_string(account_number));
		}

	private:
		uint64 steam_id64_{};
	};

}

#endif
