#ifndef _PSIM_BASE_GENERATOR_
#define _PSIM_BASE_GENERATOR_

#include <cstdint>
#include <limits>
#include <random>
#include <base/types.h>
#include <base/buffer.h>

namespace psim {
    namespace generators {
        class BaseGenerator {
        public:
            using floatpair = std::pair<float, float>;
            using uint32pair = std::pair<uint32_t, uint32_t>;
            using ttlpair = std::pair<uint32_t, float>;

            static ttlpair defTTL(uint32_t seconds, float deviation ); //seconds+/-deviation
            static floatpair defMass(float kg, float deviation ); //kg+/-deviation
            static floatpair defSpeed(float s, float deviation ); // meters per second+/-deviation
            static floatpair defSpawningRate(float particles, float seconds); //particles per seconds
            //angles

            BaseGenerator(ttlpair ttl, floatpair mass, floatpair speed, floatpair rate );
            virtual ~BaseGenerator() = default;
            BaseGenerator(const BaseGenerator& o) = default;
            BaseGenerator& operator=(const BaseGenerator& o) = default;

            virtual void generate(base::Buffer& buffer, base::scalar::time dt, uint32_t limit = UINT32_MAX) = 0;
            virtual BaseGenerator* clone() const = 0;

            void setWorldSize(const base::vec::vec3f& size);

        protected:
            struct {
                ttlpair defTTL;
                bool variableTTL;
                std::uniform_real_distribution<base::scalar::time> distTTL;

                floatpair defMass;
                bool variableMass;
                std::uniform_real_distribution<float> distMass;

                floatpair defSpeed;
                bool variableSpeed;
                std::uniform_real_distribution<float> distSpeed;

                floatpair defParticleSpawnRate;
                float particleSpawnRate;

                base::vec::vec3f worldSize;
                bool worldSizeSet;
            } baseProperties;;

            static std::ranlux24_base& randEngine();

            base::scalar::time genTTL() const;
            float genMass() const;
            float genSpeed() const;

            void BaseGenerator::toNormalizedScale(base::particle & p);
        };
    }
}

#endif
