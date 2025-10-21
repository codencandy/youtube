#ifndef CNC_PONG_H
#define CNC_PONG_H

#define CNC_CANDYENGINE_IMPLEMENTATION
#include "engine/CNC_CandyEngine.h"

typedef Circle     Ball;
typedef Rectangle  Pad;
typedef Rectangle  Wall;

typedef struct Pong : Application
{
    Ball      m_ball;
    Pad       m_padLeft;
    Pad       m_padRight;
    Wall      m_wallTop;
    Wall      m_wallBottom;

    u32       m_numGridLines;
    Line*     m_grid;

    Shape**   m_colliders;

} Pong;

#endif//CNC_PONG_H
