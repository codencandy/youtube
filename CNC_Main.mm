#include <AppKit/AppKit.h>
#include <dlfcn.h>

#include "CNC_Types.h"
#include "CNC_Application.h"

#include "CNC_Window.mm"
#include "CNC_Renderer.mm"
#include "CNC_PlatformServices.cpp"

AppLib loadLib( const char* libName )
{
    AppLib lib;
    
    void* dylib = dlopen( libName, RTLD_NOW  );

    if( dylib != NULL )
    {
        lib.f_loadApp   = (loadapp_fcn)dlsym( dylib, "LoadApplication" );
        lib.f_updateApp = (updateapp_fcn)dlsym( dylib, "UpdateApplication" );
        lib.f_renderApp = (renderapp_fcn)dlsym( dylib, "RenderApplication" );
    }

    return lib;
}

int main()
{
    bool running = true;

    AppLib christmasLib = loadLib( "christmas.dylib" );
    AppLib clockLib     = loadLib( "clock.dylib" );

    NSApplication* app = [NSApplication sharedApplication];

    [app setPresentationOptions: NSApplicationPresentationDefault];
    [app setActivationPolicy: NSApplicationActivationPolicyRegular];
    [app finishLaunching];

    MainWindow*       window   = CreateMainWindow( &running );
    MainRenderer*     renderer = CreateMainRenderer();
    PlatformServices* services = CreatePlatformServices();

    window.contentView = renderer->m_view;

    Application* christmas = christmasLib.f_loadApp( services, renderer );
    Application* clock     = clockLib.f_loadApp( services, renderer );
    
    while( running )
    {
        @autoreleasepool
        {
            NSEvent* event = NULL;

            do
            {
                event = [app nextEventMatchingMask: NSEventMaskAny
                                         untilDate: NULL
                                            inMode: NSDefaultRunLoopMode
                                           dequeue: true];

                [app sendEvent: event];
                [app updateWindows];
            }
            while( event != NULL );

            // wait for display refresh
            [window->m_displaySignal wait];

            christmasLib.f_updateApp( christmas );
            christmasLib.f_renderApp( christmas );

            clockLib.f_updateApp( clock );
            clockLib.f_renderApp( clock );

            // render a new frame using the GPU
            Render( renderer );
        }
    }

    return 0;
}