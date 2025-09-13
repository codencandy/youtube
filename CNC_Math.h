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

#endif//CNC_MATH_H
