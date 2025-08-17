#ifndef CNC_TYPES_H
#define CNC_TYPES_H

typedef float                                        f32;
typedef double                                       f64;
typedef unsigned int                                 u32;
typedef signed int                                   s32;

typedef __attribute__((__ext_vector_type__(2))) float v2;
typedef __attribute__((__ext_vector_type__(3))) float v3;
typedef __attribute__((__ext_vector_type__(4))) float v4;
typedef __attribute__((__ext_vector_type__(4))) u32  uv4;

typedef struct { v3 columns[3]; }                     m3;
typedef struct { v4 columns[4]; }                     m4;

v2 vec2( f32 x, f32 y )
{
    v2 result = { x, y };
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

enum draw_type
{
    CNC_IMAGE    = 0,
    CNC_PARTICLE = 1,
    CNC_RECT     = 2,
    CNC_CIRCLE   = 3,
    CNC_LINE     = 4
};

typedef struct ModelData
{
    m4        m_modelMatrix;
    m4        m_pivotMatrix;
    v4        m_rotation;
    uv4       m_data; // x -> shape

} ModelData;

typedef struct DrawCall
{
    draw_type m_type;
    u32       m_textureId;
    u32       m_snowMask;
    u32       m_skyMask;
    u32       m_numInstances;
    v2        m_position;
    v2        m_size;
    
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
    f32 m_time;
    
} Particle;

typedef struct Colour
{
    v4 m_color;

} Colour;

#endif//CNC_TYPES_H