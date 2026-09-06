#ifndef CNC_MATH_H
#define CNC_MATH_H

#include "CNC_Types.h"

#define CNC_PI 3.141592653589793

m4 translationMatrix( f32 x, f32 y )
{
    v4 row1 = { 1.0f, 0.0f, 0.0f,    x };
    v4 row2 = { 0.0f, 1.0f, 0.0f,    y };
    v4 row3 = { 0.0f, 0.0f, 1.0f, 0.0f };
    v4 row4 = { 0.0f, 0.0f, 0.0f, 1.0f };

    return simd_matrix_from_rows( row1, row2, row3, row4 );
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

f32 vectorLength( v2 v )
{
    return sqrt(v.x * v.x + v.y * v.y);
}

f32 distance( v2 a, v2 b )
{
    return vectorLength( vec2( b.x - a.x, b.y - a.y ) );
} 
f32 lineAngle( v2 p1, v2 p2 )
{
    f32 result = atan2f(p2.y - p1.y, p2.x - p1.x);
    return result;
}

#endif//CNC_MATH_H
