#include <psim/model.h>
#include <omp.h>
#include <iostream>

namespace psim {
	Model::Model(size wSize, const GeneratorList & genList, const InteractorList& iList, int numThreads)
		: worldSize(wSize)
	{
		for (const generators::BaseGenerator* g : genList) {
			generators::BaseGenerator *tmp = g->clone();
			tmp->setWorldSize(wSize);
			generators.push_back(tmp);
		}

		for (const interactors::BaseInteractor* i : iList) {
			interactors.push_back(i->clone());
		}

		omp_set_num_threads(numThreads);
	}

	Model::Model(const Model & m)
		: worldSize(m.worldSize)
	{
		for (const generators::BaseGenerator* g : m.generators) {
			generators.push_back(g->clone());
		}

		for (const interactors::BaseInteractor* i : m.interactors) {
			interactors.push_back(i->clone());
		}
	}

	Model & Model::operator=(const Model & m)
	{
		if (this != &m) {
			worldSize = m.worldSize;
			for (generators::BaseGenerator* g : generators) {
				delete g;
				g = nullptr;
			}
			generators.clear();

			for (const generators::BaseGenerator* g : m.generators) {
				generators.push_back(g->clone());
			}

			for (interactors::BaseInteractor *i : interactors) {
				delete i;
				i = nullptr;
			}
			interactors.clear();

			for (const interactors::BaseInteractor* i : m.interactors) {
				interactors.push_back(i->clone());
			}
		}

		return *this;
	}

	Model::~Model()
	{
		for (generators::BaseGenerator* g : generators) {
			delete g;
			g = nullptr;
		}
		generators.clear();

		for (interactors::BaseInteractor *i : interactors) {
			delete i;
			i = nullptr;
		}
		interactors.clear();
	}

	void Model::progress(std::chrono::microseconds dt, Buffer & buffer)
	{
		base::time dt_sec = dt.count() *1.0e-6f;

		#pragma omp parallel for schedule(static, 10)
		for (int32_t i = 0; i < buffer.activeCount(); ++i) {

			buffer[i] = processParticle(dt_sec, buffer[i]);
		}

		buffer.sort();
		for (generators::BaseGenerator* g : generators) {
			g->generate(buffer, dt_sec);
		}
	}

	particle Model::processParticle(base::time dt, particle p)
	{
		p.ttl -= dt;
		if (p.ttl > 0) {
			toWorldScale(p);
			vec::vec3Accel a{ 0, 0, 0 };

			bool keepAlive = true;

			for (interactors::BaseInteractor* i : interactors) {
				interactors::BaseInteractor::result r = i->computeAcceleration(p);
				a = a + r.first;
				keepAlive &= r.second;
			}
			if (!keepAlive) {
				p.ttl = -1;
			}

			p.velocity = p.velocity + (a * dt);
			p.pos = p.pos + (p.velocity * dt);

			//modify appearance

			toNormalizedScale(p);
		}
		return p;
	}

	void Model::toWorldScale(particle & p)
	{
		p.pos = { p.pos[axis::X] * worldSize[axis::X],
				p.pos[axis::Y] * worldSize[axis::Y],
				p.pos[axis::Z] * worldSize[axis::Z] };
	}

	void Model::toNormalizedScale(particle & p)
	{
		p.pos = { p.pos[axis::X] / worldSize[axis::X],
				p.pos[axis::Y] / worldSize[axis::Y],
				p.pos[axis::Z] / worldSize[axis::Z] };
	}
}

