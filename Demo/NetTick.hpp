#ifndef CSGOTOOLS_NET_TICK_HPP_
#define CSGOTOOLS_NET_TICK_HPP_

#include <Steam.hpp>

namespace csgotools {

	struct NetTick {
		NetTick(uint32 tick, uint32 host_computation_time,
				uint32 host_computation_std_deviation,
				uint32 host_frame_start_time_std_deviation) {
			this->tick = tick;
			this->host_computation_time = host_computation_time;
			this->host_computation_std_deviation = host_computation_std_deviation;
			this->host_frame_start_time_std_deviation = host_frame_start_time_std_deviation;
		}

		// tick: Current Tick
		// host_computationtime: How much time the server take to process one tick result in microseconds(u) (4ms for 64tick)
		// host_computationtime_std_deviation: standard deviation or variation in microseconds(u)
		// host_frame_start_time_std_deviation: time to the server wake up from a sleep call after meeting the server framerate
		uint32 tick{0};
		uint32 host_computation_time{0};
		uint32 host_computation_std_deviation{0};
		uint32 host_frame_start_time_std_deviation{0};
	};
}

#endif

