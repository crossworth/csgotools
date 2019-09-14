#pragma once


#include <sstream>

#include "Steam.hpp"

namespace csgotools {

    enum class SendPropType {
        INT,
        FLOAT,
        VECTOR,
        VECTOR_XY,
        STRING,
        ARRAY,
        DATA_TABLE,
        INT64,
        NUM_SEND_PROPS_TYPE
    };

    class SendProp {
    public:
        static constexpr uint32 kSPropExclude = (1 << 6);
        static constexpr uint32 kSPropInsideArray = (1 << 8);
        static constexpr uint32 kSPropCollapsible = (1 << 11);
        static constexpr uint32 kSPropChangesOften = (1 << 18);


        SendPropType Type() const { return type_; }

        void Type(SendPropType type) { type_ = type; }

        std::string VarName() const { return var_name_; }
        void VarName(const std::string& var_name) { var_name_ = var_name; }
        int32 Flags() const { return flags_; }
        void Flags(int32 flags) { flags_ = flags; }
        int32 Priority() const { return priority_; }
        void Priority(int32 priority) { priority_ = priority; }
        std::string DTName() const { return dt_name_; }
        void DTName(const std::string& dt_name) { dt_name_ = dt_name; }
        int32 NumElements() const { return num_elements_; }
        void NumElements(int32 num_elements) { num_elements_ = num_elements; }
        float LowValue() const { return low_value_; }
        void LowValue(float low_value) { low_value_ = low_value; }
        float HighValue() const { return high_value_; }
        void HighValue(float high_value) { this->high_value_ = high_value; }
        int32 NumBits() const { return num_bits_; }
        void NumBits(int32 num_bits) { this->num_bits_ = num_bits; }

        SendProp(SendPropType type, const std::string& var_name, int32 flags, int32 priority, const std::string& dt_name, int32 num_elements, float low_value, float high_value, int32 num_bits)
            : type_{ type },
            var_name_{ var_name },
            flags_{ flags },
            priority_{ priority },
            dt_name_{ dt_name },
            num_elements_{ num_elements },
            low_value_{ low_value },
            high_value_{ high_value },
            num_bits_{ num_bits } {}

        SendProp() {}

        operator std::string() const {
            return GetDebugStringIdented(*this, 0);
        }

        static std::string GetDebugStringIdented(const SendProp& prop, uint32 indentation = 4) {
            std::stringstream out;
            std::string padding;

            for (uint32 i = 0; i < indentation; ++i) {
                padding += " ";
            }

            out << "------------SEND PROP------------" << std::endl;
            out << padding << "VarName: " << prop.VarName() + "\n";
            out << padding << "Flags: " << std::to_string(prop.Flags()) + "\n";
            out << padding << "Priority: " << std::to_string(prop.Priority()) + "\n";
            out << padding << "DTName: " << prop.dt_name_ + "\n";
            out << padding << "NumElements: " << std::to_string(prop.NumElements()) + "\n";
            out << padding << "LowValue: " << std::to_string(prop.LowValue()) + "\n";
            out << padding << "HighValue: " << std::to_string(prop.HighValue()) + "\n";
            out << padding << "NumBits: " << std::to_string(prop.NumBits()) + "\n";
            out << "------------SEND PROP------------" << std::endl;

            return out.str();
        }

    private:
        SendPropType type_{};
        
        std::string var_name_;
        int32 flags_{};
        int32 priority_{};
        std::string dt_name_;
        int32 num_elements_{};
        float low_value_{};
        float high_value_{};
        int32 num_bits_{};
    };

    inline std::ostream& operator<<(std::ostream& out, SendProp& prop) {
        out << static_cast<std::string>(prop) << std::endl;
        return out;
    }

}
