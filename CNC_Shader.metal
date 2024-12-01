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
};

struct ModelData
{
    float4x4 m_modelMatrix;
    float4x4 m_pivotMatrix;
    float    m_rotation;
};

constexpr sampler textureSampler( mag_filter::linear, min_filter::linear );

vertex VertexOutput MainVertexShader( VertexInput           in      [[stage_in]],
                                      constant UniformData& uniform [[buffer(1)]],
                                      constant ModelData&   model   [[buffer(2)]] )
{
    VertexOutput out;

    float angle = model.m_rotation;
    float2x2 rotationMatrix = {
        { cos(angle), -sin(angle) }, // important to define row major
        { sin(angle),  cos(angle) }  // important to define row major
    };

    float4 position = float4( in.m_position, 1.0 );

    position     = model.m_pivotMatrix    * position;
    position.xy  = rotationMatrix         * position.xy;
    position     = model.m_modelMatrix    * position;
    position     = uniform.m_projection2D * position;
    
    out.m_position  = position;
    out.m_uv        = in.m_uv;

    return out;
}

fragment float4 MainFragmentShader( VertexOutput in [[stage_in]],
                                    texture2d<float> image )
{
    float4 color = image.sample( textureSampler, in.m_uv );
    return color;
}