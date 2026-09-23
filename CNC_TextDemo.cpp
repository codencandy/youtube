#include "CNC_TextDemo.h"
#include "CNC_PlatformServices.h"
#include "CNC_TtfTypes.h"

Application* LoadApplication( PlatformServices* services, void* renderer )
{
    TextDemo* textdemo = (TextDemo*)malloc( sizeof( TextDemo ) );
    textdemo->m_services = services;
    textdemo->m_renderer = renderer;

    File* ttfFile = services->f_loadFile( "./res/montserrat-regular.ttf" );
    ttfFile->m_endianess = BIGENDIAN;

    TtfFont* ttfFont = services->f_loadFont( ttfFile );

    textdemo->m_ttfFont = ttfFont;
    
    return textdemo;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* app )
{
    PlatformServices* services = app->m_services;
    void*             renderer = app->m_renderer;

    TextDemo* textDemo = (TextDemo*)app;
    TtfFont*  font     = textDemo->m_ttfFont;
    
    f32    fontScale = (f32)(100.0f / font->m_headTable.m_unitsPerEm);
    f32    xOffset   = 50.0f;
    f32    yOffset   = 50.0f;
    Glyph* g         = NULL;

    for( u32 j=0; j<10; ++j )
    {
        g = &font->m_glyphTable.m_glphys[j];
        for( u32 i=0; i<g->m_numPoints; ++i )
        {
            services->f_renderCircle( renderer, vec2( g->m_x[i] * fontScale + xOffset, g->m_y[i] * fontScale + yOffset ), 5.0f, rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );
        }
        xOffset += 100.0f;
    }
}