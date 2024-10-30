#ifndef CNC_RENDERER_H
#define CNC_RENDERER_H

#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

@interface MainRenderer : NSObject< MTKViewDelegate >
{
    @public
        id< MTLDevice >       m_gpu;
        id< MTLCommandQueue > m_commandQueue;
        MTKView*              m_view;
}
@end

void Render( MainRenderer* renderer );
MainRenderer* CreateMainRenderer();

#endif//CNC_RENDERER_H
