#pragma once

#include "MemoryBitStream.hpp"
#include <unordered_map>
#include <sstream>

namespace csgotools {

    class ConVar {
        std::string name_;
        std::string value_;
    public:

        ConVar() {}

        ConVar(const std::string& name, const std::string& value)
            : name_(name),
              value_(value) {}

        std::string Name() const { return name_; }
        std::string Value() const { return value_; }

        explicit operator std::string() const;

        static std::unordered_map<std::string, ConVar> CreateFromMemory(MemoryBitStream& memory);
        static std::unordered_map<std::string, ConVar> CreateFromMemory(MemoryBitStream&& memory);

        template<typename T>
        T As() {
            T result;

            std::stringstream value(value_);

            if (!(value >> result)) {
                std::cerr << "ConVar::As: Could not convert to the type specified" << std::endl;
            }

            return result;
        }
    };

    std::ostream& operator<<(std::ostream& out, ConVar& con_var);

}
