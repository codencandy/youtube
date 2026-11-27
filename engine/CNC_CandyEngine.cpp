#include "CNC_CandyEngine.h"
#include "CNC_Application.h"
#include "CNC_Types.h"
#include "CNC_PlatformServices.h"

typedef struct CandyEngine
{
    PlatformServices* m_services;
    void*             m_renderer;
    Application*      m_app;
    WindowProperties  m_windowProperties;

} CandyEngine;

static CandyEngine CNC_ENGINE;

namespace cnc
{
    void InitCandyEngine( void* services, void* renderer, Application* app )
    {
        CNC_ENGINE.m_renderer = renderer;
        CNC_ENGINE.m_services = (PlatformServices*)services;
        CNC_ENGINE.m_app      = app;
    }

    void SetBackgroundColor( color c )
    {
        CNC_ENGINE.m_windowProperties.m_backgroundColor = c;
        CNC_ENGINE.m_services->f_windowProperties( CNC_ENGINE.m_renderer, &CNC_ENGINE.m_windowProperties );
    }

    void SetFullscreen( bool fullscreen )
    {
        CNC_ENGINE.m_windowProperties.m_fullscreen = fullscreen;
        CNC_ENGINE.m_services->f_windowProperties( CNC_ENGINE.m_renderer, &CNC_ENGINE.m_windowProperties );
    }

    void SetWindowSize( v2int size )
    {
        if( size.x <= 0 || size.y <= 0 )
            return;

        CNC_ENGINE.m_windowProperties.m_size = size;
        CNC_ENGINE.m_services->f_setWindowSize( CNC_ENGINE.m_renderer, size );
    }

    TtfFont* LoadFont( const char* font )
    {
        File* fontFile = CNC_ENGINE.m_services->f_loadFile( font );
        if( fontFile != NULL )
        {
            fontFile->m_endianess = BIGENDIAN;

            TtfFont* ttfFont = CNC_ENGINE.m_services->f_loadFont( fontFile, CNC_ENGINE.m_renderer );
            return ttfFont;
        }

        return NULL;
    }

    void DrawText( TtfFont* font, const char* text, v2 position, f32 size, color c )
    {
        CNC_ENGINE.m_services->f_renderText(  CNC_ENGINE.m_renderer, CNC_ENGINE.m_app, font, text, position, size, c );
    }

    void DrawRect( v2 pos, f32 width, f32 height, color c )
    {
        CNC_ENGINE.m_services->f_renderRect( CNC_ENGINE.m_renderer, pos, width, height, c );
    }

    void DrawRoundedRect( v2 pos, f32 width, f32 height, f32 radius, color c )
    {
        CNC_ENGINE.m_services->f_roundedRect( CNC_ENGINE.m_renderer, pos, width, height, radius, c );
    }

    void DrawCircle( v2 center, f32 radius, color c )
    {
        CNC_ENGINE.m_services->f_renderCircle( CNC_ENGINE.m_renderer, center, radius, c );
    }

    void DrawLine( v2 start, v2 end, f32 width, color c )
    {
        CNC_ENGINE.m_services->f_renderLine( CNC_ENGINE.m_renderer, start, end, width, c );
    }
}
