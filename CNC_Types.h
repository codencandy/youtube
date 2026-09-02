#ifndef CNC_TYPES_H
#define CNC_TYPES_H

#include <simd/simd.h>

typedef float          f32;
typedef double         f64;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char   u8;
typedef signed int     s32;

typedef simd::float2   v2;
typedef simd::float3   v3;
typedef simd::float4   v4;
typedef simd::float3x3 m3;
typedef simd::float4x4 m4;
typedef       v4       color;

enum ENDIANESS
{
    BIGENDIAN    = 0,
    LITTLEENDIAN = 1
};

color rgba( f32 r, f32 g, f32 b, f32 a )
{
    color c;

    c.x = r;
    c.y = g;
    c.z = b;
    c.w = a;

    return c;
}

v2 vec2( f32 x, f32 y )
{
    v2 result;

    result.x = x;
    result.y = y;

    return result;
}

typedef struct VertexInput
{
    v3 m_position;
    v2 m_uv;
    
} VertexInput;

typedef struct UniformData
{
    m4  m_projection2D;
    f32 m_screenWidth;
    f32 m_screenHeight;
    f32 m_time;

} UniformData;

typedef struct ModelData
{
    m4  m_modelMatrix;
    m4  m_pivotMatrix;
    f32 m_rotation;

} ModelData;

typedef struct PrimitiveData
{
    v2    m_pos; 
    v2    m_size;
    color m_color;

} PrimitiveData;

enum draw_type
{
    CNC_IMAGE    = 0,
    CNC_PARTICLE = 1,
    CNC_RECT     = 2,
    CNC_CIRCLE   = 3,
    CNC_LINE     = 4
};

typedef struct DrawCall
{
    draw_type m_type;
    u32       m_textureId;
    u32       m_snowMask;
    u32       m_skyMask;
    u32       m_numInstances;
    v2        m_position;
    v2        m_size;
    color     m_color;
    
} DrawCall;

typedef struct Image
{
    void*     m_data;
    s32       m_width;
    s32       m_height;
    s32       m_channels;
    u32       m_textureId;
    ModelData m_modelData;

} Image;

typedef struct File
{
    void*     m_data;
    u32       m_sizeInBytes;
    ENDIANESS m_endianess;

} File;

typedef struct Particle
{
    v2  m_position;
    f32 m_speed;
    f32 m_size;
    f32 m_time;
    
} Particle;

#endif//CNC_TYPES_H