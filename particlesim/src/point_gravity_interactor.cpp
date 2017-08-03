#include <psim/point_gravity_interactor.h>

namespace psim {
    namespace interactors {
        PointGravityInteractor::PointGravityInteractor(vec::vec3Coord pos, float massKG, float gConst)
            : AbstractGravityInteractor(gConst)
            , position(pos)
            , mass(massKG)
            , gravityCoeff(gConst*massKG)
        { }

        PointGravityInteractor::PointGravityInteractor(const PointGravityInteractor & o)
            : AbstractGravityInteractor(o)
            , position(o.position)
            , mass(o.mass)
            , gravityCoeff(o.gravityCoeff)
        { }

        PointGravityInteractor & PointGravityInteractor::operator=(const PointGravityInteractor & o)
        {
            if (this != &o) {
                AbstractGravityInteractor::operator=(o);
                position = o.position;
                mass = o.mass;
                gravityCoeff = o.gravityCoeff;
            }

            return *this;
        }

        BaseInteractor::result PointGravityInteractor::computeAcceleration(const particle & p)
        {
            return std::make_pair(
                p.directionTo(position)*(((gravityCoeff) / p.distanceToSq(position))),
                true);
        }

        BaseInteractor * PointGravityInteractor::clone() const
        {
            return new PointGravityInteractor(*this);
        }
    }
}
