#include "CNC_Christmas.h"

Application* LoadApplication( cnc::MemoryPool* pool, cnc::MemoryPool* transient, cnc::UserInput* input, PlatformServices* services, void* renderer, void* ui )
{
    Christmas* app   = ALLOC_STRUCT( pool, Christmas );

    InitApplication( app, pool, transient, input, services, renderer, ui );

    app->m_landscape    = cnc::CreateImage( app, "res/landscape.png" );
    app->m_skymask      = cnc::CreateImage( app, "res/skymask.png" );
    app->m_snowmask     = cnc::CreateImage( app, "res/snowmask.png" );
    app->m_numParticles = 3000;
    app->m_particles    = ALLOC_ARRAY( app->m_pool, Particle, app->m_numParticles );

    cnc::srand( 300 );

    u32 snowFlakes = 2900;
    u32 stars      = 100;

    for( u32 i=0; i<snowFlakes; ++i )
    {
        Particle* p = &app->m_particles[i];

        f32 x     = (f32)(cnc::rand() % CNC_WINDOW_WIDTH );
        f32 y     = (f32)(cnc::rand() % CNC_WINDOW_HEIGHT ) - 300.0f;
        f32 speed = (f32)(cnc::rand() % 20)  / 10.0f;
        f32 size  = (f32)(cnc::rand() % 100) / 10.0f;

        p->m_position = vec2( x, y );
        p->m_speed    = speed;
        p->m_size     = size;
        p->m_time     = 0.0f;
    }

    for( u32 i=0; i<stars; ++i )
    {
        Particle* p = &app->m_particles[snowFlakes + i];

        f32 x     = (f32)(cnc::rand() % CNC_WINDOW_WIDTH );
        f32 y     = (f32)(cnc::rand() % CNC_WINDOW_HEIGHT - 150);
        f32 size  = (f32)(cnc::rand() % 150) / 10.0f;

        p->m_position = vec2( x, y );
        p->m_speed    = 0.0f;
        p->m_size     = size;
        p->m_time     = 0.0f;
    }

    services->f_uploadParticles( app->m_renderer, app->m_particles, app->m_numParticles );

    return app;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* application )
{
    Christmas* app = (Christmas*)application;
    PlatformServices* services = app->m_services;

    cnc::DrawImage( app, app->m_landscape, vec2( 0.0f, 0.0f ) );
    services->f_renderParticles( app->m_renderer, app->m_numParticles, app->m_snowmask->m_textureId, app->m_skymask->m_textureId );
}
