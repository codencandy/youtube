#include "CNC_ShapeDemo.h"
#include "CNC_CandyEngine.cpp"

Rectangle AddRectangle( v2 position, v2 size, Colour c )
{
    Rectangle rect;
    rect.m_position = position;
    rect.m_size     = size;
    rect.m_color    = c;

    return rect;
}

Circle AddCircle( v2 center, f32 radius, Colour c )
{
    Circle circle;
    circle.m_center = center;
    circle.m_radius = radius;
    circle.m_color  = c;

    return circle;
}

Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    ShapeDemo* app = ALLOC_STRUCT( pool, ShapeDemo );

    InitApplication( app, pool, transient, services, renderer );

    Colour red    = colour( 1.0f, 0.0f, 0.0f, 1.0f );
    Colour purple = colour( 1.0f, 0.0f, 1.0f, 1.0f );

    app->m_testRect   = AddRectangle( vec2( 10.0f, 10.0f ), vec2( 100.0f, 20.0f ), purple );
    app->m_testCircle = AddCircle(    vec2( 100.0f, 100.0f ), 20.0f, purple );

    return app;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* application )
{
    ShapeDemo* app = (ShapeDemo*)application;
    
    cnc::DrawRectangle( app, app->m_testRect );
    cnc::DrawCircle   ( app, app->m_testCircle );
}