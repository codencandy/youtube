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

Line* AddGrid( MemoryPool* pool, v2 screenSize, f32 cellSize, u32* numLines )
{
    f32    w        = screenSize.x;
    f32    h        = screenSize.y;
    u32    cols     = w / cellSize;
    u32    rows     = h / cellSize;
    *numLines       = rows + cols;
    Line*  lines    = ALLOC_ARRAY( pool, Line, *numLines );
    Colour grey     = colour( 0.5f, 0.5f, 0.5f, 1.0f );

    f32 x = 0.0f;
    f32 y = 0.0f;

    for( u32 i=0; i<cols; ++i )
    {
        Line* l    = &lines[i];
        l->m_start = vec2( x, y );
        l->m_end   = vec2( x, screenSize.y );
        l->m_color = grey;

        x += cellSize;
    }

    x = 0.0f;
    y = 0.0f;

    for( u32 i=0; i<rows; ++i )
    {
        Line* l    = &lines[i + cols];
        l->m_start = vec2( x, y );
        l->m_end   = vec2( screenSize.x, y );
        l->m_color = grey;
        
        y += cellSize;
    }

    return lines;
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
    app->m_testLine   = AddLine(      vec2( 0.0f, 0.0f), vec2( 400.0f, 400.0f ), white );
    app->m_grid       = AddGrid( pool, app->m_screenSize, 20.0f, &app->m_numGridLines );

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