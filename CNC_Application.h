#ifndef CNC_APPLICATION_H
#define CNC_APPLICATION_H

#include "CNC_Memory.h"
#include "CNC_PlatformServices.h"

typedef struct TimeInfo
{
    u32 m_hours;
    u32 m_minute;

} TimeInfo;
typedef struct Application
{
    MemoryPool*       m_pool;
    PlatformServices* m_services;
    void*             m_renderer;
    TimeInfo          m_timeInfo;

} Application;

extern "C"
{
    Application* LoadApplication( MemoryPool* pool, PlatformServices* services, void* renderer );
    void         UpdateApplication( Application* app );
    void         RenderApplication( Application* app );

    typedef Application*(*loadapp_fcn)(MemoryPool*, PlatformServices*, void*);
    typedef void (*updateapp_fcn)(Application*);
    typedef void (*renderapp_fcn)(Application*);
}

typedef struct AppLib
{
    loadapp_fcn   f_loadApp;
    updateapp_fcn f_updateApp;
    renderapp_fcn f_renderApp;

} AppLib;

#endif//CNC_APPLICATION_H
