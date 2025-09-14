#ifndef CNC_SHAPEDEMO_H
#define CNC_SHAPEDEMO_H

#include "CNC_Types.h"
#include "CNC_Application.h"

typedef struct ShapeDemo : Application
{
    Rectangle m_testRect;
    Circle    m_testCircle;
    Line      m_testLine;

    u32       m_numGridLines;
    Line*     m_grid;

} ShapeDemo;

#endif//CNC_SHAPEDEMO_H
