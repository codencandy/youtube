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

    pong->m_ball.m_circle       = cnc::CreateCircle( center, 20.0f, white );
    pong->m_ball.m_position     = center;
    pong->m_ball.m_radius       = 20.0f;
    pong->m_ball.m_velocity     = vec2( 20.0f, 0.0f );
    pong->m_ball.m_acceleration = 0.0f;

    pong->m_padLeft.m_rect      = cnc::CreateRectangle( padLeft, padSize, red );
    pong->m_padLeft.m_normal    = vec2( 1.0f, 0.0f );

    pong->m_padRight.m_rect     = cnc::CreateRectangle( padRight, padSize, red );
    pong->m_padRight.m_normal   = vec2( -1.0f, 0.0f );

    pong->m_grid = cnc::CreateGrid( pool, pong->m_screenSize, 20.0f, &pong->m_numGridLines );

    return pong;
}

void UpdateApplication( Application* application )
{
    Pong* app = (Pong*)application;
    
    // p' = p + (v * dt)
    v2 position = app->m_ball.m_position;
    v2 velocity = app->m_ball.m_velocity;

    position                      = position + (velocity * app->m_timeInfo.m_dt);
    app->m_ball.m_position        = position;
    app->m_ball.m_circle.m_center = position;
}

void RenderApplication( Application* app )
{
    Pong* pong = (Pong*)app;
    
    cnc::DrawGrid     ( pong, pong->m_grid, pong->m_numGridLines );
    cnc::DrawCircle   ( pong, pong->m_ball.m_circle );
    cnc::DrawRectangle( pong, pong->m_padLeft.m_rect );
    cnc::DrawRectangle( pong, pong->m_padRight.m_rect );
}