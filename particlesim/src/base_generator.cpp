#include <psim/base_generator.h>
#include <chrono>

namespace psim {
	namespace generators {
		BaseGenerator::ttlpair BaseGenerator::defTTL(uint32_t seconds, float deviation)
		{
			return std::make_pair(seconds, deviation);
		}

		BaseGenerator::floatpair BaseGenerator::defMass(float kg, float deviation)
		{
			return std::make_pair(kg, deviation);
		}

		BaseGenerator::floatpair BaseGenerator::defSpeed(float s, float deviation)
		{
			return std::make_pair(s, deviation);
		}

		BaseGenerator::floatpair BaseGenerator::defSpawningRate(float particles, float seconds)
		{
			return std::make_pair(particles, seconds);
		}

		BaseGenerator::BaseGenerator(ttlpair ttl, floatpair mass, floatpair speed, floatpair rate)
		{
			baseProperties.defTTL = ttl;
			if (ttl.second > 0) {
				baseProperties.distTTL = std::uniform_real_distribution<base::time>(ttl.first - ttl.second, ttl.first + ttl.second);
				baseProperties.variableTTL = true;
			} else {
				baseProperties.variableTTL = false;
			}

			baseProperties.defMass = mass;
			if (mass.second > 0) {
				baseProperties.distMass = std::uniform_real_distribution<float>(mass.first - mass.second, mass.first + mass.second);
				baseProperties.variableMass = true;
			} else {
				baseProperties.variableMass = false;
			}

			baseProperties.defSpeed = speed;
			if (speed.second > 0) {
				baseProperties.distSpeed = std::uniform_real_distribution<float>(speed.first - speed.second, speed.first + speed.second);
				baseProperties.variableSpeed = true;
			} else {
				baseProperties.variableSpeed = false;
			}

			baseProperties.defParticleSpawnRate = rate;
			baseProperties.particleSpawnRate = rate.first / rate.second;

			baseProperties.worldSizeSet = false;
		}

		void BaseGenerator::setWorldSize(const vec::vec3f & size)
		{
			baseProperties.worldSize = size;
			baseProperties.worldSizeSet = false;
		}

		std::ranlux24_base & BaseGenerator::randEngine()
		{
			static std::ranlux24_base generator;
			static bool initialized = false;

			if (!initialized) {
				generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
				initialized = true;
			}

			return generator;
		}

		base::time BaseGenerator::genTTL() const
		{
			base::time ret;

			if (baseProperties.variableTTL) {
				ret = baseProperties.distTTL(BaseGenerator::randEngine());
			} else {
				ret = baseProperties.defTTL.first;
			}

			return ret;
		}

		float BaseGenerator::genMass() const
		{
			float ret;

			if (baseProperties.variableMass) {
				ret = baseProperties.distMass(BaseGenerator::randEngine());
			} else {
				ret = baseProperties.defMass.first;
			}

			return ret;
		}

		float BaseGenerator::genSpeed() const
		{
			float ret;

			if (baseProperties.variableSpeed) {
				ret = baseProperties.distSpeed(BaseGenerator::randEngine());
			} else {
				ret = baseProperties.defSpeed.first;
			}

			return ret;
		}

		void BaseGenerator::toNormalizedScale(particle & p)
			{
				p.pos[axis::X] = p.pos[axis::X] / baseProperties.worldSize[axis::X];
				p.pos[axis::Y] = p.pos[axis::Y] / baseProperties.worldSize[axis::Y];
				p.pos[axis::Z] = p.pos[axis::Z] / baseProperties.worldSize[axis::Z];
			}
	}
}