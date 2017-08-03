#ifndef _PSIM_PLANAR_GRAVITY_INTERACTOR_
#define _PSIM_PLANAR_GRAVITY_INTERACTOR_

#include <psim/abstract_gravity_interactor.h>

namespace psim {
    namespace interactors {
        class PlanarGravityInteractor : public AbstractGravityInteractor
        {
        public:
            PlanarGravityInteractor(vec::vec3Coord position, vec::vec3Coord normal, float massKG, float gConst = G, bool deactivateOnContact = true);
            virtual ~PlanarGravityInteractor() = default;
            PlanarGravityInteractor(const PlanarGravityInteractor& o);
            PlanarGravityInteractor& operator=(const PlanarGravityInteractor& o);

            virtual BaseInteractor::result computeAcceleration(const particle & p) override; //force in Newtons
            virtual BaseInteractor* clone() const override;

        protected:
            vec::vec3Coord pos;
            vec::vec3Coord n;
            float mass;
            float d; //used in distance calculation
            bool deactivate;

            float gravityCoeff;

        };
    }
}
#endif // !_PSIM_PLANAR_GRAVITY_INTERACTOR_
