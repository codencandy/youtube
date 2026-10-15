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

v2int halfwayPoint( v2int p1, v2int p2 )
{
    v2int halfway;

    halfway = vec2( (s16)(((s32)p1.x + p2.x) / 2),
                    (s16)(((s32)p1.y + p2.y) / 2) );

    return halfway;
}

f32 pow2( f32 x )
{
    return x*x;
}

v2 bezier( v2 p1, v2 p2, v2 p3, f32 t )
{
    v2 result;

    result.x = (pow2(1.0f - t)) * p1.x + (2.0f*(1.0f - t)*t*p2.x) + t*t*p3.x;
    result.y = (pow2(1.0f - t)) * p1.y + (2.0f*(1.0f - t)*t*p2.y) + t*t*p3.y;

    return result;
}

bool isNull( f32 value )
{
    static f32 epsilon = 0.000001f;
    return fabs( value ) <= epsilon ? true : false;
}

/*
    p1 - on curve point
    p2 - control point
    p3 - on curve point
    t1 - first intersection   [-1.0f for no intersection]
    t2 - second intersection  [-1.0f for no intersection]
    s  - scanline to test
 */
void bezierIntersection( v2 p1, v2 p2, v2 p3, f32* t1, f32* t2, f32 s )
{
    /*
        This function solves if a given scanline intersects with the quadratic 
        function at² + bt + c which is derived from the bezier curve given by p1, 
        p2 and p3 where p2 is the control point. More specifically it calculates
        where this function is 0. The two potential results are stored in t1 and t2.

        To move the curve for checking the scanline the parameter c is subtracted
        by scanline.

        The special case of a linear function where a is 0 only calculates one t value.

        The intersections are calculated against the quadratic function, the bounds
        check to ensure that t1 and t2 are inside the contour segment is done by
        the caller.
     */

    *t1 = -1.0f;
    *t2 = -1.0f;

    /*
        HOW TO BUILD THE BEZIER FUNCTION:

        The Bezier function interpolates from p1 to p2 and from
        p2 to p3. That means it interpolates between two linear interpolations.

        (1) Linear interpolation:           p1 + (p2 - p1)t
                                            p1 + p2t - p1t
                                            p1 - p1t + p2t
                                            (1 - t)p1 + p2t
        
        (2) Add two interpolations:         (1 - t)A + Bt          where A and B are interpolations
                                            (1-t)[(1-t)p1 + p2t] + t[(1-t)p2 + p3t]

        (3) Bezier function:                B(t) = (1-t)²p1 + 2(1-t)tp2 + t²p3

        (4) Binomial formula:               (1-t)² = 1 - 2t + t²

        (5) Substitute (4) in (3):           B(t) = (1-2t+t²)p₁ + (2-2t)tp₂ + t²p₃
                                                 = p₁ - 2p₁t + p₁t² + 2p₂t - 2p₂t² + p₃t²
                                                 = p₁ - 2p₁t + 2p₂t + p₁t² - 2p₂t² + p₃t²
                                                 = p₁ - 2p₁t + 2p₂t + (p₁ - 2p₂ + p₃)t²
                                                 = p₁ + 2p₂t - 2p₁t + (p₁ - 2p₂ + p₃)t²
                                                 = p₁ + 2(p₂ - p₁)t + (p₁ - 2p₂ + p₃)t²

        (6) Determine a, b and c:           B(t) = at² + bt + c                                                 
                                                 = (p₁ - 2p₂ + p₃)t² + 2(p₂ - p₁)t + p₁
                                                 
                                            a = (p₁ - 2p₂ + p₃)
                                            b = 2(p₂ - p₁)
                                            c = p₁
    */

    f32 a = p1.y - 2.0f*p2.y + p3.y;
    f32 b = 2.0f*(p2.y - p1.y);
    f32 c = p1.y;

    /* 
        move the bezier curve in position such that the scanline s equals 0 
     */
    c -= s;

    /*
        check if the bezier curve is a linear function which constitutes a 
        line with f(t) = b*t + c where a is 0 
        
            0 = bt + c
            t = -c/b

     */
    if( isNull( a ) )
    {
        if( !isNull( b ) )
        {
            *t1 = -c/b;
        }
        return;
    }

    /*
        use the quadratic formula to solve for 0
            t₁,t₂ = (-b ± √(b²-4ac)) / 2a

        NOTE: make sure that (b²-4ac) is not negative since this
              yields NaN when calculating the root
     */

    f32 discriminant = b*b - 4.0f*a*c;
    if( discriminant < 0 && !isNull( discriminant ) )
    {
        return;
    }

    if( isNull( discriminant ) )
    {
        *t1 = -b / (2.0f*a);
        return;
    }

    *t1 = (-b + sqrtf(discriminant)) / (2.0f*a);
    *t2 = (-b - sqrtf(discriminant)) / (2.0f*a);
}

#endif//CNC_MATH_H
