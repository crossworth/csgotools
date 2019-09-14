#include "ServerClass.hpp"
#include <sstream>

using namespace csgotools;

uint32 ServerClass::Bits = 0;

ServerClass::operator std::string() const {
    std::stringstream output;
    output << "ID: " << class_id_ << std::endl;
    output << "Name: " << name_ << std::endl;
    output << "DTName: " << dt_name_ << std::endl;
    output << "Props" << std::endl;

    for (auto& props : flattened_props_) {
        output << SendProp::GetDebugStringIdented(*props.prop, 4);

        if (props.prop_array) {
            output << SendProp::GetDebugStringIdented(*props.prop_array, 8);
        }

        output << std::endl;
    }
    return output.str();
}

std::ostream& csgotools::operator<<(std::ostream& out, const ServerClass& server_class) {
    out << static_cast<std::string>(server_class) << std::endl;
    return out;
}
