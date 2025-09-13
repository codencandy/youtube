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

Line AddLine( v2 start, v2 end, Colour c )
{
    Line line;
    line.m_start = start;
    line.m_end   = end;
    line.m_color = c;

    return line;
}

Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    ShapeDemo* app = ALLOC_STRUCT( pool, ShapeDemo );

    InitApplication( app, pool, transient, services, renderer );

    Colour red    = colour( 1.0f, 0.0f, 0.0f, 1.0f );
    Colour purple = colour( 1.0f, 0.0f, 1.0f, 1.0f );
    Colour white  = colour( 1.0f, 1.0f, 1.0f, 1.0f );

    app->m_testRect   = AddRectangle( vec2( 10.0f, 10.0f ), vec2( 100.0f, 20.0f ), purple );
    app->m_testCircle = AddCircle(    vec2( 100.0f, 100.0f ), 20.0f, red );
    app->m_testLine   = AddLine(      vec2( 10.0f, 200.0f), vec2( 300.0f, 400.0f ), white );

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
    cnc::DrawLine     ( app, app->m_testLine );
}