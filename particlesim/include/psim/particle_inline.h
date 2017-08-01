#ifndef _PSIM_PARTICLE_INLINE_
#define _PSIM_PARTICLE_INLINE_

namespace psim{

	inline base::coord particle::distanceTo(const vec::vec3Coord &p) const noexcept
	{
		return std::sqrt((pos[0]-p[0])*(pos[0]-p[0]) 
			+ (pos[1]-p[1])*(pos[1]-p[1]) 
			+ (pos[2]-p[2])*(pos[2]-p[2]));
	}

	inline base::coord particle::distanceToSq(const vec::vec3Coord &p) const noexcept
	{
		return (pos[0]-p[0])*(pos[0]-p[0]) 
			+ (pos[1]-p[1])*(pos[1]-p[1]) 
			+ (pos[2]-p[2])*(pos[2]-p[2]);
	}

	inline vec::vec3Coord particle::directionTo(const vec::vec3Coord &p) const noexcept
	{
		return (p - pos) * (1 / distanceTo(p));
	}
}

#endif