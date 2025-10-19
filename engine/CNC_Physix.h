#ifndef CNC_PHYSIX_H
#define CNC_PHYSIX_H

#include "CNC_Types.h"
#include "CNC_Math.h"

namespace cnc
{
    // 2D collision system
    Contact DetectCollision     ( Shape* a, Shape* b );
    void    ResolveCollision    ( Shape* a, Shape* b, Contact c, f32 restitution, f32 friction);

    Contact detect_circle_circle( Shape* a, Shape* b );
    Contact detect_circle_rect  ( Shape* a, Shape* b );
    Contact detect_circle_line  ( Shape* a, Shape* b );
    Contact detect_rect_rect    ( Shape* a, Shape* b );
    Contact detect_rect_line    ( Shape* a, Shape* b );
    Contact detect_line_line    ( Shape* a, Shape* b );
}

#include "CNC_Physix.cpp"

#endif//CNC_PHYSIX_H