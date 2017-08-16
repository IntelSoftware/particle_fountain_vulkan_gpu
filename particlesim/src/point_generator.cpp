#include <psim/point_generator.h>
#include <base/utils.h>

#include <iostream>

namespace psim {
    namespace generators {
        BaseGenerator::floatpair PointGenerator::defTheta(float theta, float deviation)
        {
            return std::make_pair(theta, deviation);
        }

        BaseGenerator::floatpair PointGenerator::defPhi(float phi, float deviation)
        {
            return std::make_pair(phi, deviation);
        }

        PointGenerator::PointGenerator(base::vec::vec3f pos, floatpair theta, floatpair phi, ttlpair ttl, floatpair mass, floatpair speed, floatpair rate)
            : BaseGenerator(ttl, mass, speed, rate)
            , accumulatedParticleCount(0)
        {
            pointGenProperties.position = pos;

            theta = std::make_pair(theta.first * base::utils::DEGTORAD, theta.second * base::utils::DEGTORAD);
            pointGenProperties.defTheta = theta;
            if (theta.second > 0) {
                pointGenProperties.distTheta = std::uniform_real_distribution<float>(theta.first - theta.second, theta.first + theta.second);
                pointGenProperties.variableTheta = true;
            } else {
                pointGenProperties.variableTheta = false;
            }

            phi = std::make_pair(phi.first * base::utils::DEGTORAD, phi.second * base::utils::DEGTORAD);
            pointGenProperties.defPhi = phi;
            if (phi.second > 0) {
                pointGenProperties.distPhi = std::uniform_real_distribution<float>(phi.first - phi.second, phi.first + phi.second);
                pointGenProperties.variablePhi = true;
            } else {
                pointGenProperties.variablePhi = false;
            }
        }

        PointGenerator::PointGenerator(const PointGenerator & o)
            : BaseGenerator(o)
            , pointGenProperties(o.pointGenProperties)
            , accumulatedParticleCount(o.accumulatedParticleCount)
        { }

        PointGenerator & PointGenerator::operator=(const PointGenerator & o)
        {
            if (this != &o) {
                BaseGenerator::operator=(o);
                pointGenProperties = o.pointGenProperties;
                accumulatedParticleCount = o.accumulatedParticleCount;
            }

            return *this;
        }

        void PointGenerator::generate(base::Buffer & buffer, base::scalar::time dt, uint32_t limit)
        {
            accumulatedParticleCount += baseProperties.particleSpawnRate * dt;

            if (accumulatedParticleCount > 1) {
                uint32_t count = static_cast<uint32_t>(accumulatedParticleCount);
                accumulatedParticleCount -= count;
                if (count > limit) {
                    count = limit;
                }

                bool lastAdd = true;
                while ((lastAdd == true) && (count > 0)) {
                        float phi = genPhi();
                        float theta = genTheta();
                        float speed = genSpeed();
                        base::vec::vec3f velocity = speed * base::utils::fromSpherical(theta, phi);

                        base::particle p{
                            pointGenProperties.position,
                            {1.0f, 1.0f, 1.0f},
                            velocity,
                            genTTL(),
                            genMass()
                        };
                        toNormalizedScale(p);

                        lastAdd = buffer.add(p);
                        --count;
                }
            }
        }

        BaseGenerator * PointGenerator::clone() const
        {
            return new PointGenerator(*this);
        }

        float PointGenerator::genTheta() const
        {
            float ret;

            if (pointGenProperties.variableTheta) {
                ret = pointGenProperties.distTheta(BaseGenerator::randEngine());
            } else {
                ret = pointGenProperties.defTheta.first;
            }

            return ret;
        }

        float PointGenerator::genPhi() const
        {
            float ret;

            if (pointGenProperties.variablePhi) {
                ret = pointGenProperties.distPhi(BaseGenerator::randEngine());
            } else {
                ret = pointGenProperties.defPhi.first;
            }

            return ret;
        }
    }
}
