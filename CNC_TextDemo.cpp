#include "CNC_TextDemo.h"
#include "CNC_PlatformServices.h"
#include "CNC_TtfTypes.h"
#include "CNC_Tools.h"
#include "CNC_Constants.h"

void fontDemo1( Application* app );
void fontDemo2( Application* app );


Application* LoadApplication( PlatformServices* services, void* renderer )
{
    TextDemo* textdemo = (TextDemo*)malloc( sizeof( TextDemo ) );
    textdemo->m_services = services;
    textdemo->m_renderer = renderer;

    StartTimer( &textdemo->m_timer );

    File* montserratFile  = services->f_loadFile( "./res/montserrat-regular.ttf" );
    File* bladerunnerFile = services->f_loadFile( "./res/bladerunner.ttf" );
    montserratFile->m_endianess  = BIGENDIAN;
    bladerunnerFile->m_endianess = BIGENDIAN;

    TtfFont* montserrat  = services->f_loadFont( montserratFile, renderer );
    TtfFont* bladerunner = services->f_loadFont( bladerunnerFile, renderer );

    textdemo->m_montserrat                          = montserrat;
    textdemo->m_bladerunner                         = bladerunner;
    textdemo->m_windowProperties.m_backgroundColor  = GREY;
    services->f_windowProperties( renderer, &textdemo->m_windowProperties );
    
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

    TextDemo* textDemo   = (TextDemo*)app;

    TtfFont*  montserrat  = textDemo->m_montserrat;
    TtfFont*  bladerunner = textDemo->m_bladerunner;

    static const char* capitals  = "A B C D E F G H I";
    static const char* umlauts   = "Ä ä Ü ü ß Ö ö & %";
    static const char* numbers   = "1 2 3 4 5 6 7 8 9";
    static const char* pangram   = "Sphinx of black quartz, judge my vow!";
    static const char* hawking   = 
"A well-known scientist (some say it was Bertrand Russell)\nonce gave a public \
lecture on astronomy.\nHe described how the earth orbits around the sun and how\n\
the sun, in turn, orbits around the center of a vast collection of stars called \
our galaxy.";

    static f32 fontSize = 32.0f;
    if( MouseWheelScrolled( app->m_input ) )
    {
        if( app->m_input->m_mouse.m_wheel > 0 )
            fontSize += 2.0f;
        else
            fontSize -= 2.0f;
    }

    services->f_renderText( renderer, app, bladerunner, capitals, vec2( 40.0f, 40.0f ),  fontSize, sanzo1.paleRawUmber );
    services->f_renderText( renderer, app, bladerunner, numbers , vec2( 40.0f, 80.0f ),  fontSize, sanzo1.ochraceousSalmon );
    services->f_renderText( renderer, app, montserrat,  umlauts , vec2( 40.0f, 120.0f ), fontSize, sanzo1.slateColor );
    services->f_renderText( renderer, app, montserrat,  pangram , vec2( 40.0f, 160.0f ), fontSize, sanzo1.sulpurYellow );
    services->f_renderText( renderer, app, montserrat,  hawking,  vec2( 40.0f, 250.0f ), fontSize, sanzo1.paleRawUmber );
}
