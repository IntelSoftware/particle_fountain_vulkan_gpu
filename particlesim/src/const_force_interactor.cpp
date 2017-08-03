#include <psim/const_force_interactor.h>

namespace psim {
    namespace interactors {
        ConstForceInteractor::ConstForceInteractor(vec::vec3Coord direction, base::force magnitude)
            : force(direction * magnitude)
        {}

        BaseInteractor::result ConstForceInteractor::computeAcceleration(const particle & p)
        {
            return std::make_pair(force *(1/p.massKG), true);
        }

        BaseInteractor * ConstForceInteractor::clone() const
        {
            return new ConstForceInteractor(*this);
        }
    }
}
