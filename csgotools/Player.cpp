#include "Player.hpp"

#include <sstream>

using namespace csgotools;

Player Player::CreateFromDemoUserInfo(DemoMemoryBitStream& memory) {
    Player player;

    player.version_ = memory.ReadInt64();
    int64 steam_id = memory.ReadInt64();
    player.steam_id_.SteamID64(SWAP64(steam_id));
    player.name_ = memory.ReadFixedString(Player::kMaxPlayerNameLength);
    int32 user_id = memory.ReadInt32();
    player.user_id_ = SWAP32(user_id);

    // NOTE(Pedro) We dont use the guid, since we can get it from the SteamID
    memory.ReadFixedString(Player::kSignedGUIDLength + 1);

    memory.ReadInt32(); // Same with friendsID, we can get it from the SteamID if we need.

    // Read the friendsName, I never had found a demo where there is data on this field
    // I guess it was supposed to contain the Steam3 or the custom URL profile
    memory.ReadFixedString(Player::kMaxPlayerNameLength);

    player.is_fake_player_ = memory.ReadByte() != 0; // Avoid the warning C4800
    player.is_hltv_proxy_ = memory.ReadByte() != 0;

    // Read the custom files
    for (uint32 i = 0; i < Player::kMaxCustomFiles; i++) {
        player.custom_files_[i] = memory.ReadInt32();
    }

    // Files downloaded counter <- It increses each time the server download a new file
    player.files_downloaded_ = memory.ReadByte();

    return player;
}

Player Player::CreateFromMatchInfo() {
    return Player{};
}

Player::operator std::string() const {
    std::stringstream out;
    out << "------------Player------------" << std::endl;
    out << "Name: " << name_ << std::endl;
    out << "SteamID: " << steam_id_.SteamID64() << " " << steam_id_.SteamID32() << std::endl;
    out << "UserID: " << user_id_ << std::endl;
    out << "Is Fake Player: " << std::boolalpha << is_fake_player_ << std::endl;
    out << "IS HLTV Proxy: " << std::boolalpha << is_hltv_proxy_ << std::endl;
    out << "CustomFiles: ";
    for (uint32 i = 0; i < Player::kMaxCustomFiles; i++) {
        out << custom_files_[i];

        if (i < (Player::kMaxCustomFiles - 1)) {
            out << ", ";
        }
    }
    out << std::endl;
    out << "FilesDownloaded: " << files_downloaded_ << std::endl;

    out << "Kills: " << kills_ << std::endl;
    out << "Deaths: " << deaths_ << std::endl;
    out << "Assists: " << assists_ << std::endl;
    out << "MVP's: " << mvps_ << std::endl;
    out << "Score: " << score_ << std::endl;
    out << "Headshots: " << headshots_ << std::endl;
    out << "Headshots %: " << HeadshotsPorcentage() << std::endl;
    out << "Healt: " << health_ << std::endl;
    out << "Armor: " << armor_ << std::endl;
    out << "Has Helmet: " << has_helmet_ << std::endl;
    out << "Has Defuse Kit: " << has_kit_ << std::endl;
    out << "Position: " << position_ << std::endl;
    out << "Eye Angle: " << eye_angle_ << std::endl;

    out << "------------Player------------" << std::endl;

    return out.str();
}

std::ostream& csgotools::operator<<(std::ostream& out, const Player& player) {
    out << static_cast<std::string>(player);
    return out;
}
