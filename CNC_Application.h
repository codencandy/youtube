#ifndef CNC_APPLICATION_H
#define CNC_APPLICATION_H

#include "CNC_PlatformServices.h"

typedef struct Application
{
    PlatformServices* m_services;
    void*             m_renderer;

} Application;

extern "C"
{
    Application* LoadApplication( PlatformServices* services, void* renderer );
    void         UpdateApplication( Application* app );
    void         RenderApplication( Application* app );

    typedef Application*(*loadapp_fcn)(PlatformServices*, void*);
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
