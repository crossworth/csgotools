#include "ServerClass.hpp"
#include <sstream>

using namespace csgotools;

uint32 ServerClass::Bits = 0;

ServerClass::operator std::string() const {
    std::stringstream out;
    out << "------------SERVER CLASS------------" << std::endl;
    out << "ID: " << class_id_ << std::endl;
    out << "Name: " << name_ << std::endl;
    out << "DTName: " << dt_name_ << std::endl;
    out << "Props" << std::endl;

    for (auto& props : flattened_props_) {
        out << SendProp::GetDebugStringIdented(*props.prop, 4);

        if (props.prop_array) {
            out << SendProp::GetDebugStringIdented(*props.prop_array, 8);
        }

        out << std::endl;
    }
    out << "------------SERVER CLASS------------" << std::endl;
    return out.str();
}

std::ostream& csgotools::operator<<(std::ostream& out, const ServerClass& server_class) {
    out << static_cast<std::string>(server_class) << std::endl;
    return out;
}
