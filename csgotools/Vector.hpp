#pragma once

#include <string>
#include <typeinfo>

namespace csgotools {
    
    template<typename T>
    struct Vector {
        T x{0};
        T y{0};
        T z{0};

        Vector() = default;

        Vector(T x, T y, T z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        explicit operator std::string() const {
            return {"Vector " + std::string{typeid(x).name()} + "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"};
        }

    };


    template<typename T>
    std::ostream& operator<<(std::ostream& out, const Vector<T>& vector) {
        out << static_cast<std::string>(vector);
        return out;
    }

}
