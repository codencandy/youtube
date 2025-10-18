#include <AppKit/AppKit.h>
#include <dlfcn.h>
#include <time.h>
#include <mach/mach_time.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "libs/imgui/imgui.h"
#include "libs/imgui/backends/imgui_impl_osx.h"
#include "libs/imgui/backends/imgui_impl_metal.h"

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

void setupImGui( MainRenderer* renderer )
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF( "res/blender_thin.ttf", 16.0f );
    
    // Setup Renderer backend
    ImGui_ImplMetal_Init( renderer->m_gpu );
    ImGui_ImplOSX_Init( renderer->m_view );

    io.DisplaySize.x = renderer->m_view.bounds.size.width;
    io.DisplaySize.y = renderer->m_view.bounds.size.height;

    CGFloat framebufferScale   = renderer->m_view.window.screen.backingScaleFactor ?: NSScreen.mainScreen.backingScaleFactor;
    io.DisplayFramebufferScale = ImVec2( framebufferScale, framebufferScale );

    ImGuiStyle& myStyle = ImGui::GetStyle();
    
    // padding & spacing
    myStyle.WindowPadding     = ImVec2( 12.0f, 12.0f );
    myStyle.FramePadding      = ImVec2( 12.0f, 8.0f );
    myStyle.ItemSpacing       = ImVec2( 12.0f, 12.0f );
    myStyle.ItemInnerSpacing  = ImVec2( 4.0f, 4.0f );
    myStyle.IndentSpacing     = 21.0f;

    // borders
    myStyle.FrameBorderSize         = 0.0f;
    myStyle.WindowBorderSize        = 0.0f;
    myStyle.SeparatorTextBorderSize = 1.0f;

    // rounding
    myStyle.WindowRounding    = 6.0f;
    myStyle.FrameRounding     = 3.0f;
    myStyle.ScrollbarRounding = 9.0f;
    myStyle.GrabRounding      = 2.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.13f, 0.13f, 0.70f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.35f, 0.79f, 0.81f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.09f, 0.10f, 0.11f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.17f, 0.18f, 0.20f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.45f, 0.45f, 0.45f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.09f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.23f, 0.82f, 0.72f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.88f, 0.77f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.88f, 0.77f, 0.24f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.38f, 0.39f, 0.38f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.27f, 0.28f, 0.30f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.80f, 0.80f, 0.80f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.23f, 0.28f, 0.35f, 0.86f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.41f, 0.44f, 0.47f, 0.80f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.39f, 0.45f, 0.52f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.9f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

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
    if( cnc_vector_test() )
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

    static MemoryPool* permanent = CreateMemoryPool( CNC_MEGABYTES(10) );
    static MemoryPool* transient = CreateMemoryPool( CNC_MEGABYTES(10) );

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

    window.contentView = renderer->m_view;

    Application* christmas = christmasLib.f_loadApp( permanent, transient, services, renderer );
    Application* clock     = clockLib.f_loadApp(     permanent, transient, services, renderer );
    Application* shape     = shapeLib.f_loadApp(     permanent, transient, services, renderer );
    Application* pong      = pongLib.f_loadApp(      permanent, transient, services, renderer );

    pong->m_timeInfo.m_startTime   = startTime;
    pong->m_timeInfo.m_elspaseTime = 0.0;

    setupImGui( renderer );
    
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