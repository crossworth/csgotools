#pragma once

#include "Steam.hpp"

namespace csgotools {
    
    struct GameEventKey {
        int32 type{0};
        std::string string_val;
        float float_val;
        int32 long_val;
        int32 short_val;
        int32 byte_val;
        bool bool_val;
        uint64 uint64_val;
        std::string wstring_val;

        explicit operator std::string() const {
            std::string out;

            out += "Type: " + std::to_string(type) + "\n";

            switch (type) {
                case 1:
                out += "string: " + string_val + "\n";
                break;
                case 2:
                out += "float: " + std::to_string(float_val) + "\n";
                break;
                case 3:
                out += "long: " + std::to_string(long_val) + "\n";
                break;
                case 4:
                out += "short: " + std::to_string(short_val) + "\n";
                break;
                case 5:
                out += "byte: " + std::to_string(byte_val) + "\n";
                break;
                case 6:
                out += "bool: " + std::to_string(bool_val) + "\n";
                break;
                case 7:
                out += "uint64: " + std::to_string(uint64_val) + "\n";
                break;
                case 8:
                out += "wsrting: " + wstring_val + "\n";
                break;
                default:
                out += "Type not found \n";
            }

            return out;
        }
    };


}
