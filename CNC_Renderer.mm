#ifdef UNITY_BUILD
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
#else
#include "CNC_Renderer.h"
#endif

@implementation MainRenderer

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{

}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    @autoreleasepool
    {
        static float red = 0.0f;
        red += 0.05f;

        MTLRenderPassDescriptor* renderDesc = [m_view currentRenderPassDescriptor];
        renderDesc.colorAttachments[0].clearColor = MTLClearColorMake( fabs(sin(red)), 0.0, 0.0, 1.0 );
        id< MTLCommandBuffer >        commandBuffer  = [m_commandQueue commandBuffer];
        id< MTLRenderCommandEncoder > commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor: renderDesc];

        [commandEncoder endEncoding];
        [commandBuffer presentDrawable: [m_view currentDrawable]];
        [commandBuffer commit];
    }
}

@end

void Render( MainRenderer* renderer )
{
    [renderer->m_view draw];
}

MainRenderer* CreateMainRenderer()
{
    MainRenderer* renderer = [MainRenderer new];

    CGRect renderFrame = CGRectMake( 0, 0, 500, 500 );

    renderer->m_gpu = MTLCreateSystemDefaultDevice();
    renderer->m_commandQueue = [renderer->m_gpu newCommandQueue];
    renderer->m_view         = [[MTKView alloc] initWithFrame: renderFrame device: renderer->m_gpu];

    renderer->m_view.paused       = true;
    renderer->m_view.needsDisplay = false;
    renderer->m_view.delegate     = renderer;
    
    return renderer;
}