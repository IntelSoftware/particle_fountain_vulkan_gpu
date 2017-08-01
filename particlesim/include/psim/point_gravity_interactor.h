#ifndef _PSIM_POINT_GRAVITY_INTERACTOR_
#define _PSIM_POINT_GRAVITY_INTERACTOR_

#include <psim/abstract_gravity_interactor.h>

namespace psim {
	namespace interactors {
		class PointGravityInteractor : public AbstractGravityInteractor
		{
		public:
			PointGravityInteractor(vec::vec3Coord pos, float massKG, float gConst = G);
			virtual ~PointGravityInteractor() = default;
			PointGravityInteractor(const PointGravityInteractor& o);
			PointGravityInteractor& operator=(const PointGravityInteractor& o);

			virtual result computeAcceleration(const particle & p) override; //force in Newtons
			virtual BaseInteractor* clone() const override;

		protected:
			vec::vec3Coord position;
			float mass;
			float gravityCoeff;
			

		};
	}
}

#endif // !_PSIM_POINT_GRAVITY_INTERACTOR_
