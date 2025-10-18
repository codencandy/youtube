#include <AppKit/AppKit.h>
#include "CNC_Constants.h"

@interface MainWindowDelegate : NSObject< NSWindowDelegate >
{
    @public
        bool* m_running;
}

- (instancetype)initWithRunFlag:(bool*)running;

@end

@interface MainWindow : NSWindow
{
    @public 
        NSCondition*     m_displaySignal;
        CVDisplayLinkRef m_displayLink;
}
@end

@implementation MainWindowDelegate

- (instancetype)initWithRunFlag:(bool*)running
{
    self = [super init];

    m_running = running;

    return self;
}

- (bool)windowShouldClose:(NSWindow*)window
{
    *m_running = false;
    return true;
}

@end

@implementation MainWindow

- (bool)windowCanBecomeMain { return true; }
- (bool)windowCanBecomeKey  { return true; }

@end

CVReturn DisplayCallback( CVDisplayLinkRef    displayLink, 
                      const CVTimeStamp*  in, 
                      const CVTimeStamp*  out, 
                      unsigned long long  flagsIn, 
                      unsigned long long* flagsOut, 
                      void*               context)
{
    MainWindow* window = (MainWindow*)context;

    [window->m_displaySignal signal];

    return kCVReturnSuccess;
}

MainWindow* CreateMainWindow( bool* running )
{
    NSRect contentRecct = NSMakeRect( 0, 0, CNC_WINDOW_WIDTH, CNC_WINDOW_HEIGHT );
    MainWindow* window = [[MainWindow alloc] initWithContentRect: contentRecct
                                                       styleMask: NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                                                         backing: NSBackingStoreBuffered
                                                           defer: false];

    MainWindowDelegate* delegate = [[MainWindowDelegate alloc] initWithRunFlag:running];

    [window setTitle: @"codencandy"];
    [window setDelegate: delegate];
    [window makeKeyAndOrderFront: NULL];            

    window->m_displaySignal = [NSCondition new];          

    CVDisplayLinkCreateWithActiveCGDisplays( &window->m_displayLink );
    CVDisplayLinkSetOutputCallback( window->m_displayLink, DisplayCallback, (void*)window );
    CVDisplayLinkStart( window->m_displayLink );                                    

    return window;                                                          
}