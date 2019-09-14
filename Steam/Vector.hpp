#ifndef CSGOTOOLS_VECTOR_HPP_
#define CSGOTOOLS_VECTOR_HPP_

#include <iostream>

namespace csgotools {

	struct Vector {
		Vector() = default;

		Vector(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

		friend std::ostream& operator<<(std::ostream& out, const Vector& vect) {
			out << "Vector(" << vect.x << ", " << vect.y << ", " << vect.z << ")" << std::endl;
			return out;
		}
	
		float x{};
		float y{};
		float z{};
	};

}

#endif