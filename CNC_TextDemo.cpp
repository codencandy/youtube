#include "CNC_TextDemo.h"
#include "CNC_PlatformServices.h"

Application* LoadApplication( PlatformServices* services, void* renderer )
{
    TextDemo* textdemo = (TextDemo*)malloc( sizeof( TextDemo ) );
    textdemo->m_services = services;
    textdemo->m_renderer = renderer;

    File* testFile = textdemo->m_services->f_loadFile( "CNC_TextDemo.h" );
    
    return textdemo;
}

void UpdateApplication( Application* app )
{

}

void RenderApplication( Application* app )
{

}