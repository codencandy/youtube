#include "CNC_CandyEngine.h"
#include "CNC_Application.h"
#include "CNC_Types.h"

static CandyEngine CNC_ENGINE;

namespace cnc
{
    void InitCandyEngine( PlatformServices* services, void* renderer, Application* app )
    {
        CNC_ENGINE.m_renderer = renderer;
        CNC_ENGINE.m_services = services;
        CNC_ENGINE.m_app      = app;
    }

    void SetWindowProperties( window_props p, void* prop )
    {
        switch( p )
        {
            case CNC_FULLSCREEN:
            {
                bool* fullscreen = (bool*)prop;
                CNC_ENGINE.m_windowProperties.m_fullscreen = *fullscreen;
                break;
            }

            case CNC_SCREENSIZE:
            {
                v2int* size = (v2int*)prop;
                CNC_ENGINE.m_windowProperties.m_size = *size;
                break;
            }

            case CNC_BGCOLOR:
            {
                color* c = (color*)prop;
                CNC_ENGINE.m_windowProperties.m_backgroundColor = *c;
                break;
            }

            CNC_ENGINE.m_services->f_windowProperties( CNC_ENGINE.m_renderer, &CNC_ENGINE.m_windowProperties );
        }

    }

    TtfFont* LoadFont( const char* font )
    {
        File* fontFile  = CNC_ENGINE.m_services->f_loadFile( font );
        fontFile->m_endianess  = BIGENDIAN;

        TtfFont* ttfFont  = CNC_ENGINE.m_services->f_loadFont( fontFile, CNC_ENGINE.m_renderer );
        return ttfFont;
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
