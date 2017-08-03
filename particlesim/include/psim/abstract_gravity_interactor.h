#ifndef _PSIM_ABSTRACT_GRAVITY_INTERACTOR_
#define _PSIM_ABSTRACT_GRAVITY_INTERACTOR_

#include <psim/base_interactor.h>

namespace psim {
    namespace interactors {
        class AbstractGravityInteractor : public BaseInteractor
        {
        public:
                static constexpr float G = 6.67408e-11f;
                static constexpr float MASS_EARTH = 5.972e+24f;
                static constexpr float MASS_SUN = 1.989e+30f;

                AbstractGravityInteractor(float gConst = G);
                virtual ~AbstractGravityInteractor() = default;

        protected:
                float gravityConstant;
        };
    }
}
#endif
