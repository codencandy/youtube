#include "CNC_ShapeDemo.h"

Application* LoadApplication( cnc::MemoryPool* pool, cnc::MemoryPool* transient, cnc::UserInput* input, PlatformServices* services, void* renderer )
{
    ShapeDemo* app = ALLOC_STRUCT( pool, ShapeDemo );

    InitApplication( app, pool, transient, input, services, renderer );

    Colour red    = colour( 1.0f, 0.0f, 0.0f, 1.0f );
    Colour purple = colour( 1.0f, 0.0f, 1.0f, 1.0f );
    Colour white  = colour( 1.0f, 1.0f, 1.0f, 1.0f );

    app->m_testRect   = cnc::CreateRectangle( vec2( 10.0f, 10.0f ), vec2( 100.0f, 20.0f ), purple );
    app->m_testCircle = cnc::CreateCircle(    vec2( 100.0f, 100.0f ), 20.0f, red );
    app->m_testLine   = cnc::CreateLine(      vec2( 0.0f, 0.0f), vec2( 400.0f, 400.0f ), white );
    app->m_grid       = cnc::CreateGrid( pool, app->m_screenSize, 20.0f, &app->m_numGridLines );

    return app;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* application )
{
    ShapeDemo* app = (ShapeDemo*)application;
    
    cnc::DrawGrid     ( app, app->m_grid, app->m_numGridLines );    
    cnc::DrawRectangle( app, app->m_testRect );
    cnc::DrawCircle   ( app, app->m_testCircle );
    cnc::DrawLine     ( app, app->m_testLine );
}