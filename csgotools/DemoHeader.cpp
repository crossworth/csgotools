#include "DemoHeader.hpp"

using namespace csgotools;


const std::string DemoHeader::kDemoHeaderID = "HL2DEMO";

float DemoHeader::TickRate() const {
    return (playblack_time_ > 0.0f) ? static_cast<float>(playback_frames_ / playblack_time_) : 0.0f;
}

float DemoHeader::TickTime() const {
    return (playback_frames_ > 0.0f) ? static_cast<float>(playblack_time_ / playback_frames_) : 0.0f;
}

bool DemoHeader::IsValid() const {
    return (file_stamp_ == DemoHeader::kDemoHeaderID && protocol_ == DemoHeader::kDemoProtocol);
}

DemoHeader::operator std::string() const {
    std::string result;
    result += "FileStamp: " + FileStamp() + "\n";
    result += "Protocol: " + std::to_string(Protocol()) + "\n";
    result += "Network Protocol: " + std::to_string(NetworkProtocol()) + "\n";
    result += "Server Name: " + ServerName() + "\n";
    result += "Client Name: " + ClientName() + "\n";
    result += "Map Name: " + MapName() + "\n";
    result += "Game Dir: " + GameDir() + "\n";
    result += "Playback Time: " + std::to_string(PlayblackTime()) + "\n";
    result += "Playback Ticks: " + std::to_string(PlaybackTicks()) + "\n";
    result += "Playback Frames: " + std::to_string(PlaybackFrames()) + "\n";
    result += "Sign on Length: " + std::to_string(SignOnLength()) + "\n";
    result += "Server Tick Rate: " + std::to_string(TickRate()) + "\n";
    return result;
}

void DemoHeader::Clear() {
    file_stamp_.clear();
    protocol_ = 0;
    network_protocol_ = 0;
    server_name_.clear();
    client_name_.clear();
    map_name_.clear();
    game_dir_.clear();
    playblack_time_ = 0.0f;
    playback_ticks_ = 0;
    playback_frames_ = 0;
    sign_on_length_ = 0;
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
    out << static_cast<std::string>(header) << std::endl;
    return out;
}
