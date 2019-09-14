#include "CommandInfo.hpp"

using namespace csgotools;

CommandInfo CommandInfo::CreateFromMemory(MemoryBitStream& memory) {
	CommandInfo command;
	memory.ReadBytes(static_cast<void*>(&command), sizeof(CommandInfo));
	return command;
}


