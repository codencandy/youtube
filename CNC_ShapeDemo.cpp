#include "CNC_ShapeDemo.h"
#include "CNC_CandyEngine.cpp"

Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    ShapeDemo* app = ALLOC_STRUCT( pool, ShapeDemo );

    InitApplication( app, pool, transient, services, renderer );

    Colour red    = colour( 1.0f, 0.0f, 0.0f, 1.0f );
    Colour purple = colour( 1.0f, 0.0f, 1.0f, 1.0f );
    Colour white  = colour( 1.0f, 1.0f, 1.0f, 1.0f );

    app->m_testRect   = CreateRectangle( vec2( 10.0f, 10.0f ), vec2( 100.0f, 20.0f ), purple );
    app->m_testCircle = CreateCircle(    vec2( 100.0f, 100.0f ), 20.0f, red );
    app->m_testLine   = CreateLine(      vec2( 0.0f, 0.0f), vec2( 400.0f, 400.0f ), white );
    app->m_grid       = CreateGrid( pool, app->m_screenSize, 20.0f, &app->m_numGridLines );

    return app;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* application )
{
    ShapeDemo* app = (ShapeDemo*)application;
    
    for( u32 i=0; i<app->m_numGridLines; ++i )
    {
        Line l = app->m_grid[i];
        cnc::DrawLine( app, l );
    }
    
    cnc::DrawRectangle( app, app->m_testRect );
    cnc::DrawCircle   ( app, app->m_testCircle );
    cnc::DrawLine     ( app, app->m_testLine );

}