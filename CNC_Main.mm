#include <AppKit/AppKit.h>

#ifdef UNITY_BUILD
#include "CNC_Window.mm"
#include "CNC_Renderer.mm"
#else
#include "CNC_Window.h"
#include "CNC_Renderer.h"
#endif

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

            // render a new frame using the GPU
            Render( renderer );
        }
    }

    return 0;
}