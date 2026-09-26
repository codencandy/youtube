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

    const char* demoText = "A B C D E F G H I J K L M O P Q R S T U V W X Y Z";
    
    services->f_renderText( renderer, services, font, demoText, vec2( 50.0f, 100.0f ), 90.0f );
}