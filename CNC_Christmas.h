#ifndef CNC_CHRISTMAS_H
#define CNC_CHRISTMAS_H

#define CNC_CANDYENGINE_IMPLEMENTATION
#include "engine/CNC_CandyEngine.h"

typedef struct Christmas : Application
{
    Image*    m_landscape;
    Image*    m_skymask;
    Image*    m_snowmask;

    u32       m_numParticles;
    Particle* m_particles;

} Christmas;

#endif//CNC_CHRISTMAS_H
