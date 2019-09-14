#ifndef CSGOTOOLS_QANGLE_HPP_
#define CSGOTOOLS_QANGLE_HPP_

#include <iostream>

namespace csgotools {

	struct QAngle {
		float x{ 0 };
		float y{ 0 };
		float z{ 0 };

		QAngle() = default;

		QAngle(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

        operator std::string() const {
            return "QAngle(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
		}

		friend std::ostream& operator<<(std::ostream& out, const QAngle& angle) {
            out << static_cast<std::string>(angle);
			return out;
		}
	};

}

#endif