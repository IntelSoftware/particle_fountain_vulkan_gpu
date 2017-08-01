#include <psim/particle.h>

namespace psim {
	std::size_t particle::size()
	{
		return sizeof(psim::particle);
	}

	std::size_t particle::modelSize()
	{
		return sizeof(vec::vec3Coord) + sizeof(base::colorRGB);
	}

	std::size_t particle::coordOffset()
	{
		return offsetof(particle, pos);
	}

	std::size_t particle::colorOffset()
	{
		return offsetof(particle, color);
	}

	std::size_t particle::visualSize()
	{
		return sizeof(vec::vec3Coord) + sizeof(base::colorRGB) + sizeof(float);
	}
}