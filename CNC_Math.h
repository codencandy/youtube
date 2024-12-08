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

#endif//CNC_MATH_H
