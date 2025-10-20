#include "CNC_CandyEngine.h"

void cnc::DrawRectangle( Application* app, v2 position, v2 size, Colour c )
{
    void*             renderer = app->m_renderer;
    PlatformServices* services = app->m_services;

    DrawCall call;
    call.m_type     = CNC_RECT;
    call.m_position = position;
    call.m_size     = size;
    call.m_color    = c;

    services->f_submitDrawCall( renderer, call );
}

void cnc::DrawCircle( Application* app, v2 position, f32 radius, Colour c )
{
    void*             renderer = app->m_renderer;
    PlatformServices* services = app->m_services;

    DrawCall call;
    call.m_type     = CNC_CIRCLE;
    call.m_position = position;
    call.m_radius   = radius;
    call.m_color    = c;

    services->f_submitDrawCall( renderer, call );
}

void cnc::DrawLine( Application* app, v2 start, v2 end, Colour c )
{
    void*             renderer = app->m_renderer;
    PlatformServices* services = app->m_services;

    DrawCall call;

    f32 d           = cnc_distance( start, end );
    call.m_type     = CNC_LINE;
    call.m_position = start;
    call.m_size     = vec2( d, 3.0f );
    call.m_angle    = -cnc_lineangle( start, end );
    call.m_color    = c;

    services->f_submitDrawCall( renderer, call );
}

void cnc::DrawRectangle( Application* app, Rectangle& rect )
{
    cnc::DrawRectangle( app, rect.m_position, rect.m_size, rect.m_color );
}

void cnc::DrawCircle( Application* app, Circle& circle )
{
    cnc::DrawCircle( app, circle.m_center, circle.m_radius, circle.m_color );
}

void cnc::DrawLine( Application* app, Line& line )
{
    cnc::DrawLine( app, line.m_start, line.m_end, line.m_color );
}

void cnc::DrawGrid( Application* app, Line* grid, u32 numLines )
{
    for( u32 i=0; i<numLines; ++i )
    {
        Line l = grid[i];
        cnc::DrawLine( app, l );
    }
}

void cnc::DrawImage( Application* app, Image* image, v2 position, v2 pivotPoint, f32 angle )
{
    void*             renderer = app->m_renderer;
    PlatformServices* services = app->m_services;

    DrawCall call;
    call.m_type         = CNC_IMAGE;
    call.m_textureId    = image->m_textureId;
    call.m_position     = position;
    call.m_pivotPoint   = pivotPoint;
    call.m_angle        = angle;
    call.m_numInstances = 1;

    services->f_submitDrawCall( renderer, call );
}

Image* cnc::CreateImage( Application* app, const char* imageName )
{
    Image* image = ALLOC_STRUCT( app->m_pool, Image );

    PlatformServices* services = app->m_services;
    void*             renderer = app->m_renderer;

    image              = services->f_loadImage( imageName );
    image->m_textureId = services->f_uploadImage( renderer, image );

    return image;
}

Rectangle cnc::CreateRectangle( v2 position, v2 size, Colour c )
{
    Rectangle rect;
    rect.m_position = position;
    rect.m_size     = size;
    rect.m_color    = c;
    rect.m_type     = CNC_RECT;

    return rect;
}

Circle cnc::CreateCircle( v2 center, f32 radius, Colour c )
{
    Circle circle;
    circle.m_center = center;
    circle.m_radius = radius;
    circle.m_color  = c;
    circle.m_type   = CNC_CIRCLE;

    return circle;
}

Line cnc::CreateLine( v2 start, v2 end, Colour c )
{
    Line line;
    line.m_start = start;
    line.m_end   = end;
    line.m_color = c;
    line.m_type  = CNC_LINE;

    return line;
}

Line* cnc::CreateGrid( MemoryPool* pool, v2 screenSize, f32 cellSize, u32* numLines )
{
    f32    w        = screenSize.x;
    f32    h        = screenSize.y;
    u32    cols     = w / cellSize;
    u32    rows     = h / cellSize;
    *numLines       = rows + cols;
    Line*  lines    = ALLOC_ARRAY( pool, Line, *numLines );
    Colour grey     = colour( 0.2f, 0.2f, 0.2f, 1.0f );

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

