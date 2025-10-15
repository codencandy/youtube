#ifndef CNC_PONG_H
#define CNC_PONG_H

#include "CNC_Types.h"
#include "CNC_Application.h"

typedef struct Ball
{
    Circle m_circle;
    f32    m_radius;
    v2     m_position;
    v2     m_velocity;
    f32    m_acceleration;

} Ball;

typedef struct Pad
{
    Rectangle m_rect;
    v2        m_normal;

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
