#include "CNC_TextDemo.h"
#include "CNC_CandyEngine.h"

Application* LoadApplication( void* services, void* renderer )
{
    TextDemo* textdemo = (TextDemo*)malloc( sizeof( TextDemo ) );

    cnc::InitCandyEngine( services, renderer, textdemo);
    cnc::SetBackgroundColor( WHITE );

    textdemo->m_montserrat  = cnc::LoadFont( "./res/montserrat-regular.ttf" );
    textdemo->m_bladerunner = cnc::LoadFont( "./res/bladerunner.ttf" );

    return textdemo;
}

void UpdateApplication( Application* app )
{
}

void RenderApplication( Application* app )
{
    TextDemo* textDemo    = (TextDemo*)app;
    TtfFont*  montserrat  = textDemo->m_montserrat;
    TtfFont*  bladerunner = textDemo->m_bladerunner;

    static       f32   fontSize  = 32.0f;
    static const char* title     = "Stephen Hawing";
    static const char* hawking   =
"A well-known scientist (some say it was Bertrand Russell) once gave a public \
lecture on astronomy. He described how the earth orbits around the sun and how \
the sun, in turn, orbits around the center of a vast collection of stars called \
our galaxy.";

    if( MouseWheelScrolled( app->m_input ) )
    {
        if( app->m_input->m_mouse.m_wheel > 0 )
            fontSize += 1.0f;
        else
            fontSize -= 1.0f;
    }

    cnc::DrawText( bladerunner, title,   vec2( 40.0f, 100.0f ), fontSize * 2.0, sanzo296.slateColor );
    cnc::DrawText( montserrat,  hawking, vec2( 40.0f, 250.0f ), fontSize,       sanzo296.paleRawUmber );
    cnc::DrawRoundedRect( vec2( 40.0f, 500.0f ), 100.0f, 40.0f, 5.0f, sanzo296.slateColor );
}
