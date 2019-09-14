#ifndef CSGOTOOLS_DEMO_HEADER_HPP_
#define CSGOTOOLS_DEMO_HEADER_HPP_

#include <MemoryBitStream.hpp>

namespace csgotools {

	class DemoHeader {
	public:
		DemoHeader() = default;

		static DemoHeader CreateFromMemoryBitStream(MemoryBitStream& memory);

		const float GetTickRate() const;

		const float GetTickTime() const;

		std::string GetFileStamp() const { return file_stamp_; }
		int32 GetProtocol() const { return protocol_; }
		int32 GetNetworkProtocol() const { return network_protocol_; }
		std::string GetServerName() const { return server_name_; }
		std::string GetClientName() const { return client_name_; }
		std::string GetMapName() const { return map_name_; }
		std::string GetGameDir() const { return game_dir_; }
		float GetPlaybackTime() const { return playblack_time_; }
		int32 GetPlayBackTicks() const { return playback_ticks_; }
		int32 GetPlaybackFrames() const { return playback_frames_; }
		int32 GetSignOnLength() const { return sign_on_length_; }

		static const std::string kDemoHeaderID;
		static const int32 kDemoProtocol = 4;
		static const int32 kMaxOSPath = 260;

	private:
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
	};

	std::ostream& operator<<(std::ostream& out, const DemoHeader& header);
	
}

#endif;

