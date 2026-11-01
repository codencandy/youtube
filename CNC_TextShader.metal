#include <metal_stdlib>
using namespace metal;

struct VertexOutput
{
    float4 m_position [[position]];
    float2 m_uv;
};

struct UniformData
{
    float4x4 m_projection2D;
    float    m_screenWidth;
    float    m_screenHeight;
    float    m_time;
};

struct BoundingBox
{
    float2 m_position;
    float2 m_size;
};

vertex VertexOutput TextVertexShader( constant BoundingBox* box         [[buffer(0)]],
                                      constant UniformData& uniform     [[buffer(1)]],    
                                      uint                  instanceId  [[instance_id]],
                                      uint                  vertexId    [[vertex_id]])
{
    VertexOutput out;

    BoundingBox b = box[instanceId];

    // bottom left
    if( vertexId == 0 )
    {
        out.m_position = float4( b.m_position.x, b.m_position.y + b.m_size.y, 0.0, 1.0 );
        out.m_uv       = float2( 0.0, 0.0 );
    }

    // bottom right
    if( vertexId == 1 )
    {
        out.m_position = float4( b.m_position.x + b.m_size.x, b.m_position.y + b.m_size.y, 0.0, 1.0 );
        out.m_uv       = float2( 1.0, 0.0 );
    }

    // top left
    if( vertexId == 2 )
    {
        out.m_position = float4( b.m_position.x, b.m_position.y, 0.0, 1.0 );
        out.m_uv       = float2( 0.0, 1.0 );
    }

    // top right
    if( vertexId == 3 )
    {
        out.m_position = float4( b.m_position.x + b.m_size.x, b.m_position.y, 0.0, 1.0 );
        out.m_uv       = float2( 1.0, 1.0 );
    }

    out.m_position = uniform.m_projection2D * out.m_position;

    return out;
}              

fragment float4 TextFragmentShader( VertexOutput in [[stage_in]])
{
    float4 color = float4( 1.0, 1.0, 1.0, 0.2 );
    return color;
}