#ifndef CNC_CANDYENGINE_H
#define CNC_CANDYENGINE_H

#include "CNC_Types.h"
#include "CNC_Application.h"

namespace cnc
{
    // 2D drawing
    void      DrawRectangle  ( Application* app, v2 position, v2 size, Colour c );
    void      DrawRectangle  ( Application* app, Rectangle& rect );
    void      DrawCircle     ( Application* app, v2 position, f32 radius, Colour c );    
    void      DrawCircle     ( Application* app, Circle& cirlce );
    void      DrawLine       ( Application* app, v2 start, v2 end, Colour c );
    void      DrawLine       ( Application* app, Line& line );
    void      DrawImage      ( Application* app, Image* image, v2 position, v2 pivotPoint = vec2(0.0f, 0.0f), f32 angle = 0.0f );
    
    Image*    CreateImage    ( Application* app, const char* imageName );
    Rectangle CreateRectangle( v2 position, v2 size, Colour c );
    Circle    CreateCircle   ( v2 center, f32 radius, Colour c );
    Line      CreateLine     ( v2 start, v2 end, Colour c );
    Line*     CreateGrid     ( MemoryPool* pool, v2 screenSize, f32 cellSize, u32* numLines );
}

#endif//CNC_CANDYENGINE_H
 