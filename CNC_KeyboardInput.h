#ifndef CNC_KEYBOARDINPUT_H
#define CNC_KEYBOARDINPUT_H

#include "CNC_Types.h"

typedef struct Key
{
    bool m_isDown;
    bool m_wasDown;
    u32  m_transitions;

} Key;

#define KC_LEFTARROW  0x7B
#define KC_RIGHTARROW 0x7C
#define KC_DOWNARROW  0x7D
#define KC_UPARROW    0x7E

enum ARROW_KEYS
{
    ARROW_LEFT  = 0, // -> KC_LEFTARROW
    ARROW_RIGHT = 1, // -> KC_RIGHTARROW
    ARROW_DOWN  = 2, // -> KC_DOWNARROW
    ARROW_UP    = 3  // -> KC_UPARROW
};

typedef struct KeyboardInput
{
    Key m_arrowKeys[4];

} KeyboardInput;

bool SingleKeyPress( Key* k )
{
    return ( k->m_transitions > 0 && 
            (!k->m_wasDown || k->m_transitions > 1) ) ? true : false;
}

#endif//CNC_KEYBOARDINPUT_H
