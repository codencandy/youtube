#include "CNC_Pong.h"
#include "CNC_Application.h"
#include "CNC_CandyEngine.cpp"

Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    Pong* pong = ALLOC_STRUCT( pool, Pong );

    InitApplication( pong, pool, transient, services, renderer );

    Colour white  = colour( 1.0f, 1.0f, 1.0f, 1.0 );
    v2     center = vec2( pong->m_screenSize.x / 2.0f, pong->m_screenSize.y / 2.0f );

    pong->m_ball.m_cirlce = cnc::CreateCircle( center, 20.0f, white );
    pong->m_grid          = cnc::CreateGrid( pool, pong->m_screenSize, 20.0f, &pong->m_numGridLines );

    return pong;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* app )
{
    Pong* pong = (Pong*)app;
    
    cnc::DrawGrid  ( pong, pong->m_grid, pong->m_numGridLines );
    cnc::DrawCircle( pong, pong->m_ball.m_cirlce );
    
}