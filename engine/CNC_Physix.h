#ifndef CNC_PHYSIX_H
#define CNC_PHYSIX_H

#include "CNC_Types.h"
#include "CNC_Math.h"

namespace cnc
{
    // there are different types of collision
    //
    // 1. moving object with static object
    // 2. moving object with moving object
    //
    // what's the expectation of the outcome
    // 1. a collision happened
    // 2. new position based on the collision
    // 3. updated direction based on collision
    // 4. updated velocity and acceleration based on collision

    // the detection function return true in case of a collision
    // and outputs a new out normal vector and the out penetration vector

    // the resolve function applies the results of the detection function to 
    // calculate the new position of the shapes

    Contact DetectCollision(  Shape* a, Shape* b );
    void    ResolveCollision( Shape* a, Shape* b, Contact c, f32 restitution, f32 friction);

    Contact detect_circle_circle( Shape* a, Shape* b );
    Contact detect_circle_rect(   Shape* a, Shape* b );
    Contact detect_circle_line(   Shape* a, Shape* b );
    Contact detect_rect_rect(     Shape* a, Shape* b );
    Contact detect_rect_line(     Shape* a, Shape* b );
    Contact detect_line_line(     Shape* a, Shape* b );

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

    }

    Contact detect_circle_circle( Shape* a, Shape* b )
    {
        Contact c;
        return c;
    }
    
    Contact detect_circle_rect( Shape* a, Shape* b )
    {
        Contact c;
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

#endif//CNC_PHYSIX_H