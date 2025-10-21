#include "CNC_Pong.h"

Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    Pong* pong = ALLOC_STRUCT( pool, Pong );

    InitApplication( pong, pool, transient, services, renderer );

    Colour white          = colour( 1.0f, 1.0f, 1.0f, 1.0f );
    Colour red            = colour( 1.0f, 0.0f, 0.0f, 1.0f );
    Colour grey           = colour( 0.3f, 0.3f, 0.3f, 1.0f );
    v2     center         = vec2( pong->m_screenSize.x / 2.0f, pong->m_screenSize.y / 2.0f );
    v2     padSize        = vec2( 20.0f, 200.0f );
    v2     padLeft        = vec2( 50.0f,                        pong->m_screenSize.y / 2.0f - padSize.y / 2.0f );
    v2     padRight       = vec2( pong->m_screenSize.x - 50.0f, pong->m_screenSize.y / 2.0f - padSize.y / 2.0f );
    v2     wallSize       = vec2( pong->m_screenSize.x, 20.0f );
    f32    ballRadius     = 20.0f;

    pong->m_colliders     = NULL;
    pong->m_ball          = cnc::CreateCircle( center, ballRadius, white );
    pong->m_padLeft       = cnc::CreateRectangle( padLeft, padSize, red );
    pong->m_padRight      = cnc::CreateRectangle( padRight, padSize, red );
    pong->m_wallTop       = cnc::CreateRectangle( vec2( 0.0f, 0.0f ), wallSize, grey );
    pong->m_wallBottom    = cnc::CreateRectangle( vec2( 0.0f, pong->m_screenSize.y - 20.0f ), wallSize, grey );
    pong->m_grid          = cnc::CreateGrid( pool, pong->m_screenSize, 20.0f, &pong->m_numGridLines );

    cnc::InitCollider( &pong->m_ball,       false, center,                            vec2( 200.0f, 10.0f ), 0.0f, 1.0f );
    cnc::InitCollider( &pong->m_padLeft,    true,  padLeft  + (padSize/2.0f),         vec2(0.0f,0.0f),       0.0f, 10.0f );
    cnc::InitCollider( &pong->m_padRight,   true,  padRight + (padSize/2.0f),         vec2(0.0f, 0.0f),      0.0f, 10.0f );
    cnc::InitCollider( &pong->m_wallTop,    true,  vec2(0.0f,0.0f) + (wallSize/2.0f), vec2(0.0f,0.0f),       0.0f, 10.0f );
    cnc::InitCollider( &pong->m_wallBottom, true,  vec2( 0.0f, pong->m_screenSize.y - 20.0f ) + ( wallSize / 2.0f ), vec2(0.0f,0.0f), 0.0f, 10.0f );

    arrput( pong->m_colliders, &pong->m_ball );
    arrput( pong->m_colliders, &pong->m_padLeft );
    arrput( pong->m_colliders, &pong->m_padRight );
    arrput( pong->m_colliders, &pong->m_wallTop );
    arrput( pong->m_colliders, &pong->m_wallBottom );

    pong->m_leftBounds  = 0.0f - ballRadius;
    pong->m_rightBounds = pong->m_screenSize.x + ballRadius;

    return pong;
}

void UpdateApplication( Application* application )
{
    Pong* app        = (Pong*)application;
    u32   shapeCount = arrlen( app->m_colliders );
    Ball* ball       = &app->m_ball;
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
                cnc::ResolveCollision( app->m_colliders[i], app->m_colliders[j], c, 1.0f, 0.0f) ;
            }
        }
    }

    if( ball->m_center.x < app->m_leftBounds || ball->m_center.x > app->m_rightBounds )
    {
        ball->m_position = vec2( app->m_screenSize.x / 2.0f, app->m_screenSize.y / 2.0f );
        ball->m_velocity = vec2( 200.0f, 10.0f );
        ball->m_center   = ball->m_position;
    }
}

void RenderApplication( Application* app )
{
    Pong* pong = (Pong*)app;
    
    cnc::DrawGrid     ( pong, pong->m_grid, pong->m_numGridLines );
    cnc::DrawRectangle( pong, pong->m_wallTop );
    cnc::DrawRectangle( pong, pong->m_wallBottom );
    cnc::DrawCircle   ( pong, pong->m_ball );
    cnc::DrawRectangle( pong, pong->m_padLeft );
    cnc::DrawRectangle( pong, pong->m_padRight );
}
