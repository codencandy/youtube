#ifndef CNC_INPUT_H
#define CNC_INPUT_H

#include "CNC_Types.h"

enum arrow_keys
{
    ARROW_LEFT  = 0,
    ARROW_RIGHT = 1,
    ARROW_DOWN  = 2,
    ARROW_UP    = 3
};

enum function_keys
{
    ESCAPE = 0,
    ENTER  = 1,
    SPACE  = 2,
};

typedef struct Key
{
    bool m_isDown;
    bool m_wasDown;
    u32  m_transitions;

} Key;

typedef struct Input
{
    Key m_arrowKeys[4];
    Key m_functionKeys[3];

} Input;

bool KeyPressed( Input* input, arrow_keys key )
{
    Key* k = &input->m_arrowKeys[key];
    if( k->m_isDown && k->m_transitions > 0 )
    {
        return true;
    }

    return false;
}

bool KeyPressed( Input* input, function_keys key )
{
    Key* k = &input->m_functionKeys[key];
    if( k->m_isDown && k->m_transitions > 0 )
    {
        return true;
    }

    return false;
}

#endif//CNC_INPUT_H
