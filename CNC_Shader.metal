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
    float4 m_color;
    float2 m_uv;
};

struct UniformData
{
    float4x4 m_projection2D;
    float    m_screenWidth;
    float    m_screenHeight;
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
    float  m_time;
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

vertex VertexOutput ParticleVertexShader( VertexInput           in         [[stage_in]],
                                          constant UniformData& uniform    [[buffer(1)]],
                                          device   Particle*    particles  [[buffer(2)]],
                                          texture2d<float>      snowMask   [[texture(0)]],
                                          texture2d<float>      skyMask    [[texture(1)]],
                                          uint                  instanceId [[instance_id]] )
{
    VertexOutput out;

    float4 position = float4( in.m_position, 1.0 );

    float x     = particles[instanceId].m_position.x;
    float y     = particles[instanceId].m_position.y;
    float speed = particles[instanceId].m_speed;
    float size  = particles[instanceId].m_size;

    float4x4 modelMatrix = float4x4(
        size,  0.0, 0.0, 0.0,
         0.0, size, 0.0, 0.0,
         0.0,  0.0, 1.0, 0.0,
           x,    y, 0.0, 1.0
    );

    position = modelMatrix * position;

    float2 vertexUV = float2( position.xy / float2( uniform.m_screenWidth, uniform.m_screenHeight ) );
    float  snowAlpha = snowMask.sample( textureSampler, vertexUV ).a;
    float  skyAlpha  = skyMask.sample( textureSampler,  vertexUV ).a;

    // snowflakes
    if( instanceId < 2900 )
    {
        out.m_color = float4( 1.0, 1.0, 1.0, 0.5 );

        float t  = particles[instanceId].m_time;
        float dt = uniform.m_time - t;
        
        if( dt > 5.0 && dt < t )
        {
            particles[instanceId].m_position.y = -40.0;
            particles[instanceId].m_speed      = 2.0 + sin( uniform.m_time + y) * 0.5;
            particles[instanceId].m_time       = 0.0;

            return out;
        }

        particles[instanceId].m_position.y += speed;

        if( snowAlpha != 0.0 && t == 0.0 )
        {
            particles[instanceId].m_speed = 0.0;
            particles[instanceId].m_time  = uniform.m_time;
        }
    }
    // stars
    else
    {
        out.m_color = float4( 1.0, 1.0, 0.0, 1.0 );
        out.m_color.a = 0.3 + sin( (uniform.m_time + y) * 3.0 ) * 0.7;
        

        if( skyAlpha != 0.0 )
        {
            out.m_color.a = 0.0;
        }
    }

    out.m_position = uniform.m_projection2D * position;
    out.m_uv       = in.m_uv;

    return out;
}

fragment float4 ParticleFragmentShader( VertexOutput in [[stage_in]] )
{
    float4 color = in.m_color;

    float2 center = float2( 0.5 );
    float  d      = distance( center, in.m_uv );
    float  edge   = 0.08;
    float  alpha  = smoothstep( 0.4, 0.4 - edge, d );

    color.a *= alpha;

    return color;
}