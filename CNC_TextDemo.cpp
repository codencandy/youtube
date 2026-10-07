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
    const char* umlauts   = "Ä ä Ü ü ß Ö ö & %";
    const char* numbers   = "1 2 3 4 5 6 7 8 9";
    const char* pangram   = "Sphinx of black quartz, judge my vow!";
    const char* hawking   = 
"A well-known scientist (some say it was Bertrand Russell)\nonce gave a public \
lecture on astronomy.\nHe described how the earth orbits around the sun and how\n\
the sun, in turn, orbits around the center of a vast collection of stars called \
our galaxy.";
    
    services->f_renderText( renderer, app, font, hawking, vec2( 40.0f, 150.0f ), 32.0f );
}