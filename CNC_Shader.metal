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
    float4 m_color;
};

struct UniformData
{
    float4x4 m_projection2D;
    float    m_time;
};

struct ModelData
{
    float4x4 m_modelMatrix;
    float4x4 m_pivotMatrix;
    float    m_rotation;
};

struct Particle
{
    float2 m_position;
    float  m_speed;
    float  m_size;
};

constexpr sampler textureSampler( mag_filter::linear, min_filter::linear );

vertex VertexOutput MainVertexShader( VertexInput           in         [[stage_in]],
                                      constant UniformData& uniform    [[buffer(1)]],
                                      constant ModelData&   model      [[buffer(2)]],
                                      uint                  instanceId [[instance_id]] )
{
    VertexOutput out;

    float angle = model.m_rotation * (instanceId + 1);

    float2x2 rotationMatrix = {
        { cos( angle ), -sin( angle )},
        { sin( angle ),  cos( angle )}
    };

    float4 position = model.m_pivotMatrix * float4( in.m_position, 1.0 );
    position.xy     = rotationMatrix * position.xy;

    out.m_position  = uniform.m_projection2D * model.m_modelMatrix * position;
    out.m_uv        = in.m_uv;

    return out;
}

fragment float4 MainFragmentShader( VertexOutput in [[stage_in]],
                                    texture2d<float> image )
{
    float4 color = image.sample( textureSampler, in.m_uv );
    return color;
}

vertex VertexOutput SnowVertexShader( const    VertexInput  in         [[stage_in]],
                                      constant UniformData& uniform    [[buffer(1)]],
                                      device   Particle*    snowflakes [[buffer(2)]],
                                      uint                  instanceId [[instance_id]] )
{
    VertexOutput out;

    float4 position = float4( in.m_position, 1.0 );
    float  s        = snowflakes[instanceId].m_size;
    float  speed    = snowflakes[instanceId].m_speed;
    float2 pos      = snowflakes[instanceId].m_position;
    float  x        = pos.x;
    float  y        = pos.y;

    float4x4 modelMatrix = float4x4( 
          s, 0.0, 0.0, 0.0,
        0.0,   s, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
          x,   y, 0.0, 1.0
    );

    position = modelMatrix * position;

    out.m_position = uniform.m_projection2D * position;
    out.m_uv       = in.m_uv;

    if( instanceId < 900 )
    {
        // update the particles
        snowflakes[instanceId].m_position.y  = y + speed;
        if( y > 510 )
        {
            snowflakes[instanceId].m_position.y = -40;
        }
        out.m_color = float4( 1.0, 1.0, 1.0, 0.3 );
    }
    else
    {
        out.m_color = float4( 235.0 / 255.0, 222.0 / 255.0, 10.0 / 255, 0.5 );
        out.m_color.a += sin( uniform.m_time * 5 + pos.y ) * 0.5;
    }

    return out;
}

fragment float4 SnowFragmentShader( VertexOutput     in   [[stage_in]],
                                    texture2d<float> mask )
{
    const float gamma = 2.2;
    float4      color = in.m_color;

    float2 center    = float2(0.5, 0.5);  // Circle center in UV space
    float  radius    = 0.4;               // Circle radius
    float  edgeWidth = 0.01;              // Smooth edge width

    // Compute the distance from the fragment to the circle center.
    float dist = distance(in.m_uv, center);

    // Use smoothstep to create a smooth edge.
    float alpha = smoothstep(radius, radius - edgeWidth, dist);

    color.a   *= alpha;
    color.rgb  = pow( color.rgb, 1.0/gamma);

    if( color.r != 1.0 )
    {
        float2 inPos = in.m_position.xy;
        color.a -=  mask.sample( textureSampler, inPos / float2( 2000.0, 1000.0 ) ).a;
    }

    return color;
}