#include "CNC_CandyEngine.h"
#include "CNC_Math.h"
#include "CNC_Memory.h"

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

    f32 d           = distance( start, end );
    call.m_type     = CNC_LINE;
    call.m_position = start;
    call.m_size     = vec2( d, 1.0f );
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

Image* cnc::LoadImage( Application* app, const char* imageName )
{
    Image* image = ALLOC_STRUCT( app->m_pool, Image );

    PlatformServices* services = app->m_services;
    void*             renderer = app->m_renderer;

    image              = services->f_loadImage( imageName );
    image->m_textureId = services->f_uploadImage( renderer, image );

    return image;
}
