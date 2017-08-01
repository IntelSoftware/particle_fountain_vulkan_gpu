#ifndef _PSIM_MODEL_
#define _PSIM_MODEL_

/*
scene size
conversion of particles to scene coords
emiters
interactors (physical, visual)
1)convert
2) discard (optional) by ttl
3) compute physics
bla bla remember about generator ranges in pos v and ttl depend on time 


*/

#include <chrono>
#include <list>
#include <psim/buffer.h>
#include <psim/base_generator.h>
#include <psim/base_interactor.h>

namespace psim {
	class Model final
	{
	public:
		using size = vec::vec3f;
		using GeneratorList = std::list<generators::BaseGenerator*>;
		using InteractorList = std::list<interactors::BaseInteractor*>;

		Model(size wSize, const GeneratorList& genList, const InteractorList& iList, int numThreads = 1);
		Model(const Model & m);
		Model& operator = (const Model & m);
		~Model();
		void progress(std::chrono::microseconds dt, Buffer & buffer);
	private:
		size worldSize;
		GeneratorList generators;
		InteractorList interactors;
		
		particle processParticle(base::time dt, particle p);
		void toWorldScale(particle & p);
		void toNormalizedScale(particle & p);


	};
}

#endif