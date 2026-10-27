#include "CNC_Input.h"

#define KC_ARROWLEFT  0x7B
#define KC_ARROWRIGHT 0x7C
#define KC_ARROWDOWN  0x7D
#define KC_ARROWUP    0x7E

void InitInput( Input* input )
{
    u32 numArrowKeys = 4;
    for( u32 i=0; i<numArrowKeys; ++i )
    {
        input->m_arrowKeys[i].m_wasDown     = false;
        input->m_arrowKeys[i].m_isDown      = false;
        input->m_arrowKeys[i].m_transitions = 0;
    }
}

void ResetInput( Input* input )
{
    u32 numArrowKeys = 4;
    for( u32 i=0; i<numArrowKeys; ++i )
    {
        input->m_arrowKeys[i].m_wasDown     = input->m_arrowKeys[i].m_isDown;
        input->m_arrowKeys[i].m_transitions = 0;
    }
}

bool ProcessInput( NSEvent* event, Input* input )
{
    bool isDown = (event.type == NSEventTypeKeyDown );

    if( event.type == NSEventTypeKeyDown || event.type == NSEventTypeKeyUp )
    {
        if( event.keyCode >= KC_ARROWLEFT && event.keyCode <= KC_ARROWUP )
        {
            Key* k = &input->m_arrowKeys[event.keyCode - KC_ARROWLEFT];

            if( isDown != k->m_isDown )
            {
                k->m_isDown = isDown;
                k->m_transitions++;
            }
            return true;
        }
        return false;
    }

    return false;
}