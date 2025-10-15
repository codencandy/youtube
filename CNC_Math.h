#ifndef CNC_MATH_H
#define CNC_MATH_H

#include "CNC_Types.h"

#define CNC_PI 3.141592653589793

f32 cnc_sqr( f32 x )
{
    f32 root = x / 3.0f;
    int i;
    if( x <= 0 ) 
        return 0;
    for( i=0; i<32; i++ )
    {
        root = (root + x / root) / 2;
    }
    return root;
}

f32 cnc_atan( f32 z) 
{
    // constants for approximation
    const double a = 0.280872;
    if (z > 1.0) {
        return CNC_PI/2 - cnc_atan(1.0 / z);
    } else if (z < -1.0) {
        return -CNC_PI/2 - cnc_atan(1.0 / z);
    } else {
        // polynomial approximation
        return (z / (1.0 + a * z * z));
    }
}

// atan2 built on top of our atan approximation
f32 cnc_atan2( f32 y, f32 x) {
    if( x > 0.0 ) 
    {
        return cnc_atan(y / x);
    } else if (x < 0.0 && y >= 0.0) 
    {
        return cnc_atan(y / x) + CNC_PI;
    } else if (x < 0.0 && y < 0.0) 
    {
        return cnc_atan(y / x) - CNC_PI;
    } else if (x == 0.0 && y > 0.0) 
    {
        return CNC_PI / 2.0;
    } else if (x == 0.0 && y < 0.0) 
    {
        return -CNC_PI / 2.0;
    } else 
    {
        return 0.0; // undefined for (0,0)
    }
}

f32 cnc_lineangle( v2 a, v2 b )
{
    f32 angle = cnc_atan2( (b.y - a.y), (b.x - a.x) );

    return angle;
}

f32 cnc_dot( v2& a, v2& b ) 
{
    f32 result = (a.x * b.x) + (a.y * b.y);
    return result;
}

f32 cnc_length( v2  a ) 
{
    f32 result = cnc_sqr( a.x * a.x + a.y * a.y ); 
    return result;
}

v2 cnc_normalize( v2 v ) 
{
    f32 len = cnc_length( v );
    return (len > 0.0f) ? vec2( v.x / len, v.y / len) : vec2( 0.0f, 0.0f );
}

v2 cnc_reflect( v2 a, v2 n) 
{
    v2 result;

    f32 dot = cnc_dot( a, n );
    v2  b   = n * (2.0f * dot);
    
    result = a - b;
    return  result;
}

m4 translationMatrix( f32 x, f32 y )
{
    v4 col1 = { 1.0f, 0.0f, 0.0f, 0.0f };
    v4 col2 = { 0.0f, 1.0f, 0.0f, 0.0f };
    v4 col3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    v4 col4 = {    x,    y, 0.0f, 1.0f };

    m4 result = {
        .columns {
            col1, col2, col3, col4
        }
    };

    return result;
}

m4 translationMatrix( v2 position )
{
    m4 result = translationMatrix( position.x, position.y );
    return result;
}

m4 identityMatrix()
{
    return translationMatrix( 0.0f, 0.0f );
}

f32 toRadians( f32 degrees )
{
    f32 radians = (CNC_PI / 180.0f) * degrees;
    return radians;
}

f32 distance( v2 a, v2 b )
{
    f32 d;

    f32 x = (b.x - a.x) * (b.x - a.x);
    f32 y = (b.y - a.y) * (b.y - a.y);

    d = cnc_sqr( x + y );

    return d;
}

static u32 xorshift_state = 2463534242u; /* default seed (nonzero) */

void cnc_srand( u32 seed )
{
    /* avoid zero seed because xorshift32 gets stuck at 0 */
    xorshift_state = (seed != 0u) ? seed : 2463534242u;
}

u32 cnc_rand()
{
    u32 x = xorshift_state;
    /* xorshift32 algorithm */
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    xorshift_state = x;
    return x;
}

bool cnc_v2_equal( v2 a, v2 b )
{
    if( a.x == b.x && a.y == b.y )
        return true;
    else
        return false;
}

bool cnc_vector_test()
{
    v2 a   = vec2(  1.0f,  0.0f );
    v2 b   = vec2(  0.0f,  1.0f );
    v2 c   = vec2( 10.0f, 10.0f );
    v2 d   = vec2( -1.0f,  0.0f );
    
    bool result = false;

    bool add     = cnc_v2_equal( a+b,    vec2(  1.0f,  1.0f ) );
    bool sub     = cnc_v2_equal( c-a,    vec2(  9.0f, 10.0f ) );
    bool scale   = cnc_v2_equal( c*2.0f, vec2( 20.0f, 20.0f ) );
    bool dot     = (cnc_dot( a, b ) == 0.0f );
    bool length  = (cnc_length( vec2( 3.0f, 0.0f ) ) == 3.0f );
    bool norm    = cnc_length( cnc_normalize( c ) ) == 1.0f;
    bool reflect = cnc_v2_equal( cnc_reflect( d, a), a ); 
        
    return (add && sub && scale && dot && norm && reflect);
}

#endif//CNC_MATH_H
