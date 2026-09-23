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

    services->f_renderRect(   renderer, vec2( 10.0f, 10.0f ), 200.0f, 30.0f, rgba(1.0f, 0.0f, 0.0f, 1.0f) );
    services->f_renderCircle( renderer, vec2( 100.0f, 100.0f ), 30.0f, rgba( 0.0f, 1.0f, 0.0f, 1.0f ) );
    services->f_renderLine(   renderer, vec2( 10.0f, 200.0f), vec2(210.0f, 300.0f), 5.0f, rgba( 0.0f, 0.0f, 1.0f, 1.0f) );
    services->f_renderLine(   renderer, vec2( 10.0f, 300.0f), vec2(210.0f, 200.0f), 5.0f, rgba( 0.0f, 1.0f, 1.0f, 1.0f) );

    TextDemo* textDemo = (TextDemo*)app;
    TtfFont*  font     = textDemo->m_ttfFont;
    Glyph*    g        = &font->m_glyphTable.m_glphys[0];

    f32 fontScale = (f32)(100.0f / font->m_headTable.m_unitsPerEm);

    for( u32 i=0; i<g->m_numPoints; ++i )
    {
        services->f_renderCircle( renderer, vec2( g->m_x[i] * fontScale, g->m_y[i] * fontScale ), 10.0f, rgba( 0.0f, 1.0f, 0.0f, 1.0f ) );
    }
}