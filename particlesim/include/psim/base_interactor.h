#ifndef _PSIM_BASE_INTERACTOR_
#define _PSIM_BASE_INTERACTOR_

#include <utility>
#include <psim/particle.h>

namespace psim{
	namespace interactors {
		class BaseInteractor
		{
		public:
			using result = std::pair<vec::vec3Accel, bool>;
			virtual ~BaseInteractor() = default;

			virtual result computeAcceleration(const particle & p) = 0; 
			virtual BaseInteractor* clone() const = 0;
		};

	}
}

#endif