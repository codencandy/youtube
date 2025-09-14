#ifndef CNC_APPLICATION_H
#define CNC_APPLICATION_H

#include "CNC_Constants.h"
#include "CNC_Memory.h"
#include "CNC_PlatformServices.h"
#include "CNC_Libs.h"
#include "libs/imgui/imgui.h"

#ifndef NULL
#define NULL 0x00
#endif

typedef struct TimeInfo
{
    u32 m_hours;
    u32 m_minute;

} TimeInfo;
typedef struct Application
{
    MemoryPool*       m_pool;
    MemoryPool*       m_transient;
    PlatformServices* m_services;
    void*             m_renderer;
    TimeInfo          m_timeInfo;
    ImGuiIO*          m_io;
    v2                m_screenSize;

} Application;

extern "C"
{
    Application* LoadApplication( MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer );
    void         UpdateApplication( Application* app );
    void         RenderApplication( Application* app );

    typedef Application*(*loadapp_fcn)(MemoryPool*, MemoryPool*, PlatformServices*, void*);
    typedef void (*updateapp_fcn)(Application*);
    typedef void (*renderapp_fcn)(Application*);
}

typedef struct AppLib
{
    loadapp_fcn   f_loadApp;
    updateapp_fcn f_updateApp;
    renderapp_fcn f_renderApp;

} AppLib;

void InitApplication( Application* app, MemoryPool* pool, MemoryPool* transient, PlatformServices* services, void* renderer )
{
    app->m_pool       = pool;
    app->m_transient  = transient;
    app->m_services   = services;
    app->m_renderer   = renderer;
    app->m_screenSize = vec2( CNC_WINDOW_WIDTH, CNC_WINDOW_HEIGHT );
}

#endif//CNC_APPLICATION_H
