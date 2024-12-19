#include "CNC_Christmas.h"
#include "CNC_PlatformServices.h"
#include "CNC_Math.h"

void LoadChristmas( ChristmasApp* app )
{
    app->m_foreground = PlatformLoadImage( "res/winter_front.png" );
    app->m_landscape  = PlatformLoadImage( "res/winter_complete.png" );

    app->m_landscape->m_modelData.m_modelMatrix = identityMatrix();
    app->m_landscape->m_modelData.m_pivotMatrix = identityMatrix();

    app->m_foreground->m_textureId = PlatformUploadImage( app->m_renderer, app->m_foreground );
    app->m_landscape->m_textureId  = PlatformUploadImage( app->m_renderer, app->m_landscape );
}

void UpdateChristmas( ChristmasApp* app )
{

}

void RenderChristmas( ChristmasApp* app )
{
    PlatformRenderImage( app->m_renderer, app->m_landscape->m_textureId );
    PlatformRenderParticles( app->m_renderer, app->m_foreground->m_textureId );
}