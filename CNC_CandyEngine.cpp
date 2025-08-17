#include "CNC_CandyEngine.h"
#include "CNC_Math.h"
#include "CNC_Memory.h"

void cnc::DrawRectangle( Application* app, v2 position, v2 size, Colour c )
{
    void*             renderer = app->m_renderer;
    PlatformServices* services = app->m_services;

}

void cnc::DrawCircle( Application* app, v2 position, f32 radius, Colour c )
{

}

void cnc::DrawLine( Application* app, v2 start, v2 end, Colour c )
{

}

void cnc::DrawImage( Application* app, Image* image, v2 position, v2 pivotPoint, f32 angle )
{
    void*             renderer = app->m_renderer;
    PlatformServices* services = app->m_services;

    f32 centerX = (f32)image->m_width  / 2.0f;
    f32 centerY = (f32)image->m_height / 2.0f;

    ModelData& modelData = image->m_modelData;
    modelData.m_modelMatrix = translationMatrix( position.x, position.y );
    modelData.m_pivotMatrix = translationMatrix( pivotPoint.x, pivotPoint.y );
    modelData.m_rotation.x  = angle;
    modelData.m_data.x      = CNC_IMAGE;
    
    services->f_updateImage( renderer, image );
    services->f_renderImage( renderer, image->m_textureId, 1 );
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
