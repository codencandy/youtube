#include <AppKit/AppKit.h>

#include "CNC_Types.h"
#include "CNC_Window.mm"
#include "CNC_Renderer.mm"
#include "CNC_PlatformServices.cpp"
#include "CNC_Breakout.cpp"

int main()
{
    bool running = true;

    NSApplication* app = [NSApplication sharedApplication];

    [app setPresentationOptions: NSApplicationPresentationDefault];
    [app setActivationPolicy: NSApplicationActivationPolicyRegular];
    [app finishLaunching];

    MainWindow*   window    = CreateMainWindow( &running );
    MainRenderer* renderer  = CreateMainRenderer();
    UserInput     userInput = {0};

    window.contentView = renderer->m_view;

    Breakout breakout = {0};
    breakout.m_renderer = renderer;
    breakout.m_input    = &userInput;

    LoadBreakout( &breakout );

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

                if( !ProcessInput( &userInput, event ) )
                {
                    [app sendEvent: event];
                    [app updateWindows];
                }
            }
            while( event != NULL );

            // wait for display refresh
            [window->m_displaySignal wait];

            UpdateBreakout( &breakout );
            RenderBreakout( &breakout );

            // render a new frame using the GPU
            Render( renderer );
        }
    }

    return 0;
}

bool ProcessInput( UserInput* input, void* event )
{
    NSEvent* e = (NSEvent*)event;

    if( e.type == NSEventTypeKeyDown )
    {
        switch( e.keyCode )
        {
            case ARROW_UP_CODE:    { input->m_up.m_isDown    = true;  return true; }
            case ARROW_DOWN_CODE:  { input->m_down.m_isDown  = true;  return true; }
            case ARROW_LEFT_CODE:  { input->m_left.m_isDown  = true;  return true; }
            case ARROW_RIGHT_CODE: { input->m_right.m_isDown = true;  return true; }
            default: break;
        }
    }

    if( e.type == NSEventTypeKeyUp )
    {
        switch( e.keyCode )
        {
            case ARROW_UP_CODE:    { input->m_up.m_isDown    = false;  return true; }
            case ARROW_DOWN_CODE:  { input->m_down.m_isDown  = false;  return true; }
            case ARROW_LEFT_CODE:  { input->m_left.m_isDown  = false;  return true; }
            case ARROW_RIGHT_CODE: { input->m_right.m_isDown = false;  return true; }
            default: break;
        }
    }

    return false;
}