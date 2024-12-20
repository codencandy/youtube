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
    float  m_edgeWidth;
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

vertex VertexOutput ParticleVertexShader( const    VertexInput  in         [[stage_in]],
                                          constant UniformData& uniform    [[buffer(1)]],
                                          device   Particle*    particles  [[buffer(2)]],
                                          texture2d<float>      snowMask   [[texture(0)]],
                                          texture2d<float>      skyMask    [[texture(1)]],
                                          uint                  instanceId [[instance_id]] )
{
    VertexOutput out;

    float4 position = float4( in.m_position, 1.0 );
    float  s        = particles[instanceId].m_size;
    float  speed    = particles[instanceId].m_speed;
    float2 pos      = particles[instanceId].m_position;
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

    float2 vertexUv = position.xy / float2( 1000.0, 500.0 );

    // snow
    if( instanceId < 2900 )
    {
        out.m_color     = float4( 1.0, 1.0, 1.0, 0.2 );
        out.m_edgeWidth = 0.08;

        float mask = snowMask.sample( textureSampler, vertexUv ).a;

        // update the particles
        particles[instanceId].m_position.y  = y + speed;

        float t = particles[instanceId].m_time;
        if( t != 0 )
        {
            float dt = uniform.m_time - t;
            if( dt > 10.0 )
            {
                particles[instanceId].m_speed      = 1.5 + sin(dt);
                particles[instanceId].m_time       = 0.0;
                particles[instanceId].m_position.y = -40.0;
            }
            out.m_color.a = 0.5;
        }
        else if( mask != 0 && position.y > s )
        {
            particles[instanceId].m_speed = 0.025;
            particles[instanceId].m_time  = uniform.m_time;
        }
    }
    // stars
    else
    {
        float mask = skyMask.sample( textureSampler, vertexUv ).a;

        out.m_color = float4( 235.0 / 255.0, 222.0 / 255.0, 10.0 / 255, 0.5 );
        out.m_color.a += sin( uniform.m_time * 5 + pos.y ) * 0.5;
        out.m_edgeWidth = 0.01 + abs(sin( uniform.m_time * 5 + pos.y )) * 0.07;

        if( mask != 0.0 )
        {
            out.m_color = float4( 0.0 );
        }
    }

    return out;
}

fragment float4 ParticleFragmentShader( VertexOutput in [[stage_in]] )
{
    const float gamma = 2.2;
    float4      color = in.m_color;

    // center in UV space
    float2 center    = float2(0.5, 0.5);  
    float  radius    = 0.3;               
    float  edgeWidth = 0.08;              

    // compute the distance from the fragment to the circle center.
    float dist = distance(in.m_uv, center);

    // smoothstep to create a smooth edge.
    float alpha = smoothstep(radius, radius - in.m_edgeWidth, dist);

    color.a   *= alpha;
    color.rgb  = pow( color.rgb, 1.0/gamma);

    return color;
}