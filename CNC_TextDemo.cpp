#include "CNC_TextDemo.h"
#include "CNC_PlatformServices.h"
#include "CNC_TtfTypes.h"
#include "CNC_Constants.h"

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

    static const char* title     = "Stephen Hawing";
    static const char* hawking   =
"A well-known scientist (some say it was Bertrand Russell) once gave a public \
lecture on astronomy. He described how the earth orbits around the sun and how \
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

    services->f_renderText(  renderer, app, bladerunner, title, vec2( 40.0f, 100.0f ),  fontSize * 2.0, sanzo296.slateColor );
    services->f_renderText(  renderer, app, montserrat,  hawking,  vec2( 40.0f, 250.0f ), fontSize, sanzo296.paleRawUmber );
    services->f_roundedRect( renderer, vec2( 40.0f, 500.0f ), 100.0f, 40.0f, 5.0f, sanzo296.slateColor );
}
