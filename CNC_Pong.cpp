#include "CNC_Pong.h"

Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    Pong* pong = ALLOC_STRUCT( pool, Pong );

    InitApplication( pong, pool, transient, services, renderer );

    Colour white    = colour( 1.0f, 1.0f, 1.0f, 1.0f );
    Colour red      = colour( 1.0f, 0.0f, 0.0f, 1.0f );
    v2     center   = vec2( pong->m_screenSize.x / 2.0f, pong->m_screenSize.y / 2.0f );
    v2     padSize  = vec2( 20.0f, 200.0f );
    v2     padLeft  = vec2( 50.0f,                        pong->m_screenSize.y / 2.0f - padSize.y / 2.0f );
    v2     padRight = vec2( pong->m_screenSize.x - 50.0f, pong->m_screenSize.y / 2.0f - padSize.y / 2.0f );

    pong->m_colliders               = NULL;
    pong->m_ball                    = cnc::CreateCircle( center, 20.0f, white );
    pong->m_ball.m_position         = center;
    pong->m_ball.m_center           = center;
    pong->m_ball.m_static           = false;
    pong->m_ball.m_radius           = 20.0f;
    pong->m_ball.m_velocity         = vec2( 100.0f, 0.0f );
    pong->m_ball.m_acceleration     = 0.1f;
    pong->m_ball.m_mass             = 1.0f;
 
    pong->m_padLeft                 = cnc::CreateRectangle( padLeft, padSize, red );
    pong->m_padLeft.m_center        = padLeft + (padSize / 2.0f);
    pong->m_padLeft.m_static        = true;
    pong->m_padLeft.m_mass          = 10.0f;
    pong->m_padLeft.m_velocity      = 0.0f;
    pong->m_padLeft.m_acceleration  = 0.0f;

    pong->m_padRight                = cnc::CreateRectangle( padRight, padSize, red );
    pong->m_padRight.m_center       = padRight + (padSize / 2.0f);
    pong->m_padRight.m_static       = true;
    pong->m_padRight.m_mass         = 10.0f;
    pong->m_padRight.m_velocity     = 0.0f;
    pong->m_padRight.m_acceleration = 0.0f;

    pong->m_grid                = cnc::CreateGrid( pool, pong->m_screenSize, 20.0f, &pong->m_numGridLines );

    arrput( pong->m_colliders, &pong->m_ball );
    arrput( pong->m_colliders, &pong->m_padLeft );
    arrput( pong->m_colliders, &pong->m_padRight );

    return pong;
}

void UpdateApplication( Application* application )
{
    Pong* app        = (Pong*)application;
    u32   shapeCount = arrlen( app->m_colliders );

    Ball* ball = &app->m_ball;
    ball->m_position = ball->m_position + (ball->m_velocity * app->m_timeInfo.m_dt);
    ball->m_center   = ball->m_position;

    // simulation loop
    for( u32 i = 0; i < shapeCount; ++i ) 
    {
        for( u32 j = i + 1; j < shapeCount; ++j ) 
        {
            Contact c = cnc::DetectCollision( app->m_colliders[i], app->m_colliders[j] );
            if( c.m_colliding ) 
            {
                cnc::ResolveCollision( app->m_colliders[i], app->m_colliders[j], c, 0.6f, 0.0f) ;
            }
        }
    }
}

void RenderApplication( Application* app )
{
    Pong* pong = (Pong*)app;
    
    cnc::DrawGrid     ( pong, pong->m_grid, pong->m_numGridLines );
    cnc::DrawCircle   ( pong, pong->m_ball );
    cnc::DrawRectangle( pong, pong->m_padLeft );
    cnc::DrawRectangle( pong, pong->m_padRight );
}