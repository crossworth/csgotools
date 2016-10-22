#include "Player.hpp"

using namespace csgotools;

Player Player::CreateFromMemoryBitStream(MemoryBitStream& memory) {
	Player player;

	player.version_ = memory.ReadInt64();
	int64 steam_id = memory.ReadInt64();
	player.steam_id_.SetSteamID64(SWAP64(steam_id));
	player.name_ = memory.ReadFixedString(Player::kMaxPlayerNameLength);
	int32 user_id = memory.ReadInt32();
	player.user_id_ = SWAP32(user_id);

	// NOTE(Pedro) We dont use the guid, since we can get it from the SteamID
	memory.ReadFixedString(Player::kSignedGUIDLength + 1);

	memory.ReadInt32(); // Same with friendsID, we can get it from the SteamID if we need.

	// Read the friendsName, I never had found a demo where there is data on this field
	// I guess it was supposed to contain the Steam3 or the custom URL profile
	memory.ReadFixedString(Player::kMaxPlayerNameLength);

	player.is_fake_player_ = memory.ReadByte() != 0; // Remove the warning C4800
	player.is_hltv_proxy_ = memory.ReadByte() != 0;

	// Read the custom files
	for (uint32 i = 0; i < Player::kMaxCustomFiles; i++) {
		player.custom_files_[i] = memory.ReadInt32();
	}
	
	// Files downloaded counter <- It increses each time the server download a new file
	player.files_downloaded_ = memory.ReadByte();

	return player;
}


std::ostream& csgotools::operator<<(std::ostream& out, const Player& player) {
	out << "------------Player------------" << std::endl;
	out << "Name: " << player.GetName() << std::endl;
	out << "SteamID: " << player.GetSteamID().GetSteamID64() << " " << player.GetSteamID().GetSteamID32() << std::endl;
	out << "UserID: " << player.GetUserID() << std::endl;
	out << "Is Fake Player: " << std::boolalpha << player.IsFakePlayer() << std::endl;
	out << "IS HLTV Proxy: " << std::boolalpha << player.IsHLTVProxy() << std::endl;
	out << "CustomFiles: ";
	for (uint32 i = 0; i < Player::kMaxCustomFiles; i++) {
		out << player.custom_files_[i];

		if (i < (Player::kMaxCustomFiles - 1)) {
			out << ", ";
		}
	}
	out << std::endl;
	out << "FilesDownloaded: " << player.files_downloaded_ << std::endl;

	// TODO(Pedro): Add all the shit

	out << "------------Player------------" << std::endl;
	return out;
}
