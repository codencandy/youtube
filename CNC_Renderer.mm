#include "CNC_Types.h"
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>
#include "CNC_PlatformServices.h"

@interface MainRenderer : NSObject< MTKViewDelegate >
{
    @public
        id< MTLDevice >       m_gpu;
        id< MTLCommandQueue > m_commandQueue;
        MTKView*              m_view;

        id< MTLLibrary >      m_library;
        VertexInput           m_rectangle[6];
        UniformData           m_uniform;

        id< MTLBuffer >       m_rectangleBuffer;
        id< MTLBuffer >       m_uniformBuffer;

        id< MTLRenderPipelineState > m_renderState;

        id< MTLTexture >      m_clockBgr;
}

- (bool)checkError:(NSError*)error;
- (void)createShader;
- (void)createGeometry;
- (void)createUniform;
- (void)createPipeline;
- (void)loadBackground;

@end

@implementation MainRenderer

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{

}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    @autoreleasepool
    {
        MTLRenderPassDescriptor* renderDesc = [m_view currentRenderPassDescriptor];
        
        id< MTLCommandBuffer >        commandBuffer  = [m_commandQueue commandBuffer];
        id< MTLRenderCommandEncoder > commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor: renderDesc];

        [commandEncoder setRenderPipelineState: m_renderState];
        [commandEncoder setVertexBuffer: m_rectangleBuffer offset: 0 atIndex: 0];
        [commandEncoder setVertexBuffer: m_uniformBuffer   offset: 0 atIndex: 1];
        [commandEncoder setFragmentTexture: m_clockBgr atIndex: 0];
        [commandEncoder drawPrimitives: MTLPrimitiveTypeTriangle vertexStart: 0 vertexCount: 6];

        [commandEncoder endEncoding];
        [commandBuffer presentDrawable: [m_view currentDrawable]];
        [commandBuffer commit];
    }
}

- (bool)checkError:(NSError*)error
{
    if( error != NULL )
    {
        NSLog( @"%@", [error localizedDescription] );
        return false;
    }
    else
    {
        NSLog( @"no error" );
        return true;
    }
}

- (void)createShader
{
    NSError*  error        = NULL;
    NSString* shaderSource = [NSString stringWithContentsOfFile: @"CNC_Shader.metal" 
                                                       encoding: NSUTF8StringEncoding
                                                          error: &error];
    [self checkError: error];

    MTLCompileOptions* options = [MTLCompileOptions new];
    m_library = [m_gpu newLibraryWithSource: shaderSource
                                    options: options
                                      error: &error];

    [self checkError: error];                                      
}

- (void)createGeometry
{
    /*
        D --- C
        |     |
        A --- B
     */

    v3 A = {   0.0f, 600.0f, 0.0f };
    v3 B = { 600.0f, 600.0f, 0.0f };
    v3 C = { 600.0f,   0.0f, 0.0f };
    v3 D = {   0.0f,   0.0f, 0.0f };

    m_rectangle[0].m_position = A; m_rectangle[0].m_uv = vec2( 0.0f, 1.0f );
    m_rectangle[1].m_position = B; m_rectangle[1].m_uv = vec2( 1.0f, 1.0f );
    m_rectangle[2].m_position = C; m_rectangle[2].m_uv = vec2( 1.0f, 0.0f );

    m_rectangle[3].m_position = C; m_rectangle[3].m_uv = vec2( 1.0f, 0.0f );
    m_rectangle[4].m_position = D; m_rectangle[4].m_uv = vec2( 0.0f, 0.0f );
    m_rectangle[5].m_position = A; m_rectangle[5].m_uv = vec2( 0.0f, 1.0f );

    m_rectangleBuffer = [m_gpu newBufferWithBytes: &m_rectangle
                                           length: sizeof( VertexInput ) * 6
                                          options: MTLResourceCPUCacheModeDefaultCache];
}

- (void)createUniform
{
    f32 a =  2.0f/600.0f;
    f32 b = -2.0f/600.0f;
    f32 e = -1.0f;
    f32 f =  1.0f;

    v4 row1 = {    a, 0.0, 0.0,   e };
    v4 row2 = { 0.0f,   b, 0.0,   f };
    v4 row3 = { 0.0f, 0.0, 1.0, 0.0 };
    v4 row4 = { 0.0f, 0.0, 0.0, 1.0 };

    m_uniform.m_projection2D = simd_matrix_from_rows( row1, row2, row3, row4 );
    m_uniformBuffer = [m_gpu newBufferWithBytes: &m_uniform
                                         length: sizeof( UniformData)
                                        options: MTLResourceCPUCacheModeDefaultCache];
}

- (void)createPipeline
{
    MTLRenderPipelineDescriptor* renderDesc = [MTLRenderPipelineDescriptor new];
    MTLVertexDescriptor*         vertexDesc = [MTLVertexDescriptor new];

    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].format = MTLVertexFormatFloat2;

    vertexDesc.attributes[0].bufferIndex = 0;
    vertexDesc.attributes[1].bufferIndex = 0;

    vertexDesc.attributes[0].offset = offsetof( VertexInput, m_position );
    vertexDesc.attributes[1].offset = offsetof( VertexInput, m_uv );

    vertexDesc.layouts[0].stride       = sizeof( VertexInput );
    vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

    renderDesc.vertexDescriptor                = vertexDesc;
    renderDesc.vertexFunction                  = [m_library newFunctionWithName: @"MainVertexShader"];
    renderDesc.fragmentFunction                = [m_library newFunctionWithName: @"MainFragmentShader"];
    renderDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

    NSError* error = NULL;
    m_renderState  = [m_gpu newRenderPipelineStateWithDescriptor: renderDesc error: &error];
    [self checkError: error];
}

- (void)loadBackground
{
    Image* bgr = PlatformLoadImage( "res/clock_bgr.png" );

    MTLTextureDescriptor* textureDesc = [MTLTextureDescriptor new];
    textureDesc.width       = bgr->m_width;
    textureDesc.height      = bgr->m_height;
    textureDesc.pixelFormat = MTLPixelFormatBGRA8Unorm;

    m_clockBgr = [m_gpu newTextureWithDescriptor: textureDesc];

    MTLRegion region = MTLRegionMake2D( 0, 0, bgr->m_width, bgr->m_height );
    [m_clockBgr replaceRegion: region mipmapLevel: 0 withBytes: bgr->m_data bytesPerRow: bgr->m_width * 4];
}

@end

void Render( MainRenderer* renderer )
{
    [renderer->m_view draw];
}

MainRenderer* CreateMainRenderer()
{
    MainRenderer* renderer = [MainRenderer new];

    CGRect renderFrame = CGRectMake( 0, 0, 600, 600 );

    renderer->m_gpu = MTLCreateSystemDefaultDevice();
    renderer->m_commandQueue = [renderer->m_gpu newCommandQueue];
    renderer->m_view         = [[MTKView alloc] initWithFrame: renderFrame device: renderer->m_gpu];

    renderer->m_view.paused       = true;
    renderer->m_view.needsDisplay = false;
    renderer->m_view.delegate     = renderer;

    [renderer createShader];
    [renderer createGeometry];
    [renderer createUniform];
    [renderer createPipeline];
    [renderer loadBackground];
    
    return renderer;
}