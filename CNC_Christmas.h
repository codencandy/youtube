#ifndef CNC_CHRISTMAS_H
#define CNC_CHRISTMAS_H

#include "CNC_Types.h"

typedef struct Christmas
{
    void* m_renderer;

    Image* m_landscape;
    Image* m_skymask;
    Image* m_snowmask;

    u32       m_numParticles;
    Particle* m_particles;

} Christmas;

void LoadChristmas( Christmas* app );
void UpdateChristmas( Christmas* app );
void RenderChristmas( Christmas* app );

#endif//CNC_CHRISTMAS_H
