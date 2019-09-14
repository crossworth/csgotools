// CSGOTools/csgotools - 2016
// Player.hpp
// Pedro Henrique <system.pedrohenrique@gmail.com>
// 


#pragma once

#include  "Steam.hpp"
#include "Vector.hpp"
#include "DemoMemoryBitStream.hpp"

namespace csgotools {
    class Player {
        static constexpr uint32 kMaxPlayerNameLength = 128;
        static constexpr uint32 kSignedGUIDLength = 32;
        static constexpr uint32 kMaxCustomFiles = 4;


        SteamID steam_id_;
        int32 kills_{};
        int32 deaths_{};
        int32 assists_{};
        int32 score_{};
        int32 mvps_{};
        int32 headshots_{};

        uint64 version_{};
        std::string name_;
        uint32 user_id_{}; // local user id
        bool is_fake_player_{ false };
        bool is_hltv_proxy_{ false };
        uint32 custom_files_[Player::kMaxCustomFiles]{};
        uint8 files_downloaded_{};

        Vector<float>position_{};
        Vector<float>eye_angle_{};
        bool is_connected_{};
        bool is_observer_{};
        bool has_kit_{};
        bool has_helmet_{};
        uint32 health_{};
        uint32 armor_{};
    public:


        Player() {}

        explicit Player(const SteamID& steam_id)
            : steam_id_{steam_id} {}

        SteamID& SteamID() { return steam_id_; }
        void SteamID(const csgotools::SteamID& steam_id) { steam_id_ = steam_id; }
        int32& Kills() { return kills_; }
        void Kills(int32 kills) { kills_ = kills; }
        int32& Deaths() { return deaths_; }
        void Deaths(int32 deaths) { deaths_ = deaths; }
        int32& Assists() { return assists_; }
        void Assists(int32 assists) { assists_ = assists; }
        int32& Score() { return score_; }
        void Score(int32 score) { score_ = score; }
        int32& MVPs() { return mvps_; }
        void MVPs(int32 mvps) { mvps_ = mvps; }
        int32& Headshots() { return headshots_; }
        void Headshots(int32 headshots) { headshots_ = headshots; }
        float HeadshotsPorcentage() const { return (static_cast<float>(headshots_) / kills_) * 100.f; }

        uint64& Version() { return version_; }
        void Version(uint64 version) { version_ = version; }
        std::string& Name() { return name_; }
        void Name(const std::string& name) { name_ = name; }
        uint32& UserId() { return user_id_; }
        void UserId(uint32 user_id) { user_id_ = user_id; }
        bool& IsFakePlayer() { return is_fake_player_; }
        void IsFakePlayer(bool is_fake_player) { is_fake_player_ = is_fake_player; }
        bool& IsHltvProxy() { return is_hltv_proxy_; }
        void IsHltvProxy(bool is_hltv_proxy) { is_hltv_proxy_ = is_hltv_proxy; }
        uint8& FilesDownloaded() { return files_downloaded_; }
        void FilesDownloaded(uint8 files_downloaded) { files_downloaded_ = files_downloaded; }
        Vector<float>& Position() { return position_; }
        void Position(const Vector<float>& position) { position_ = position; }
        Vector<float>& EyeAngle() { return eye_angle_; }
        void EyeAngle(const Vector<float>& eye_angle) { eye_angle_ = eye_angle; }
        bool& IsConnected() { return is_connected_; }
        void IsConnected(bool is_connected) { is_connected_ = is_connected; }
        bool& IsObserver() { return is_observer_; }
        void IsObserver(bool is_observer) { is_observer_ = is_observer; }
        bool& HasKit() { return has_kit_; }
        void HasKit(bool has_kit) { has_kit_ = has_kit; }
        bool& HasHelmet() { return has_helmet_; }
        void HasHelmet(bool has_helmet) { has_helmet_ = has_helmet; }
        uint32& Health() { return health_; }
        void Health(uint32 health) { health_ = health; }
        uint32& Armor() { return armor_; }
        void Armor(uint32 armor) { armor_ = armor; }

        static Player CreateFromDemoUserInfo(DemoMemoryBitStream& memory);
        static Player CreateFromMatchInfo();

        explicit operator std::string() const;

    };

    std::ostream& operator<<(std::ostream& out, const Player& player);

}
