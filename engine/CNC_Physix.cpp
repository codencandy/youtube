#include "CNC_Physix.h"
#include <stdio.h>

namespace cnc
{
    Contact DetectCollision( Shape* a, Shape* b )
    {
        Contact c;
        
        switch( a->m_type ) 
        {
            // no collision for these shape types
            case CNC_IMAGE:    break;
            case CNC_PARTICLE: break;
    
            case CNC_CIRCLE:
            {
                switch( b->m_type )  
                {
                    case CNC_CIRCLE: return detect_circle_circle(a, b);
                    case CNC_RECT:   return detect_circle_rect(a, b);
                    case CNC_LINE:   return detect_circle_line(a, b);
                    default: break;
                }
                break;
            }
        
            case CNC_RECT:
            {
                switch( b->m_type ) 
                {
                    case CNC_RECT:   return detect_rect_rect( a, b );
                    case CNC_LINE:   return detect_rect_line( a, b );
                    case CNC_CIRCLE: 
                    { 
                        Contact c  = detect_circle_rect( b, a ); 
                        c.m_normal = c.m_normal * -1.0f; 
                        return c; 
                    }
                    default: break;
                }
            }
        
            case CNC_LINE:
            {
                switch( b->m_type ) 
                {
                    case CNC_CIRCLE: 
                    { 
                        Contact c = detect_circle_line( b, a ); 
                        c.m_normal = c.m_normal * -1.0f; 
                        return c; 
                    }
                    case CNC_LINE:   return detect_line_line( a, b );
                    case CNC_RECT:   return detect_rect_line( b, a );
                    default: break;
                }
                break;
            }
        }
        
        return c;
    }
    
    void ResolveCollision( Shape* a, Shape* b, Contact c, f32 restitution, f32 friction)
    {
        if( !c.m_colliding ) return;
    
        f32 invMassA = (a->m_static || a->m_mass <= 0) ? 0.0f : 1.0f / a->m_mass;
        f32 invMassB = (b->m_static || b->m_mass <= 0) ? 0.0f : 1.0f / b->m_mass;
        f32 invMassSum = invMassA + invMassB;
        if (invMassSum == 0.0f) return;
    
        // --- 1. Positional correction ---
        const f32 percent = 0.8f;
        const f32 slop = 0.01f;
        f32 correctionMag = cnc_fmax(c.m_penetration - slop, 0.0f) * percent / invMassSum;
        v2  correction    = c.m_normal * correctionMag;
    
        if( !a->m_static )
        {
            a->m_position = a->m_position - ( correction * invMassA );
            if( a->m_type == CNC_CIRCLE )
            {
                Circle* circle   = (Circle*)a;
                circle->m_center = a->m_position;
            }
        } 
            
        if( !b->m_static ) 
        {
            b->m_position = b->m_position + ( correction * invMassB );
        }
    
        // --- 2. Relative velocity ---
        v2 rv = b->m_velocity - a->m_velocity;
        f32 velAlongNormal = cnc_dot(rv, c.m_normal);
        if (velAlongNormal > 0) return; // separating
    
        // --- 3. Compute impulse scalar ---
        f32 j = -(1.0f + restitution) * velAlongNormal / invMassSum;
        v2 impulse = c.m_normal * j;
    
        // --- 4. Apply impulse ---
        if( !a->m_static) a->m_velocity = a->m_velocity - (impulse * invMassA);
        if( !b->m_static) b->m_velocity = b->m_velocity + (impulse * invMassB);
    
        // --- 5. Friction ---
        rv = b->m_velocity - a->m_velocity;
        v2 tangent = rv - (c.m_normal * cnc_dot(rv, c.m_normal));
        if( cnc_length(tangent) > 1e-6f) tangent = cnc_normalize(tangent);
    
        f32 jt = -cnc_dot(rv, tangent) / invMassSum;
        f32 maxFriction = friction * j;
        if ( cnc_absf(jt) > maxFriction) jt = (jt < 0) ? -maxFriction : maxFriction;
    
        v2 frictionImpulse = tangent * jt;
        if( !a->m_static) a->m_velocity = a->m_velocity - (frictionImpulse * invMassA);
        if( !b->m_static) b->m_velocity = b->m_velocity + (frictionImpulse * invMassB);
    }
    
    Contact detect_circle_circle( Shape* a, Shape* b )
    {
        Contact c;
        return c;
    }
    
    Contact detect_circle_rect( Shape* a, Shape* b )
    {
        Contact c = {0};
        Circle*    circle = (Circle*)a;
        Rectangle* rect   = (Rectangle*)b;
    
        v2 diff = circle->m_center - rect->m_center;
    
        
        // Clamp circle center to rectangle bounds (AABB)
        v2 closest = vec2(
            cnc_fmax( -(rect->m_size.x / 2.0f), cnc_fmin(diff.x, rect->m_size.x / 2.0f) ),
            cnc_fmax( -(rect->m_size.y / 2.0f), cnc_fmin(diff.y, rect->m_size.y / 2.0f) )
        );
        
        v2  closest_world = rect->m_center   + closest;
        v2  dist_vec      = circle->m_center - closest_world;
        f32 dist2         = cnc_dot(dist_vec, dist_vec);
        f32 radius        = circle->m_radius;
    
        if (dist2 > (radius * radius))
            return c;
    
        f32 dist        = cnc_sqr( dist2 );
        c.m_colliding   = true;
        c.m_normal      = (dist > 1e-6f) ? dist_vec * (-1.0f / dist) : vec2( 0, 1 );
        c.m_penetration = radius - dist;
    
        return c;
    }
    
    Contact detect_circle_line( Shape* a, Shape* b )
    {
        Contact c;
        return c;
    }
    
    Contact detect_rect_rect( Shape* a, Shape* b )
    {
        Contact c;
        return c;
    }
    
    Contact detect_rect_line( Shape* a, Shape* b )
    {
        Contact c;
        return c;
    }
    
    Contact detect_line_line( Shape* a, Shape* b )
    {
        Contact c;
        return c;
    }
}
