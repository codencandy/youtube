#include <metal_stdlib>
using namespace metal;

struct VertexInput
{
    float3 m_position [[attribute(0)]];
    float2 m_uv       [[attribute(1)]];
};

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

vertex VertexOutput TextVertexShader( device BoundingBox*   box         [[buffer(2)]],
                                      constant UniformData& uniform     [[buffer(1)]],    
                                      uint                  instanceId  [[instance_id]])
{
    VertexOutput out;
    return out;
}              

fragment float4 TextFragmentShader( VertexOutput in [[stage_in]])
{
    float4 color;
    return color;
}