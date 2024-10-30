#ifndef CNC_WINDOW_H
#define CNC_WINDOW_H

#include <AppKit/AppKit.h>

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

MainWindow* CreateMainWindow( bool* running );

#endif//CNC_WINDOW_H
