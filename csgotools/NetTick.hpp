#pragma once

#include <sstream>

#include "Steam.hpp"
#include "MemoryBitStream.hpp"

namespace csgotools {

    class NetTick {
        // tick: Current Tick
        // host_computationtime: How much time the server take to process one tick result in microseconds(u) (4ms for 64tick)
        // host_computationtime_std_deviation: standard deviation or variation in microseconds(u)
        // host_frame_start_time_std_deviation: time to the server wake up from a sleep call after meeting the server framerate
        uint32 tick_{0};
        uint32 host_computation_time_{0};
        uint32 host_computation_time_std_deviation_{0};
        uint32 host_frame_start_time_std_deviation_{0};
    public:

        NetTick(uint32 tick, uint32 host_computation_time, uint32 host_computation_std_deviation, uint32 host_frame_start_time_std_deviation)
            : tick_(tick),
              host_computation_time_(host_computation_time),
            host_computation_time_std_deviation_(host_computation_std_deviation),
              host_frame_start_time_std_deviation_(host_frame_start_time_std_deviation) {}

        uint32 Tick() const { return tick_; }
        uint32 HostComputationTime() const { return host_computation_time_; }
        uint32 HostComputationTimeSTDDeviation() const { return host_computation_time_std_deviation_; }
        uint32 HostFrameStartTimeSTDDeviation() const { return host_frame_start_time_std_deviation_; }

        static NetTick CreateFromMemory(MemoryBitStream& memory);
        static NetTick NetTick::CreateFromMemory(MemoryBitStream&& memory);

        explicit operator std::string() const;
    };

    std::ostream& operator<<(std::ostream& out, const NetTick& net_tick);

}
