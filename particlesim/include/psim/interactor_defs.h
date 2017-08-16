#ifndef _PSIM_INTERACTOR_DEFS_
#define _PSIM_INTERACTOR_DEFS_

namespace psim {
    namespace interactors {
        constexpr uint32_t MAX_INSTANCES = 4;
        constexpr float G = 6.67408e-11f;


        struct PointGravity
        {
            alignas(16) float pos[3];
            float massKG;
            float G;
        };

        struct PlanarGravity
        {
            alignas(16) float pos[3];
            alignas(16) float n[3];
            float massKG;
            float G;
            uint32_t deactivate;
        };

        struct ConstForce 
        {
            alignas(16) float direction[3];
            float magniutde;
        };

        struct Setup
        {
            PointGravity pointGravity[MAX_INSTANCES];
            PlanarGravity planarGravity[MAX_INSTANCES];
            ConstForce constForce[MAX_INSTANCES];

            uint32_t pointGravityCount;
            uint32_t planarGravityCount;
            uint32_t constForceCount;
        };
    }
}

#endif
