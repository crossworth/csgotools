#include "ConVar.hpp"
#include "netmessages_public.pb.h"
#include <unordered_map>

using namespace csgotools;

ConVar::operator std::string() const {
    std::string out;
    out += "------------CON VAR------------\n";
    out += "Name: " + name_ + "\n";
    out += "Value: " + value_ + "\n";
    out += "------------CON VAR------------\n";
    return out;
}

std::unordered_map<std::string, ConVar> ConVar::CreateFromMemory(MemoryBitStream& memory) {
    return CreateFromMemory(std::move(memory));
}

std::unordered_map<std::string, ConVar> ConVar::CreateFromMemory(MemoryBitStream&& memory) {
    CNETMsg_SetConVar set_con_var;
    set_con_var.ParseFromArray(memory.GetBufferPointer(), memory.GetSize());

    uint32 num_con_vars = set_con_var.convars().cvars_size();

    std::unordered_map<std::string, ConVar> result;
    result.reserve(num_con_vars);

    for (uint32 i = 0; i < num_con_vars; ++i) {
        const CMsg_CVars_CVar& con_var = set_con_var.convars().cvars(i);
        result[con_var.name()] = ConVar(con_var.name(), con_var.value());
    }

    return result;
}

std::ostream& csgotools::operator<<(std::ostream& out, ConVar& con_var) {
    out << static_cast<std::string>(con_var);
    return out;
}
