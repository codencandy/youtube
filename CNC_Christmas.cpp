#include "CNC_Christmas.h"
#include "CNC_PlatformServices.h"
#include "CNC_Math.h"
#include "CNC_Constants.h"

void LoadChristmas( Christmas* app )
{
    app->m_landscape = PlatformLoadImage( "res/landscape.png" );
    app->m_skymask   = PlatformLoadImage( "res/skymask.png" );
    app->m_snowmask  = PlatformLoadImage( "res/snowmask.png" );

    app->m_landscape->m_modelData.m_pivotMatrix = identityMatrix();
    app->m_landscape->m_modelData.m_modelMatrix = identityMatrix();

    app->m_landscape->m_textureId = PlatformUploadImage( app->m_renderer, app->m_landscape );
    app->m_skymask->m_textureId   = PlatformUploadImage( app->m_renderer, app->m_skymask );
    app->m_snowmask->m_textureId  = PlatformUploadImage( app->m_renderer, app->m_snowmask );

    app->m_numParticles = 3000;
    app->m_particles    = (Particle*)malloc( sizeof( Particle ) * app->m_numParticles );

    srand( 300 );

    u32 snowFlakes = 2900;
    u32 stars      = 100;

    for( u32 i=0; i<snowFlakes; ++i )
    {
        Particle* p = &app->m_particles[i];

        f32 x     = (f32)(rand() % CNC_WINDOW_WIDTH );
        f32 y     = (f32)(rand() % CNC_WINDOW_HEIGHT ) - 300.0f;
        f32 speed = (f32)(rand() % 20)  / 10.0f;
        f32 size  = (f32)(rand() % 100) / 10.0f;

        p->m_position = vec2( x, y );
        p->m_speed    = speed;
        p->m_size     = size;
        p->m_time     = 0.0f;
    }

    for( u32 i=0; i<stars; ++i )
    {
        Particle* p = &app->m_particles[snowFlakes + i];

        f32 x     = (f32)(rand() % CNC_WINDOW_WIDTH );
        f32 y     = (f32)(rand() % CNC_WINDOW_HEIGHT - 150);
        f32 size  = (f32)(rand() % 150) / 10.0f;

        p->m_position = vec2( x, y );
        p->m_speed    = 0.0f;
        p->m_size     = size;
        p->m_time     = 0.0f;
    }

    PlatformUploadParticles( app->m_renderer, app->m_particles, app->m_numParticles );
}

void UpdateChristmas( Christmas* app )
{

}

void RenderChristmas( Christmas* app )
{
    PlatformRenderImage( app->m_renderer, app->m_landscape->m_textureId );
    PlatformRenderParticles( app->m_renderer, app->m_numParticles, app->m_snowmask->m_textureId, app->m_skymask->m_textureId );
}
