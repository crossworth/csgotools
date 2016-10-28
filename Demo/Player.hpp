#ifndef CSGOTOOLS_PLAYER_HPP_
#define CSGOTOOLS_PLAYER_HPP_

#include <Steam.hpp>
#include <Vector.hpp>
#include <MemoryBitStream.hpp>

namespace csgotools {

	class Player {
		static constexpr uint32 kMaxPlayerNameLength = 128;
		static constexpr uint32 kSignedGUIDLength = 32;
		static constexpr uint32 kMaxCustomFiles = 4;
	public:
		Player() = default;

		static Player CreateFromMemoryBitStream(MemoryBitStream& memory);
		
		std::string GetName() const { return name_; }
		SteamID GetSteamID() const { return steam_id_; }
		uint32 GetUserID() const { return user_id_; }
		bool IsFakePlayer() const { return is_fake_player_; }
		bool IsHLTVProxy() const { return is_hltv_proxy_; }
		Vector GetPosition() const { return position_; }
		Vector GetEyeAngle() const { return eye_angle_; }
        int32 GetAssists() const { return assists_; }
        int32 GetKills() const { return kills_; }
        int32 GetDeaths() const { return deaths_; }
        int32 GetScore() const { return score_; }
        int32 GetMVPs() const { return mvps_; }
        int32 GetNumberHeadshots() const { return headshots_number_; }
        float GetHeadshotsPorcentage() const { return (static_cast<float>(headshots_number_) / kills_) * 100.f; }

		void SetName(const std::string& name) { name_ = name; }
        void SetSteamID(SteamID& steam_id) { steam_id_ = steam_id; }
        void SetUserID(uint32 user_id) { user_id_ = user_id; }
		void SetPosition(const Vector& position) { position_ = position; }
        void SetEyeAngle(const Vector& eye_angle) { eye_angle_ = eye_angle; }
        void SetKills(int32 kills) { kills_ = kills; }        
        void SetAssists(int32 assists) { assists_ = assists; }
        void SetDeaths(int32 deaths) { deaths_ = deaths; }
        void SetScore(int32 score) { score_ = score; }        
        void SetMVPs(int32 mvps) { mvps_ = mvps; }
        void SetNumberHeadshots(int32 headshots) { headshots_number_ = headshots; }

	
	private:
		uint64 version_{}; // for future compatibility
		SteamID steam_id_{};
		std::string name_;
		uint32 user_id_{}; // local user id
		bool is_fake_player_{false};
		bool is_hltv_proxy_{false};
		uint32 custom_files_[Player::kMaxCustomFiles]{};
		uint8 files_downloaded_{};

		// TODO(Pedro) check what we use on this and what not
		// Add to the operator<< overloading function
		Vector position_{};
		Vector eye_angle_{};
		bool is_connected_{};
		bool is_observer_{};
		bool has_kit_{};
		bool has_helmet_{};
		uint32 health_{};
		uint32 armor_{};
		int32 kills_{};
		int32 deaths_{};
		int32 assists_{};
		int32 score_{};
		int32 mvps_{};
        int32 headshots_number_{};
		
		friend std::ostream& operator<<(std::ostream& out, const Player& player);
	};

}

#endif

