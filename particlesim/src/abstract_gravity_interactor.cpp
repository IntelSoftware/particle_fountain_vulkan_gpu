#include <psim/abstract_gravity_interactor.h>

namespace psim {
	namespace interactors {
		AbstractGravityInteractor::AbstractGravityInteractor(float gConst)
			: gravityConstant(gConst)
		{ }
	}
}