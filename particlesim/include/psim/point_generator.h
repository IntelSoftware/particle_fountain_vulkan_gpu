#ifndef _PSIM_POINT_GENERATOR_
#define _PSIM_POINT_GENERATOR_

#include <psim/base_generator.h>

namespace psim {
    namespace generators {
        class PointGenerator : public BaseGenerator
        {
        public:
            static floatpair defTheta(float theta, float deviation); //degrees
            static floatpair defPhi(float phi, float deviation); //degrees

            PointGenerator(base::vec::vec3f pos, 
                           floatpair theta, 
                           floatpair phi, 
                           ttlpair ttl, 
                           floatpair mass, 
                           floatpair speed, 
                           floatpair rate);

            PointGenerator(const PointGenerator& o);
            PointGenerator& operator=(const PointGenerator& o);
            virtual ~PointGenerator() = default;

            virtual void generate(base::Buffer& buffer, base::scalar::time dt, uint32_t limit = UINT32_MAX) override;
            virtual BaseGenerator* clone() const override;

            protected:
                struct {
                    floatpair defTheta;
                    bool variableTheta;
                    std::uniform_real_distribution<float> distTheta;

                    floatpair defPhi;
                    bool variablePhi;
                    std::uniform_real_distribution<float> distPhi;

                    base::vec::vec3f position;
                } pointGenProperties;

                float genTheta() const;
                float genPhi() const;
            private:
                float accumulatedParticleCount;
        };
    }
}


#endif
