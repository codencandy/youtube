#ifndef CNC_SHAPEDEMO_H
#define CNC_SHAPEDEMO_H

#define CNC_CANDYENGINE_IMPLEMENTATION
#include "engine/CNC_CandyEngine.h"

typedef struct ShapeDemo : Application
{
    Rectangle m_testRect;
    Circle    m_testCircle;
    Line      m_testLine;

    u32       m_numGridLines;
    Line*     m_grid;

} ShapeDemo;

#endif//CNC_SHAPEDEMO_H
