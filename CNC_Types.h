#ifndef CNC_TYPES_H
#define CNC_TYPES_H

#include <simd/simd.h>

typedef float  f32;
typedef double f64;

typedef simd::float2   v2;
typedef simd::float3   v3;
typedef simd::float4   v4;
typedef simd::float3x3 m3;
typedef simd::float4x4 m4;

typedef struct VertexInput
{
    v3 m_position;
    v2 m_uv;
    
} VertexInput;

typedef struct UniformData
{
    m4 m_projection2D;

} UniformData;

#endif//CNC_TYPES_H