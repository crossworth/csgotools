// CSGOTools/csgotools - 2016
// FlatSendProp.hpp
// Pedro Henrique <system.pedrohenrique@gmail.com>
// 


#pragma once

#include "SendProp.hpp"

namespace csgotools {
    struct FlatSendProp {
        const SendProp* prop;
        const SendProp* prop_array;


        FlatSendProp(const SendProp* prop, const SendProp* prop_array)
            : prop{prop},
              prop_array{prop_array} {}
    };

}
