#ifndef CNC_PONG_H
#define CNC_PONG_H

#include "CNC_Types.h"
#include "CNC_Application.h"

typedef struct Ball
{
    Circle m_cirlce;

} Ball;

typedef struct Pad
{
    Rectangle m_rect;

} Pad;

typedef struct Pong : Application
{
    Ball  m_ball;
    Pad   m_padLeft;
    Pad   m_padRight;

    u32   m_numGridLines;
    Line* m_grid;

} Pong;

#endif//CNC_PONG_H
