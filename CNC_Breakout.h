#ifndef CNC_BREAKOUT_H
#define CNC_BREAKOUT_H

#include "CNC_Types.h"
#include "CNC_UserInput.h"

/* 
    class ATARI breakout
    --------------------------
    8 rows 
    14 bricks per row
*/

typedef struct Brick
{
    v2   m_position;
    v2   m_size;
    bool m_hit;

} Brick;

typedef struct Paddle
{
    v2 m_position;
    v2 m_size;

} Paddle;

typedef struct Ball
{
    v2 m_position;
    v2 m_size;
    
} Ball;

typedef struct Breakout
{
    void* m_renderer;

    UserInput* m_input;
    Brick*     m_bricks;
    Paddle     m_paddle;
    Ball*      m_ball;

    Image*     m_paddleImg;
    Image*     m_ballImg;

} Breakout;

void LoadBreakout( Breakout* app );
void UpdateBreakout( Breakout* app );
void RenderBreakout( Breakout* app );

#endif//CNC_BREAKOUT_H
