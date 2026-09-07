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

    return window;                                                          
}