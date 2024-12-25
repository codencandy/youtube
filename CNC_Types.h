#ifndef CNC_TYPES_H
#define CNC_TYPES_H

#include <simd/simd.h>

typedef float         f32;
typedef double        f64;
typedef unsigned int  u32;
typedef signed int    s32;

typedef simd::float2   v2;
typedef simd::float3   v3;
typedef simd::float4   v4;
typedef simd::float3x3 m3;
typedef simd::float4x4 m4;

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

} UniformData;

typedef struct ModelData
{
    m4  m_modelMatrix;
    m4  m_pivotMatrix;
    f32 m_rotation;

} ModelData;

enum draw_type
{
    CNC_IMAGE    = 0,
    CNC_PARTICLE = 1
};

typedef struct DrawCall
{
    draw_type m_type;
    u32       m_textureId;
    u32       m_snowMask;
    u32       m_skyMask;
    u32       m_numInstances;
    
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

typedef struct Particle
{
    v2  m_position;
    f32 m_speed;
    f32 m_size;
    
} Particle;

#endif//CNC_TYPES_H