#ifndef CNC_SHAPEDEMO_H
#define CNC_SHAPEDEMO_H

#include "CNC_Types.h"
#include "CNC_Application.h"

typedef struct ShapeDemo : Application
{
    Rectangle m_testRect;
    Circle    m_testCircle;

} ShapeDemo;

#endif//CNC_SHAPEDEMO_H
