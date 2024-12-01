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
typedef simd::float2x2 m2;
typedef simd::float3x3 m3;
typedef simd::float4x4 m4;

v2 vec2( f32 x, f32 y )
{
    v2 result;

    result.x = x;
    result.y = y;

    return result;
}

m4 transformationMatrix( f32 x, f32 y )
{
    v4 row1 = { 1.0f, 0.0f, 0.0f,    x };
    v4 row2 = { 0.0f, 1.0f, 0.0f,    y };
    v4 row3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    v4 row4 = { 0.0f, 0.0f, 0.0f, 1.0f };

    return simd_matrix_from_rows( row1, row2, row3, row4 );
}

typedef struct VertexInput
{
    v3  m_position;
    v2  m_uv;
    
} VertexInput;

typedef struct ModelData
{
    m4  m_modelMatrix;
    m4  m_pivotMatrix;
    f32 m_rotation;

} ModelData;

typedef struct UniformData
{
    m4 m_projection2D;

} UniformData;

typedef struct Image
{
    void* m_data;
    s32   m_width;
    s32   m_height;
    s32   m_channels;
    u32   m_textureId;
    m4    m_modelMatrix;
    m4    m_pivotMatrix;
    f32   m_rotation;

} Image;

#endif//CNC_TYPES_H