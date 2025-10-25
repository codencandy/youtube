#include <AppKit/AppKit.h>
#include <dlfcn.h>
#include <time.h>
#include <mach/mach_time.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../libs/imgui/imgui.h"
#include "../libs/imgui/backends/imgui_impl_osx.h"
#include "../libs/imgui/backends/imgui_impl_metal.h"

#define CNC_MEMORY_IMPLEMENTATION
#include "CNC_Memory.h"
#include "CNC_Types.h"
#include "CNC_Math.h"
#include "CNC_Application.h"
#include "CNC_Constants.h"

#include "CNC_Window.mm"
#include "CNC_Renderer.mm"

#include "CNC_PlatformServices.cpp"
#include "CNC_UserInterface.cpp"

void ProcessInput( cnc::UserInput* input, NSEvent* event )
{

}

AppLib loadLib( const char* libName )
{
    AppLib lib;
    
    void* dylib = dlopen( libName, RTLD_NOW  );

    if( dylib != NULL )
    {
        lib.f_loadApp   = (loadapp_fcn)  dlsym( dylib, "LoadApplication" );
        lib.f_updateApp = (updateapp_fcn)dlsym( dylib, "UpdateApplication" );
        lib.f_renderApp = (renderapp_fcn)dlsym( dylib, "RenderApplication" );
    }

    return lib;
}

int main()
{
    if( cnc::vector_test() )
    {
        printf( "v2 test: ok\n" );
    }
    else
    {
        printf( "v2 test: failed\n" );
    }

    // prepare timing
    f64                       startTime;
    mach_timebase_info_data_t timebase;
    mach_timebase_info( &timebase );

    u64 now   = mach_absolute_time();
    u64 nanos = now * timebase.numer / timebase.denom;
    startTime = (f64)nanos * 1e-9;

    bool running = true;

    static cnc::MemoryPool* permanent = cnc::CreateMemoryPool( CNC_MEGABYTES(10) );
    static cnc::MemoryPool* transient = cnc::CreateMemoryPool( CNC_MEGABYTES(10) );

    AppLib christmasLib = loadLib( "bin/christmas.dylib" );
    AppLib clockLib     = loadLib( "bin/clock.dylib" );
    AppLib shapeLib     = loadLib( "bin/shape.dylib" );
    AppLib pongLib      = loadLib( "bin/pong.dylib" );

    NSApplication* app = [NSApplication sharedApplication];

    [app setPresentationOptions: NSApplicationPresentationDefault];
    [app setActivationPolicy: NSApplicationActivationPolicyRegular];
    [app finishLaunching];

    MainWindow*       window   = CreateMainWindow( &running );
    MainRenderer*     renderer = CreateMainRenderer();
    PlatformServices* services = CreatePlatformServices();
    UserInterface*    ui       = CreateUserinterface();
    cnc::UserInput*   input    = cnc::CreateUserInput( permanent );

    window.contentView = renderer->m_view;

    Application* christmas = christmasLib.f_loadApp( permanent, transient, input, services, renderer );
    Application* clock     = clockLib.f_loadApp(     permanent, transient, input, services, renderer );
    Application* shape     = shapeLib.f_loadApp(     permanent, transient, input, services, renderer );
    Application* pong      = pongLib.f_loadApp(      permanent, transient, input, services, renderer );

    pong->m_timeInfo.m_startTime   = startTime;
    pong->m_timeInfo.m_elspaseTime = 0.0;

    SetupImGui( renderer );
    
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

                ProcessInput( input, event );                                            

                [app sendEvent: event];
                [app updateWindows];
            }
            while( event != NULL );

            // wait for display refresh
            [window->m_displaySignal wait];

            time_t rawTime;
            tm*    timeInfo;

            time( &rawTime );
            timeInfo = localtime( &rawTime );

            u64 currentTime  = mach_absolute_time();

            if( currentTime != 0 )
            {
                u64 currentNanos = currentTime * timebase.numer / timebase.denom;
                f64 elapsedTime  = ((f64)currentNanos * 1e-9) - startTime;
                
                pong->m_timeInfo.m_dt                = elapsedTime - pong->m_timeInfo.m_elspaseTime;
                pong->m_timeInfo.m_elspaseTime       = elapsedTime;
            }

            christmas->m_timeInfo.m_hours        = (timeInfo->tm_hour) % 12;
            christmas->m_timeInfo.m_minute       = timeInfo->tm_min;
            
            clock->m_timeInfo.m_hours            = (timeInfo->tm_hour) % 12;
            clock->m_timeInfo.m_minute           = timeInfo->tm_min;
            
            if( ui->m_christmasApp )
            {
                christmasLib.f_updateApp( christmas );
                christmasLib.f_renderApp( christmas );
            }

            if( ui->m_clockApp )
            {
                clockLib.f_updateApp( clock );
                clockLib.f_renderApp( clock );
            }

            if( ui->m_shapeApp )
            {
                shapeLib.f_updateApp( shape );
                shapeLib.f_renderApp( shape );
            }

            if( ui->m_pongApp )
            {
                pongLib.f_updateApp( pong );
                pongLib.f_renderApp( pong );
            }

            ImGui_ImplMetal_NewFrame( [renderer->m_view currentRenderPassDescriptor] );
            ImGui_ImplOSX_NewFrame( renderer->m_view);
            ImGui::NewFrame();

            ShowUserInterface( ui );

            ImGui::EndFrame();

            // render a new frame using the GPU
            Render( renderer );

            if( ImGui::IsKeyDown( ImGuiKey_Escape ) )
            {
                running = false;
            }
        }
    }

    return 0;
}