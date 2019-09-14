#pragma once

#include "Steam.hpp"

namespace csgotools {

    struct GameEventDescriptorKey {
        int32 type{};
        std::string name;
    };

}