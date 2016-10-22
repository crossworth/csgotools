#include "DemoHeader.hpp"

using namespace csgotools;

const std::string DemoHeader::kDemoHeaderID = "HL2DEMO";

const float DemoHeader::GetTickRate() const {
	if (file_stamp_.empty()) {
		return 0.0f;
	}

	return static_cast<float>(this->playback_frames_ / this->playblack_time_);
}

const float DemoHeader::GetTickTime() const {
	if (file_stamp_.empty()) {
		return 0.0f;
	}

	return  static_cast<float>(this->playblack_time_ / this->playback_frames_);
}

DemoHeader DemoHeader::CreateFromMemoryBitStream(MemoryBitStream& memory) {
	DemoHeader demo;
	// NOTE(Pedro): We have to read the NULL terminator on the end of the file stamp
	demo.file_stamp_ = memory.ReadFixedString(DemoHeader::kDemoHeaderID.length() + 1);
	demo.protocol_ = memory.ReadInt32();
	demo.network_protocol_ = memory.ReadInt32();
	demo.server_name_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
	demo.client_name_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
	demo.map_name_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
	demo.game_dir_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
	demo.playblack_time_ = memory.ReadFloat();
	demo.playback_ticks_ = memory.ReadInt32();
	demo.playback_frames_ = memory.ReadInt32();
	demo.sign_on_length_ = memory.ReadInt32();

	if (demo.file_stamp_ != DemoHeader::kDemoHeaderID) {
		std::cerr << "DemoHeader::CreateFromMemoryBitStream: FileStamp ID doesn't match the default CSGO FileStamp ID" << std::endl;
		return DemoHeader();
	}

	if (demo.protocol_ != DemoHeader::kDemoProtocol) {
		std::cerr << "DemoHeader::CreateFromMemoryBitStream: File protocol doesn't match the default CSGO demo protocol" << std::endl;
		return DemoHeader();
	}

	return demo;
}

std::ostream& csgotools::operator<<(std::ostream& out, const DemoHeader& header) {
	out << "------------DemoHeader------------" << std::endl;
	out << "FileStamp: " << header.GetFileStamp() << std::endl;
	out << "Protocol: " << header.GetProtocol() << std::endl;
	out << "Network Protocol: " << header.GetNetworkProtocol() << std::endl;
	out << "Server Name: " << header.GetServerName() << std::endl;
	out << "Client Name: " << header.GetClientName() << std::endl;
	out << "Map Name: " << header.GetMapName() << std::endl;
	out << "Game Dir: " << header.GetGameDir() << std::endl;
	out << "Playback Time: " << header.GetPlaybackTime() << std::endl;
	out << "Playback Ticks: " << header.GetPlayBackTicks() << std::endl;
	out << "Playback Frames: " << header.GetPlaybackFrames() << std::endl;
	out << "Sign on Length: " << header.GetSignOnLength() << std::endl;
	out << "Server Tick Rate: " << header.GetTickRate() << std::endl;
	out << "------------DemoHeader------------" << std::endl;
	return out;
}
