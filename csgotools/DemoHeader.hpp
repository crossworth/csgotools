#pragma once

#include "ErrorHandling.hpp"
#include "Steam.hpp"
#include "MemoryBitStream.hpp"

namespace csgotools {

    class DemoHeader {
        static const std::string kDemoHeaderID; // HL2DEMO
        static const uint32 kDemoProtocol{4};
        static const uint32 kMaxOSPath{260};

        std::string file_stamp_;
        int32 protocol_{};
        int32 network_protocol_{};
        std::string server_name_;
        std::string client_name_;
        std::string map_name_;
        std::string game_dir_;
        float playblack_time_{};
        int32 playback_ticks_{};
        int32 playback_frames_{};
        int32 sign_on_length_{};
    public:
        std::string FileStamp() const { return file_stamp_; }
        void FileStamp(const std::string& file_stamp) { file_stamp_ = file_stamp; }
        int32 Protocol() const { return protocol_; }
        void Protocol(int32 protocol) { protocol_ = protocol; }
        int32 NetworkProtocol() const { return network_protocol_; }
        void NetworkProtocol(int32 network_protocol) { network_protocol_ = network_protocol; }
        std::string ServerName() const { return server_name_; }
        void ServerName(const std::string& server_name) { server_name_ = server_name; }
        std::string ClientName() const { return client_name_; }
        void ClientName(const std::string& client_name) { client_name_ = client_name; }
        std::string MapName() const { return map_name_; }
        void MapName(const std::string& map_name) { map_name_ = map_name; }
        std::string GameDir() const { return game_dir_; }
        void GameDir(const std::string& game_dir) { game_dir_ = game_dir; }
        float PlayblackTime() const { return playblack_time_; }
        void PlayblackTime(float playblack_time) { playblack_time_ = playblack_time; }
        int32 PlaybackTicks() const { return playback_ticks_; }
        void PlaybackTicks(int32 playback_ticks) { playback_ticks_ = playback_ticks; }
        int32 PlaybackFrames() const { return playback_frames_; }
        void PlaybackFrames(int32 playback_frames) { playback_frames_ = playback_frames; }
        int32 SignOnLength() const { return sign_on_length_; }
        void SignOnLength(int32 sign_on_length) { sign_on_length_ = sign_on_length; }
        float TickRate() const;
        float TickTime() const;
        bool IsValid() const;

        explicit operator std::string() const;
        void Clear();
        static DemoHeader CreateFromMemoryBitStream(MemoryBitStream& memory);
    };

    std::ostream& operator<<(std::ostream& out, const DemoHeader& header);
}
