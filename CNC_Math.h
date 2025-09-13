#ifndef CNC_MATH_H
#define CNC_MATH_H

#include "CNC_Types.h"

#define CNC_PI 3.141592653589793

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
