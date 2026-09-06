#include "CNC_TextDemo.h"
#include "CNC_PlatformServices.h"

Application* LoadApplication( PlatformServices* services, void* renderer )
{
    TextDemo* textdemo = (TextDemo*)malloc( sizeof( TextDemo ) );
    textdemo->m_services = services;
    textdemo->m_renderer = renderer;

    File* ttfFile = services->f_loadFile( "./res/montserrat-regular.ttf" );
    ttfFile->m_endianess = BIGENDIAN;

    Font* ttfFont = services->f_loadFont( ttfFile );

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
}