#include "CNC_TextDemo.h"
#include "CNC_PlatformServices.h"
#include "CNC_TtfTypes.h"

Application* LoadApplication( PlatformServices* services, void* renderer )
{
    TextDemo* textdemo = (TextDemo*)malloc( sizeof( TextDemo ) );
    textdemo->m_services = services;
    textdemo->m_renderer = renderer;

    StartTimer( &textdemo->m_timer );

    File* ttfFile = services->f_loadFile( "./res/montserrat-regular.ttf" );
    ttfFile->m_endianess = BIGENDIAN;

    TtfFont* ttfFont = services->f_loadFont( ttfFile );

    textdemo->m_ttfFont = ttfFont;
    
    return textdemo;
}

void UpdateApplication( Application* app )
{
    UpdateTimer( &app->m_timer );
}

void RenderApplication( Application* app )
{
    PlatformServices* services = app->m_services;
    void*             renderer = app->m_renderer;

    TextDemo* textDemo = (TextDemo*)app;
    TtfFont*  font     = textDemo->m_ttfFont;

    const char* capitals  = "A B C D E F G H I";
    const char* lowercase = "a b c d e f g h i";

    f32 sizeFactor = 30.0f * fabs( sinf( app->m_timer.m_seconds / 2.0f ) );
    
    services->f_renderText( renderer, app, font, capitals, vec2( 40.0f, 150.0f ), 150.0f + sizeFactor );
}