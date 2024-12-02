#include <AppKit/AppKit.h>

#include "CNC_Types.h"
#include "CNC_Window.mm"
#include "CNC_Renderer.mm"
#include "CNC_PlatformServices.cpp"
#include "CNC_ClockApp.cpp"

int main()
{
    bool running = true;

    NSApplication* app = [NSApplication sharedApplication];

    [app setPresentationOptions: NSApplicationPresentationDefault];
    [app setActivationPolicy: NSApplicationActivationPolicyRegular];
    [app finishLaunching];

    MainWindow*   window   = CreateMainWindow( &running );
    MainRenderer* renderer = CreateMainRenderer();

    window.contentView = renderer->m_view;

    ClockApp clock = {0};
    clock.m_renderer = renderer;

    LoadClockApp( &clock );

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

            UpdateClockApp( &clock );
            RenderClockApp( &clock );

            // render a new frame using the GPU
            Render( renderer );
        }
    }

    return 0;
}