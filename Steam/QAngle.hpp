#ifndef CSGOTOOLS_QANGLE_HPP_
#define CSGOTOOLS_QANGLE_HPP_

#include <iostream>

namespace csgotools {

	struct QAngle {
		float x{};
		float y{};
		float z{};

		QAngle() = default;

		QAngle(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

		friend std::ostream& operator<<(std::ostream& out, const QAngle& angle) {
			out << "QAngle(" << angle.x << ", " << angle.y << ", " << angle.z << ")" << std::endl;
			return out;
		}
	};

}

#endif