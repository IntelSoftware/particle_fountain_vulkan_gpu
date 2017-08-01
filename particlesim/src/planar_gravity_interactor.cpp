#include <psim/planar_gravity_interactor.h>

namespace psim {
	namespace interactors {
		PlanarGravityInteractor::PlanarGravityInteractor(vec::vec3Coord position, vec::vec3Coord normal, float massKG, float gConst, bool deactivateOnContact)
			: AbstractGravityInteractor(gConst)
			, pos(position)
			, n(normal)
			, mass(massKG)
			, deactivate(deactivateOnContact)
			,gravityCoeff(massKG*gConst)
		{ 
			d = -(pos[axis::X] * n[axis::X] + pos[axis::Y] * n[axis::Y] + pos[axis::Z] * n[axis::Z]);
		}

		PlanarGravityInteractor::PlanarGravityInteractor(const PlanarGravityInteractor & o)
			: AbstractGravityInteractor(o)
			, pos(o.pos)
			, n(o.n)
			, mass(o.mass)
			, d(o.d)
			, deactivate(o.deactivate)
			,gravityCoeff(o.gravityCoeff)
		{ }

		PlanarGravityInteractor & PlanarGravityInteractor::operator=(const PlanarGravityInteractor & o)
		{
			if (this != &o) {
				AbstractGravityInteractor::operator=(o);
				pos = o.pos;
				n = o.n;
				mass = o.mass;
				d = o.d;
				gravityCoeff = o.gravityCoeff;
			}

			return *this;
		}

		BaseInteractor::result PlanarGravityInteractor::computeAcceleration(const particle & p)
		{
			/*
			Main driving equation is the distance equation from point P1=[x1,y1,z1] to the plane.
			normal n=[a,b,c]
			point on the plane P=[x,y,z]
			D = abs(a*x1+b*y1+c*z1+d)/sqrt(norm(n))
			d = -(a*x+b*y+c*z)
			NOTE1: Assuming that normal vector is of the length 1 all the time we can drop the denominator
			NOTE2: Initially we are not using abs to get a signed distance. If the resulting distance is positive this mean the point is above 
					the plane, if negative then below. When point is above the direction vector of force is -n (normal vector) otherwise is n.
			*/
			float D = p.pos[axis::X] * n[axis::X] + p.pos[axis::Y] * n[axis::Y] + p.pos[axis::Z] * n[axis::Z] + d;
			vec::vec3f r;
			if (D > 0.0f) {
				r = n * -1;
			} else {
				if (deactivate == true) {
					return std::make_pair(vec::vec3Force(), false);
				}

				r = n;
				D = -D;
			}

			return std::make_pair(
				r * ((gravityCoeff) / (D*D)),
				true
			);
		}

		BaseInteractor * PlanarGravityInteractor::clone() const
		{
			return new PlanarGravityInteractor(*this);
		}
	}
}