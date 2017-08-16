#ifndef _BASE_PARTICLE_INLINE_
#define _BASE_PARTICLE_INLINE_

namespace base{
    inline float particle::distanceTo(const vec::vec3f &p) const noexcept
    {
        return std::sqrt((pos[0]-p[0])*(pos[0]-p[0]) 
                + (pos[1]-p[1])*(pos[1]-p[1]) 
                + (pos[2]-p[2])*(pos[2]-p[2]));
    }

    inline float particle::distanceToSq(const vec::vec3f &p) const noexcept
    {
        return (pos[0]-p[0])*(pos[0]-p[0]) 
                + (pos[1]-p[1])*(pos[1]-p[1]) 
                + (pos[2]-p[2])*(pos[2]-p[2]);
    }

    inline vec::vec3f particle::directionTo(const vec::vec3f &p) const noexcept
    {
        return (p - pos) * (1 / distanceTo(p));
    }
}

#endif
