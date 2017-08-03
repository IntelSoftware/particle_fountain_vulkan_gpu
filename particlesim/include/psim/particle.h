#ifndef _PSIM_PARTICLE_
#define _PSIM_PARTICLE_

#include <cstddef>
#include <psim/types.h>

namespace psim {
    struct particle
    {
        vec::vec3Coord pos;
        base::colorRGB color;
        base::time ttl;
        vec::vec3Coord velocity;
        float massKG;

        static std::size_t size();
        static std::size_t visualSize();
        static std::size_t modelSize();
        static std::size_t coordOffset();
        static std::size_t colorOffset();

        base::coord distanceTo(const vec::vec3Coord &p) const noexcept;
        base::coord distanceToSq(const vec::vec3Coord &p) const noexcept;
        vec::vec3Coord directionTo(const vec::vec3Coord &p) const noexcept;
    };
}

#include <psim/particle_inline.h>

#endif
