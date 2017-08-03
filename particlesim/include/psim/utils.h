#ifndef _PSIM_UTILS_
#define _PSIM_UTILS_

#include <psim/types.h>
#include <cmath>

namespace psim {
    namespace utils {
        constexpr float DEGTORAD = 3.14f / 180.0f;

        inline vec::vec3f fromSpherical(float theta, float phi) 
        {
            vec::vec3f ret;

            ret[axis::X] = std::sinf(phi) * std::cosf(theta);
            ret[axis::Y] = std::sinf(phi) * std::sinf(theta);
            ret[axis::Z] = std::cosf(phi);

            return ret;
        }
    }
}

#endif
