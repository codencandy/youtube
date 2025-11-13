#include <metal_stdlib>
using namespace metal;

enum draw_type
{
    CNC_IMAGE    = 0,
    CNC_PARTICLE = 1,
    CNC_RECT     = 2,
    CNC_CIRCLE   = 3,
    CNC_LINE     = 4
};

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
    uint   m_type;
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
    float4x4  m_modelMatrix;
    float4x4  m_pivotMatrix;
    float4    m_rotation;
};

struct Particle
{
    float2 m_position;
    float  m_speed;
    float  m_size;
    float  m_time;
};

struct RenderShape
{
    float2 m_position;
    float2 m_size;
    float4 m_color;
    float  m_angle;
    uint   m_type;
};

constexpr sampler textureSampler( mag_filter::linear, min_filter::linear );

vertex VertexOutput MainVertexShader( VertexInput           in         [[stage_in]],
                                      constant UniformData& uniform    [[buffer(1)]],
                                      constant ModelData&   model      [[buffer(2)]],
                                      uint                  instanceId [[instance_id]] )
{
    VertexOutput out;

    float angle = model.m_rotation.x * (instanceId + 1);

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

fragment float4 MainFragmentShader( VertexOutput          in    [[stage_in]],
                                    constant ModelData&   model [[buffer(1)]],
                                    texture2d<float> image )
{
    float4 color = image.sample( textureSampler, in.m_uv );
    return color;
}

vertex VertexOutput ShapeVertexShader( VertexInput           in         [[stage_in]],
                                       constant UniformData& uniform    [[buffer(1)]],
                                       device RenderShape*   shapes     [[buffer(2)]],
                                       uint                  instanceId [[instance_id]] )
{
    VertexOutput out;

    float sX    = shapes[instanceId].m_size.x;
    float sY    = shapes[instanceId].m_size.y;
    float posX  = shapes[instanceId].m_position.x;
    float posY  = shapes[instanceId].m_position.y;
    float4 pos  = float4( in.m_position, 1.0 );
    float angle = shapes[instanceId].m_angle;

    float4 s1 = float4( sX, 0.0,  0.0, 0.0 );
    float4 s2 = float4( 0.0, sY,  0.0, 0.0 );
    float4 s3 = float4( 0.0, 0.0, 1.0, 0.0 );
    float4 s4 = float4( 0.0, 0.0, 0.0, 1.0 );
    float4x4 scaleMatrix = float4x4( s1, s2, s3, s4 );

    float4 p1 = float4( 1.0, 0.0, 0.0, 0.0 );
    float4 p2 = float4( 0.0, 1.0, 0.0, 0.0 );
    float4 p3 = float4( 0.0, 0.0, 1.0, 0.0 );
    float4 p4 = float4( posX, posY, 0.0, 1.0 );
    float4x4 positionMatrix = float4x4( p1, p2, p3, p4 );

    float4 r1 = float4( cos( angle ), -sin( angle ), 0.0, 0.0 );
    float4 r2 = float4( sin( angle ),  cos( angle ), 0.0, 0.0 );
    float4 r3 = float4(          0.0,           0.0, 1.0, 0.0 );
    float4 r4 = float4(          0.0,           0.0, 0.0, 1.0 );
    float4x4 rotationMatrix = float4x4( r1, r2, r3, r4 );
        
    float4 position;
    position = scaleMatrix    * pos;
    position = rotationMatrix * position;
    position = positionMatrix * position;
    
    out.m_position = uniform.m_projection2D * position;
    out.m_uv       = in.m_uv;
    out.m_color    = shapes[instanceId].m_color;
    out.m_type     = shapes[instanceId].m_type;

    return out;
}

fragment float4 ShapeFragmentShader( VertexOutput in [[stage_in]] )
{
    float4 color;

    switch( in.m_type )
    {
        case CNC_RECT:   
        {
            color = in.m_color;
            break;
        }

        case CNC_CIRCLE: 
        {
            float2 center = float2( 0.5, 0.5 );
            float  d      = length( in.m_uv - center );
            color         = in.m_color;
            color.a       = 1.0 - smoothstep( 0.45, 0.5, d );
            break;
        }
        case CNC_LINE:   
        {
            color = in.m_color;
            color.a = smoothstep( 0.2, 0.5, in.m_uv.y ) * (1.0 - smoothstep( 0.5, 0.8, in.m_uv.y ));
            break;
        }
    }

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