#ifndef CNC_TYPES_H
#define CNC_TYPES_H

typedef float                                        f32;
typedef double                                       f64;
typedef unsigned long long                           u64;
typedef unsigned int                                 u32;
typedef signed int                                   s32;

typedef __attribute__((__ext_vector_type__(2))) float v2;
typedef __attribute__((__ext_vector_type__(3))) float v3;
typedef __attribute__((__ext_vector_type__(4))) float v4;
typedef __attribute__((__ext_vector_type__(4))) u32  uv4;

typedef struct { v3 columns[3]; }                     m3;
typedef struct { v4 columns[4]; }                     m4;

enum shape_type
{
    CNC_IMAGE    = 0,
    CNC_PARTICLE = 1,
    CNC_RECT     = 2,
    CNC_CIRCLE   = 3,
    CNC_LINE     = 4
};

v2 vec2( f32 x, f32 y )
{
    v2 result = { x, y };
    return result;
}

v3 vec3( f32 x, f32 y, f32 z )
{
    v3 result = { x, y, z };
    return result;
}

v4 vec4( f32 x, f32 y, f32 z, f32 w )
{
    v4 result = { x, y, z, w };
    return result;
}

typedef struct VertexInput
{
    v3        m_position;
    v2        m_uv;
    
} VertexInput;

typedef struct UniformData
{
    m4        m_projection2D;
    f32       m_screenWidth;
    f32       m_screenHeight;
    f32       m_time;

} UniformData;


typedef struct ModelData
{
    m4        m_modelMatrix;
    m4        m_pivotMatrix;
    v4        m_rotation;

} ModelData;
typedef v4 Colour;

Colour colour( f32 r, f32 g, f32 b, f32 a )
{
    Colour result;
    result = vec4( r, g, b, a );
    return result;
}

typedef struct DrawCall
{
    shape_type m_type;
    u32        m_textureId;
    u32        m_snowMask;
    u32        m_skyMask;
    u32        m_numInstances;
    v2         m_position;
    v2         m_pivotPoint;
    v2         m_size;
    f32        m_angle;
    f32        m_radius;
    Colour     m_color;
    
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
    v2        m_position;
    f32       m_speed;
    f32       m_size;
    f32       m_time;
    
} Particle;

typedef struct Shape
{
    v2         m_position;
    v2         m_size;
    v4         m_color;
    f32        m_angle;

    v2         m_velocity;
    v2         m_acceleration;
    f32        m_mass;        // mass > 0. For "static" objects supply mass <= 0 to indicate infinite mass.
    bool       m_static;

    shape_type m_type;

} Shape;

typedef struct Rectangle : Shape
{ 
    v2         m_center;
    
} Rectangle;

typedef struct Circle : Shape
{
    v2         m_center;
    f32        m_radius;

} Circle;

typedef struct Line : Shape
{
    v2         m_start;
    v2         m_end;

} Line;

typedef struct Contact 
{
    bool       m_colliding;
    v2         m_normal;      // from A → B
    f32        m_penetration; // overlap distance
    
} Contact;


#endif//CNC_TYPES_H