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
    result += "------------------DEMO HEADER------------------\n";
    result += "FileStamp: " + file_stamp_ + "\n";
    result += "Protocol: " + std::to_string(protocol_) + "\n";
    result += "Network Protocol: " + std::to_string(network_protocol_) + "\n";
    result += "Server Name: " + server_name_ + "\n";
    result += "Client Name: " + client_name_ + "\n";
    result += "Map Name: " + map_name_ + "\n";
    result += "Game Dir: " + game_dir_ + "\n";
    result += "Playback Time: " + std::to_string(playblack_time_) + "\n";
    result += "Playback Ticks: " + std::to_string(playback_ticks_) + "\n";
    result += "Playback Frames: " + std::to_string(playback_frames_) + "\n";
    result += "Sign on Length: " + std::to_string(sign_on_length_) + "\n";
    result += "Server Tick Rate: " + std::to_string(TickRate()) + "\n";
    result += "------------------DEMO HEADER------------------\n";
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

    demo.file_stamp_ = memory.ReadFixedString(DemoHeader::kDemoHeaderID.length() + 1);
 
    if (demo.file_stamp_ != DemoHeader::kDemoHeaderID) {
        CSGOTOOLS_ERROR("DemoHeader::CreateFromMemoryBitStream: FileStamp ID doesn't match the default CSGO FileStamp ID");
        return DemoHeader();
    }

    demo.protocol_ = memory.ReadInt32();

    if (demo.protocol_ != DemoHeader::kDemoProtocol) {
        CSGOTOOLS_ERROR("DemoHeader::CreateFromMemoryBitStream: File protocol doesn't match the default CSGO demo protocol");
        return DemoHeader();
    }

    demo.network_protocol_ = memory.ReadInt32();
    demo.server_name_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
    demo.client_name_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
    demo.map_name_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
    demo.game_dir_ = memory.ReadFixedString(DemoHeader::kMaxOSPath);
    demo.playblack_time_ = memory.ReadFloat();
    demo.playback_ticks_ = memory.ReadInt32();
    demo.playback_frames_ = memory.ReadInt32();
    demo.sign_on_length_ = memory.ReadInt32();

    return demo;
}

std::ostream& csgotools::operator<<(std::ostream& out, const DemoHeader& header) {
    out << static_cast<std::string>(header) << std::endl;
    return out;
}
