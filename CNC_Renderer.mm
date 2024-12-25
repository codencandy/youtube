#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

#include "CNC_Constants.h"
#include "CNC_Types.h"
#include "CNC_PlatformServices.h"

@interface MainRenderer : NSObject< MTKViewDelegate >
{
    @public
        id< MTLDevice >       m_gpu;
        id< MTLCommandQueue > m_commandQueue;
        MTKView*              m_view;

        id< MTLLibrary >      m_library;
        UniformData           m_uniform;
        id< MTLBuffer >       m_uniformBuffer;

        id< MTLRenderPipelineState > m_renderStateImage;
        id< MTLRenderPipelineState > m_renderStateParticle;

        u32                   m_nextTextureId;
        NSMutableArray*       m_textures;
        NSMutableArray*       m_vertexBuffers;
        NSMutableArray*       m_modelBuffers;

        u32                   m_numDrawCalls;
        DrawCall              m_drawCalls[10];

        id< MTLBuffer >       m_particleBuffer;
        VertexInput           m_particleVertices[6];
}

- (bool)checkError:(NSError*)error;
- (void)createShader;
- (id< MTLBuffer >)createGeometry:(u32)width height:(u32)height;
- (void)createUniform;
- (void)createPipeline;

- (u32)uploadImage:(Image*)image;
- (void)uploadParticles:(Particle*)particles numParticles:(u32)numParticles;
- (void)renderImage:(u32)textureId instances:(u32)numInstances;
- (void)renderParticles:(u32)numParticles snowMask:(u32)snowMask skyMask:(u32)skyMask;
- (void)updateImage:(Image*)image;

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

        [commandEncoder setVertexBuffer: m_uniformBuffer   offset: 0 atIndex: 1];

        for( u32 i=0; i<m_numDrawCalls; ++i )
        {
            DrawCall call = m_drawCalls[i];

            switch( call.m_type )
            {
                case CNC_IMAGE:
                {
                    [commandEncoder setRenderPipelineState: m_renderStateImage];
                    id< MTLBuffer >  vertexBuffer = m_vertexBuffers[call.m_textureId];
                    id< MTLTexture > texture      = m_textures[call.m_textureId];
                    id< MTLBuffer >  modelBuffer  = m_modelBuffers[call.m_textureId];

                    [commandEncoder setVertexBuffer: vertexBuffer offset: 0 atIndex: 0];
                    [commandEncoder setVertexBuffer: modelBuffer  offset: 0 atIndex: 2];
                    [commandEncoder setFragmentTexture: texture atIndex: 0];
                    [commandEncoder drawPrimitives: MTLPrimitiveTypeTriangle vertexStart: 0 vertexCount: 6 instanceCount: call.m_numInstances];
                    break;
                }

                case CNC_PARTICLE:
                {
                    [commandEncoder setRenderPipelineState: m_renderStateParticle];

                    id< MTLTexture > snowMask = m_textures[call.m_snowMask];
                    id< MTLTexture > skyMask  = m_textures[call.m_skyMask];

                    [commandEncoder setVertexBytes: &m_particleVertices length: sizeof( VertexInput ) * 6 atIndex: 0];
                    [commandEncoder setVertexBuffer: m_particleBuffer  offset: 0 atIndex: 2];
                    [commandEncoder setVertexTexture: snowMask atIndex: 0];
                    [commandEncoder setVertexTexture: skyMask  atIndex: 1];
                    [commandEncoder drawPrimitives: MTLPrimitiveTypeTriangle vertexStart: 0 vertexCount: 6 instanceCount: call.m_numInstances];
                    break;
                }
            }
            
        }

        [commandEncoder endEncoding];
        [commandBuffer presentDrawable: [m_view currentDrawable]];
        [commandBuffer commit];
    }

    // reset this after every frame !!!
    m_numDrawCalls = 0;
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

- (id< MTLBuffer >)createGeometry:(u32)width height:(u32)height
{
    VertexInput rectangle[6];
    /*
        D --- C
        |     |
        A --- B
     */

    v3 A = {   0.0f, height, 0.0f };
    v3 B = {  width, height, 0.0f };
    v3 C = {  width,   0.0f, 0.0f };
    v3 D = {   0.0f,   0.0f, 0.0f };

    rectangle[0].m_position = A; rectangle[0].m_uv = vec2( 0.0f, 1.0f );
    rectangle[1].m_position = B; rectangle[1].m_uv = vec2( 1.0f, 1.0f );
    rectangle[2].m_position = C; rectangle[2].m_uv = vec2( 1.0f, 0.0f );

    rectangle[3].m_position = C; rectangle[3].m_uv = vec2( 1.0f, 0.0f );
    rectangle[4].m_position = D; rectangle[4].m_uv = vec2( 0.0f, 0.0f );
    rectangle[5].m_position = A; rectangle[5].m_uv = vec2( 0.0f, 1.0f );

    id< MTLBuffer > rectBuffer = [m_gpu newBufferWithBytes: &rectangle
                                                    length: sizeof( VertexInput ) * 6
                                                   options: MTLResourceCPUCacheModeDefaultCache];
                                                
    return rectBuffer;                                                
}

- (void)createUniform
{
    f32 a =  2.0f/CNC_WINDOW_WIDTH;
    f32 b = -2.0f/CNC_WINDOW_HEIGHT;
    f32 e = -1.0f;
    f32 f =  1.0f;

    v4 row1 = {    a, 0.0, 0.0,   e };
    v4 row2 = { 0.0f,   b, 0.0,   f };
    v4 row3 = { 0.0f, 0.0, 1.0, 0.0 };
    v4 row4 = { 0.0f, 0.0, 0.0, 1.0 };

    m_uniform.m_projection2D = simd_matrix_from_rows( row1, row2, row3, row4 );
    m_uniform.m_screenWidth  = CNC_WINDOW_WIDTH;
    m_uniform.m_screenHeight = CNC_WINDOW_HEIGHT;
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

    renderDesc.colorAttachments[0].blendingEnabled = true;
    renderDesc.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    renderDesc.colorAttachments[0].rgbBlendOperation   = MTLBlendOperationAdd;

    renderDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
    renderDesc.colorAttachments[0].sourceRGBBlendFactor   = MTLBlendFactorSourceAlpha;

    renderDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    renderDesc.colorAttachments[0].destinationRGBBlendFactor   = MTLBlendFactorOneMinusSourceAlpha;

    NSError* error = NULL;
    m_renderStateImage = [m_gpu newRenderPipelineStateWithDescriptor: renderDesc error: &error];
    [self checkError: error];

    renderDesc.vertexFunction   = [m_library newFunctionWithName: @"ParticleVertexShader"];
    renderDesc.fragmentFunction = [m_library newFunctionWithName: @"ParticleFragmentShader"];

    error = NULL;
    m_renderStateParticle = [m_gpu newRenderPipelineStateWithDescriptor: renderDesc error: &error];
    [self checkError: error];
}

- (u32)uploadImage:(Image*)image
{
    u32 textureId = m_nextTextureId;

    MTLTextureDescriptor* textureDesc = [MTLTextureDescriptor new];
    textureDesc.width       = image->m_width;
    textureDesc.height      = image->m_height;
    textureDesc.pixelFormat = MTLPixelFormatRGBA8Unorm;

    id< MTLTexture > texture      = [m_gpu newTextureWithDescriptor: textureDesc];
    id< MTLBuffer >  vertexBuffer = [self createGeometry: image->m_width height: image->m_height];
    id< MTLBuffer >  modelBuffer  = [m_gpu newBufferWithBytes: &image->m_modelData length: sizeof( ModelData ) options: MTLResourceCPUCacheModeDefaultCache];

    MTLRegion region = MTLRegionMake2D( 0, 0, image->m_width, image->m_height );
    [texture replaceRegion: region mipmapLevel: 0 withBytes: image->m_data bytesPerRow: image->m_width * 4];
    [m_textures      insertObject: texture      atIndex: textureId];
    [m_vertexBuffers insertObject: vertexBuffer atIndex: textureId];
    [m_modelBuffers  insertObject: modelBuffer  atIndex: textureId];

    m_nextTextureId++;

    return textureId;
}

- (void)uploadParticles:(Particle*)particles numParticles:(u32)numParticles
{
    m_particleBuffer = [m_gpu newBufferWithBytes: particles 
                                          length: sizeof( Particle ) * numParticles 
                                         options: MTLStorageModeShared];

    f32 width  = 1.0f;
    f32 height = 1.0f;
    
    v3 A = {   0.0f, height, 0.0f };
    v3 B = {  width, height, 0.0f };
    v3 C = {  width,   0.0f, 0.0f };
    v3 D = {   0.0f,   0.0f, 0.0f };

    m_particleVertices[0].m_position = A; m_particleVertices[0].m_uv = vec2( 0.0f, 1.0f );
    m_particleVertices[1].m_position = B; m_particleVertices[1].m_uv = vec2( 1.0f, 1.0f );
    m_particleVertices[2].m_position = C; m_particleVertices[2].m_uv = vec2( 1.0f, 0.0f );

    m_particleVertices[3].m_position = C; m_particleVertices[3].m_uv = vec2( 1.0f, 0.0f );
    m_particleVertices[4].m_position = D; m_particleVertices[4].m_uv = vec2( 0.0f, 0.0f );
    m_particleVertices[5].m_position = A; m_particleVertices[5].m_uv = vec2( 0.0f, 1.0f );                                         
}

- (void)renderImage:(u32)textureId instances:(u32)numInstances
{
    m_drawCalls[m_numDrawCalls].m_type         = CNC_IMAGE;
    m_drawCalls[m_numDrawCalls].m_textureId    = textureId;
    m_drawCalls[m_numDrawCalls].m_numInstances = numInstances;
    
    m_numDrawCalls++;
}

- (void)renderParticles:(u32)numParticles snowMask:(u32)snowMask skyMask:(u32)skyMask
{
    m_drawCalls[m_numDrawCalls].m_type         = CNC_PARTICLE;
    m_drawCalls[m_numDrawCalls].m_textureId    = 0;
    m_drawCalls[m_numDrawCalls].m_snowMask     = snowMask;
    m_drawCalls[m_numDrawCalls].m_skyMask      = skyMask;
    m_drawCalls[m_numDrawCalls].m_numInstances = numParticles;
    
    m_numDrawCalls++;
}

- (void)updateImage:(Image*)image
{
    id< MTLBuffer > modelBuffer = m_modelBuffers[image->m_textureId];
    memcpy( [modelBuffer contents], &image->m_modelData, sizeof( ModelData ) );
}

@end

void Render( MainRenderer* renderer )
{
    [renderer->m_view draw];
}

u32 PlatformUploadImage( void* renderer, Image* image )
{
    MainRenderer* r = (MainRenderer*)renderer;
    return [r uploadImage: image];
}

void PlatformUploadParticles( void* renderer, Particle* particles, u32 numParticles )
{
    MainRenderer* r = (MainRenderer*)renderer;
    [r uploadParticles: particles numParticles: numParticles];
}

void PlatformRenderImage( void* renderer, u32 textureId, u32 numInstances )
{
    MainRenderer* r = (MainRenderer*)renderer;
    [r renderImage: textureId instances: numInstances];
}

void PlatformRenderParticles( void* renderer, u32 numParticles, u32 snowMask, u32 skyMask )
{
    MainRenderer* r = (MainRenderer*)renderer;
    [r renderParticles: numParticles snowMask:snowMask skyMask:skyMask];
}

void PlatformUpdateImage( void* renderer, Image* image )
{
    MainRenderer* r = (MainRenderer*)renderer;
    [r updateImage: image];
}

MainRenderer* CreateMainRenderer()
{
    MainRenderer* renderer = [MainRenderer new];

    CGRect renderFrame = CGRectMake( 0, 0, CNC_WINDOW_WIDTH, CNC_WINDOW_HEIGHT );

    renderer->m_gpu = MTLCreateSystemDefaultDevice();
    renderer->m_commandQueue = [renderer->m_gpu newCommandQueue];
    renderer->m_view         = [[MTKView alloc] initWithFrame: renderFrame device: renderer->m_gpu];

    renderer->m_view.paused       = true;
    renderer->m_view.needsDisplay = false;
    renderer->m_view.delegate     = renderer;

    renderer->m_nextTextureId     = 0;
    renderer->m_numDrawCalls      = 0;
    renderer->m_textures          = [[NSMutableArray alloc] initWithCapacity: 10];
    renderer->m_vertexBuffers     = [[NSMutableArray alloc] initWithCapacity: 10];
    renderer->m_modelBuffers      = [[NSMutableArray alloc] initWithCapacity: 10];

    [renderer createShader];
    [renderer createUniform];
    [renderer createPipeline];
    
    return renderer;
}