#include "NetTick.hpp"
#include "netmessages_public.pb.h"

using namespace csgotools;

NetTick NetTick::CreateFromMemory(MemoryBitStream& memory) {
    return NetTick::CreateFromMemory(std::move(memory));
}

NetTick NetTick::CreateFromMemory(MemoryBitStream&& memory) {
    CNETMsg_Tick net_tick;
    net_tick.ParseFromArray(memory.GetBufferPointer(), memory.GetSize());
    return NetTick(net_tick.tick(),
                   net_tick.host_computationtime(),
                   net_tick.host_computationtime_std_deviation(),
                   net_tick.host_framestarttime_std_deviation());
}

NetTick::operator std::string() const {
    std::stringstream out;
    out << "------------NET TICK------------" << std::endl;
    out << "Tick: " << tick_ << std::endl;
    out << "Host Computation Time: " << host_computation_time_ << std::endl;
    out << "Host Computation Time STD Deviation: " << host_computation_time_std_deviation_ << std::endl;
    out << "Host Frame Start Time STD Deviation: " << host_frame_start_time_std_deviation_ << std::endl;
    out << "------------NET TICK------------" << std::endl;
    return out.str();
}

std::ostream& csgotools::operator<<(std::ostream& out, const NetTick& net_tick) {
    out << static_cast<std::string>(net_tick);
    return out;
}
