#ifndef CNC_CANDYENGINE_H
#define CNC_CANDYENGINE_H

#include "CNC_Application.h"
#include "CNC_Types.h"
#include "CNC_Input.h"
#include "CNC_Math.h"
#include "CNC_Constants.h"
#include "CNC_PlatformServices.h"
#include "CNC_Timer.h"
#include "CNC_TtfTypes.h"

typedef struct CandyEngine
{
    PlatformServices* m_services;
    void*             m_renderer;
    Application*      m_app;
    WindowProperties  m_windowProperties;

} CandyEngine;

namespace cnc
{
    void     InitCandyEngine( PlatformServices* services, void* renderer, Application* app );
    void     SetWindowProperties( window_props p, void* prop );

    TtfFont* LoadFont( const char* font );

    void     DrawText( TtfFont* font, const char* text, v2 position, f32 size, color c );
    void     DrawRoundedRect( v2 pos, f32 width, f32 height, f32 radius, color c );
}

#ifdef CNC_CANDY_IMPLEMENTATION
#include "CNC_CandyEngine.cpp"
#endif

#endif//CNC_CANDYENGINE_H
