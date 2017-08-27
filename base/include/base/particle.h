#ifndef _PSIM_PARTICLE_
#define _PSIM_PARTICLE_

#include <cstddef>
#include <base/types.h>

namespace base {
    struct particle
    {

        alignas(16) vec::vec3f pos;
        alignas(16) scalar::colorRGB color;
        alignas(16) vec::vec3f velocity;
        scalar::time ttl;
        float massKG;
        float _pad;

        static std::size_t size();

        float distanceTo(const vec::vec3f &p) const noexcept;
        float distanceToSq(const vec::vec3f &p) const noexcept;
        vec::vec3f directionTo(const vec::vec3f &p) const noexcept;
    };
}

#include <base/particle_inline.h>

#endif
