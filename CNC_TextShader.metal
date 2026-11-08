#include <metal_stdlib>
using namespace metal;

typedef float2 v2;
typedef float  f32;
typedef uint   u32;

struct VertexOutput
{
    float4 m_position [[position]];
    v2     m_uv;
    u32    m_glyphId;
};

struct UniformData
{
    float4x4 m_projection2D;
    f32      m_screenWidth;
    f32      m_screenHeight;
    f32      m_time;
};

struct BoundingBox
{
    v2  m_position;
    v2  m_size;
    v2  m_glyphOrigin;
    u32 m_glyphId;
    f32 m_fontScale;
};

struct GpuGlyph
{
    u32 m_contourIndex; // Offset of this glyph's first contour endpoint.
    u32 m_numContours;
    u32 m_pointIndex;
    u32 m_numPoints;
};

bool isNull( f32 value, f32 scale )
{
    f32 absoluteEpsilon = 1e-12;
    f32 relativeEpsilon = 1e-6; 
    return abs( value ) <= absoluteEpsilon + relativeEpsilon * scale;
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

/*
    p1 - on curve point
    p2 - control point
    p3 - on curve point
    t1 - first intersection   [-1.0f for no intersection]
    t2 - second intersection  [-1.0f for no intersection]
    s  - scanline to test
    returns true if the scanline and the bezier path exactly overlap
 */
bool bezierIntersection( v2 p1, v2 p2, v2 p3, thread f32* t1, thread f32* t2, f32 s )
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

    /* Use f64 for the coefficient and root calculations to reduce cancellation errors. */
    f32 y1 = (f32)p1.y;
    f32 y2 = (f32)p2.y;
    f32 y3 = (f32)p3.y;

    if( y1 == s && y2 == s && y3 == s )
    {
        return true;
    }

    f32 a = y1 - 2.0*y2 + y3;
    f32 b = 2.0*(y2 - y1);
    f32 c = y1 - (f32)s;


    f32 aScale = abs( y1 ) + 2.0*abs( y2 ) + abs( y3 );
    f32 bScale = 2.0*(abs( y2 ) + abs( y1 ));

    /*
        check if the bezier curve is a linear function which constitutes a 
        line with f(t) = b*t + c where a is 0 
        
            0 = bt + c
            t = -c/b

     */
    if( isNull( a, aScale ) )
    {
        if( !isNull( b, bScale ) )
        {
            *t1 = (f32)(-c/b);
        }
        return false;
    }

    /*
        use the quadratic formula to solve for 0
            t₁,t₂ = (-b ± √(b²-4ac)) / 2a

        NOTE: make sure that (b²-4ac) is not negative since this
              yields NaN when calculating the root
     */

    f32 bSquared = b*b;
    f32 fourAC = 4.0*a*c;
    f32 discriminant = bSquared - fourAC;
    f32 discriminantScale = abs( bSquared ) + abs( fourAC );

    if( discriminant < 0.0 && !isNull( discriminant, discriminantScale ) )
    {
        return false;
    }

    if( isNull( discriminant, discriminantScale ) )
    {
        *t1 = (f32)(-b / (2.0*a));
        return false;
    }

    f32 discriminantRoot = sqrt( discriminant );
    *t1 = (f32)((-b + discriminantRoot) / (2.0*a));
    *t2 = (f32)((-b - discriminantRoot) / (2.0*a));

    return false;
}

uint3 CalculateIntersections( v2 p1,
                              v2 p2,
                              v2 p3,
                              float3 rayX,
                              f32 rayY,
                              thread bool3* onPath )
{
    f32 t1               = -1.0f;
    f32 t2               = -1.0f;
    uint3 numIntersections = uint3( 0 );
    f32 epsilon          = 0.01f;

    if( !bezierIntersection( p1, p2, p3, &t1, &t2, rayY ) )
    {
        if( t1 >= 0 && t1 <= 1.0f )
        {
            v2 p = bezier( p1, p2, p3, t1 );

            if( abs( p.x - rayX.x ) <= epsilon ) (*onPath).x = true;
            if( abs( p.x - rayX.y ) <= epsilon ) (*onPath).y = true;
            if( abs( p.x - rayX.z ) <= epsilon ) (*onPath).z = true;

            // This check is valid for a piece that does not turn in Y.
            f32 minY = min( p1.y, p3.y );
            f32 maxY = max( p1.y, p3.y );

            if( rayY >= minY && rayY < maxY )
            {
                if( p.x > rayX.x ) numIntersections.x++;
                if( p.x > rayX.y ) numIntersections.y++;
                if( p.x > rayX.z ) numIntersections.z++;
            }
        }
    
        if( t2 >= 0 && t2 <= 1.0f )
        {
            v2 p = bezier( p1, p2, p3, t2 );

            if( abs( p.x - rayX.x ) <= epsilon ) (*onPath).x = true;
            if( abs( p.x - rayX.y ) <= epsilon ) (*onPath).y = true;
            if( abs( p.x - rayX.z ) <= epsilon ) (*onPath).z = true;

            // this checks if the ray is between p1 and p3 
            // or between p3 and p1 which is both possible
            f32 minY = min( p1.y, p3.y );
            f32 maxY = max( p1.y, p3.y );

            if( rayY >= minY && rayY < maxY )
            {
                if( p.x > rayX.x ) numIntersections.x++;
                if( p.x > rayX.y ) numIntersections.y++;
                if( p.x > rayX.z ) numIntersections.z++;
            }
        }
    }
    else
    {
        // in this case the ray and the bezier path overlap
        // that means that p1 and p3 are the intersection points
        // now we check if the starting point of the ray is on the path
        f32 left  = min( p1.x, p3.x );
        f32 right = max( p1.x, p3.x );

        if( rayX.x >= left && rayX.x <= right ) (*onPath).x = true;
        if( rayX.y >= left && rayX.y <= right ) (*onPath).y = true;
        if( rayX.z >= left && rayX.z <= right ) (*onPath).z = true;
    }

    return numIntersections;
}

vertex VertexOutput TextVertexShader( constant BoundingBox* box         [[buffer(0)]],
                                      constant UniformData& uniform     [[buffer(1)]],    
                                      uint                  instanceId  [[instance_id]],
                                      uint                  vertexId    [[vertex_id]])
{
    VertexOutput out;

    BoundingBox b = box[instanceId];

    switch( vertexId )
    {
        case 0:
        {
            // bottom left
            out.m_position = float4( b.m_position.x, b.m_position.y + b.m_size.y, 0.0, 1.0 );
            out.m_uv       = v2( 0.0, 0.0 ) * b.m_size;
            break;
        }
        case 1:
        {
            // bottom right
            out.m_position = float4( b.m_position.x + b.m_size.x, b.m_position.y + b.m_size.y, 0.0, 1.0 );
            out.m_uv       = v2( 1.0, 0.0 ) * b.m_size;
            break;
        }
        case 2:
        {
            // top left
            out.m_position = float4( b.m_position.x, b.m_position.y, 0.0, 1.0 );
            out.m_uv       = v2( 0.0, 1.0 ) * b.m_size;
            break;
        }
        case 3:
        {
            // top right
            out.m_position = float4( b.m_position.x + b.m_size.x, b.m_position.y, 0.0, 1.0 );
            out.m_uv       = v2( 1.0, 1.0 ) * b.m_size;
            break;
        }
    }

    // Convert bottom-up pixel UVs to font units with Y pointing down,
    // matching the points produced by InsertImpliedPoints.
    out.m_uv = b.m_glyphOrigin
             + v2( out.m_uv.x, b.m_size.y - out.m_uv.y ) / b.m_fontScale;

    out.m_position = uniform.m_projection2D * out.m_position;
    out.m_glyphId  = b.m_glyphId;

    return out;
}              

f32 GlyphCoverage( GpuGlyph          g,
                   float3            rayX,
                   f32               rayY,
                   constant v2*      pointsBuffer,
                   constant u32*     contourBuffer )
{
    v2 p1, p2, p3;

    uint3 numIntersections = uint3( 0 );
    u32  startPoint       = g.m_pointIndex;
    bool3 onPath          = bool3( false );

    for( u32 c=0; c<g.m_numContours; ++c )
    {
        u32 endPoint = g.m_pointIndex + contourBuffer[g.m_contourIndex + c];

        // Assumes each contour starts on-curve, followed by a control point.
        // Adjacent quadratic segments share their on-curve endpoint.
        for( u32 point=startPoint; point+1<=endPoint; point+=2 )
        {
            p1 = pointsBuffer[point];
            p2 = pointsBuffer[point + 1];

            // The final control point connects back to this contour's start.
            u32 nextPoint = point + 2 <= endPoint ? point + 2 : startPoint;
            p3 = pointsBuffer[nextPoint];

            numIntersections += CalculateIntersections( p1, p2, p3, rayX, rayY, &onPath );
        }

        startPoint = endPoint + 1;
    }

    bool inside0 = onPath.x || (numIntersections.x % 2) != 0;
    bool inside1 = onPath.y || (numIntersections.y % 2) != 0;
    bool inside2 = onPath.z || (numIntersections.z % 2) != 0;

    f32 coverage = 0.0f;
    coverage += inside0 ? 1.0f / 3.0f : 0.0f;
    coverage += inside1 ? 1.0f / 3.0f : 0.0f;
    coverage += inside2 ? 1.0f / 3.0f : 0.0f;
    return coverage;
}

fragment float4 TextFragmentShader( VertexOutput       in            [[stage_in]],
                                    constant GpuGlyph* glphyBuffer   [[buffer(0)]],
                                    constant v2*       pointsBuffer  [[buffer(1)]],
                                    constant u32*      contourBuffer [[buffer(2)]])
{
    GpuGlyph g = glphyBuffer[in.m_glyphId];
    f32 pixelWidth = dfdx( in.m_uv.x );
    float3 rayX = in.m_uv.x + pixelWidth * float3(
        -1.0f / 3.0f,
         0.0f,
         1.0f / 3.0f
    );

    f32 coverage = GlyphCoverage(
        g,
        rayX,
        in.m_uv.y,
        pointsBuffer,
        contourBuffer
    );

    return float4( 1.0f, 1.0f, 1.0f, coverage );
}
