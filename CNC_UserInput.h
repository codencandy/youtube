#ifndef CNC_USERINPUT_H
#define CNC_USERINPUT_H

#include "CNC_Types.h"

#define ARROW_UP_CODE    0x7E
#define ARROW_DOWN_CODE  0x7D
#define ARROW_LEFT_CODE  0x7B
#define ARROW_RIGHT_CODE 0x7C

typedef struct KeyInput
{
    bool m_isDown;
    bool m_wasDown;
    u32  m_halftransitions;
    u32  m_code;

} KeyInput;

typedef struct UserInput
{
    KeyInput m_right;
    KeyInput m_left;
    KeyInput m_up;
    KeyInput m_down;

} UserInput;

bool ProcessInput( UserInput* input, void* event );

bool KeyDown( UserInput* input, u32 code )
{
    switch( code )
    {
        case ARROW_UP_CODE:    { return input->m_up.m_isDown; }
        case ARROW_DOWN_CODE:  { return input->m_down.m_isDown; }
        case ARROW_LEFT_CODE:  { return input->m_left.m_isDown; }
        case ARROW_RIGHT_CODE: { return input->m_right.m_isDown; }
    }

    return false;
}

#define LeftKey( i ) (KeyDown(i,ARROW_LEFT_CODE))
#define RightKey( i ) (KeyDown(i,ARROW_RIGHT_CODE))
#define UpKey( i ) (KeyDown(i,ARROW_UP_CODE))
#define DownKey( i ) (KeyDown(i,ARROW_DOWN_CODE))

#endif//CNC_USERINPUT_H
