// CSGOTools/csgotools - 2016
// ServerClass.hpp
// Pedro Henrique <system.pedrohenrique@gmail.com>
// 

#pragma once

#include "Steam.hpp"
#include "FlatSendProp.hpp"
#include <vector>

namespace csgotools {
    class ServerClass {
        int32 class_id_{};
        std::string name_;
        std::string dt_name_;
        std::vector<FlatSendProp> flattened_props_;

    public:
        static uint32 Bits;

        ServerClass() {}

        ServerClass(int32 class_id, const std::string& dt_name, const std::string& name)
            : class_id_{ class_id }, name_{ name }, dt_name_{ dt_name } {}

        int32 ClassID() const { return class_id_; }
        void ClassID(int32 class_id) { class_id_ = class_id; }
        std::string Name() const { return name_; }
        void Name(const std::string& name) { name_ = name; }
        std::string DTName() const { return dt_name_; }
        void DTName(const std::string& dt_name) { dt_name_ = dt_name; }
        std::vector<FlatSendProp>& FlattenedProps() { return flattened_props_; }
        void FlattenedProps(const std::vector<FlatSendProp>& flattened_props) { flattened_props_ = flattened_props; }

        explicit operator std::string() const;
    };

    std::ostream& operator<<(std::ostream& out, const ServerClass& server_class);
    
}
