#include "CNC_TextDemo.h"
#include "CNC_PlatformServices.h"
#include "CNC_TtfTypes.h"

void fontDemo1( Application* app );
void fontDemo2( Application* app );


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
    fontDemo2( app );
}

void fontDemo2( Application* app )
{
    PlatformServices* services = app->m_services;
    void*             renderer = app->m_renderer;

    TextDemo* textDemo = (TextDemo*)app;
    TtfFont*  font     = textDemo->m_ttfFont;

    static const char* capitals  = "A B";

    services->f_renderTextIntersections( renderer, app, font, capitals , vec2( 40.0f, 250.0f ), 400.0f );
}

void fontDemo1( Application* app )
{
    PlatformServices* services = app->m_services;
    void*             renderer = app->m_renderer;

    TextDemo* textDemo = (TextDemo*)app;
    TtfFont*  font     = textDemo->m_ttfFont;

    static const char* capitals  = "A B C D E F G H I";
    static const char* umlauts   = "Ä ä Ü ü ß Ö ö & %";
    static const char* numbers   = "1 2 3 4 5 6 7 8 9";
    static const char* pangram   = "Sphinx of black quartz, judge my vow!";
    static const char* hawking   = 
"A well-known scientist (some say it was Bertrand Russell)\nonce gave a public \
lecture on astronomy.\nHe described how the earth orbits around the sun and how\n\
the sun, in turn, orbits around the center of a vast collection of stars called \
our galaxy.";
    
    services->f_renderText( renderer, app, font, capitals, vec2( 40.0f, 40.0f ),  32.0f );
    services->f_renderText( renderer, app, font, numbers , vec2( 40.0f, 80.0f ),  32.0f );
    services->f_renderText( renderer, app, font, umlauts , vec2( 40.0f, 120.0f ), 32.0f );
    services->f_renderText( renderer, app, font, pangram , vec2( 40.0f, 160.0f ), 32.0f );
    services->f_renderText( renderer, app, font, hawking,  vec2( 40.0f, 250.0f ), 32.0f );
}