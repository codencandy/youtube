#include "CNC_Input.h"

#define KC_ARROWLEFT  0x7B
#define KC_ARROWRIGHT 0x7C
#define KC_ARROWDOWN  0x7D
#define KC_ARROWUP    0x7E

#define KC_ESCAPE     0x35
#define KC_ENTER      0x24
#define KC_SPACE      0x31

void InitInput( Input* input )
{
    u32 numArrowKeys = 4;
    for( u32 i=0; i<numArrowKeys; ++i )
    {
        input->m_arrowKeys[i].m_wasDown     = false;
        input->m_arrowKeys[i].m_isDown      = false;
        input->m_arrowKeys[i].m_transitions = 0;
    }

    u32 numFunctionKeys = 3;
    for( u32 i=0; i<numFunctionKeys; ++i )
    {
        input->m_functionKeys[i].m_wasDown     = false;
        input->m_functionKeys[i].m_isDown      = false;
        input->m_functionKeys[i].m_transitions = 0;
    }

    input->m_mouse.m_position = vec2( (s16)0, (s16)0 );
    input->m_mouse.m_wheel    = 0.0f;
}

void ResetInput( Input* input )
{
    u32 numArrowKeys = 4;
    for( u32 i=0; i<numArrowKeys; ++i )
    {
        input->m_arrowKeys[i].m_wasDown     = input->m_arrowKeys[i].m_isDown;
        input->m_arrowKeys[i].m_transitions = 0;
    }

    u32 numFunctionKeys = 3;
    for( u32 i=0; i<numFunctionKeys; ++i )
    {
        input->m_functionKeys[i].m_wasDown     = input->m_functionKeys[i].m_isDown;
        input->m_functionKeys[i].m_transitions = 0;
    }

    input->m_mouse.m_wheel = 0.0f;
}

bool ProcessInput( NSEvent* event, Input* input )
{
    Key* k      = NULL;
    bool isDown = (event.type == NSEventTypeKeyDown );

    if( event.type == NSEventTypeKeyDown || event.type == NSEventTypeKeyUp )
    {
        if( event.keyCode >= KC_ARROWLEFT && event.keyCode <= KC_ARROWUP )
        {
            k = &input->m_arrowKeys[event.keyCode - KC_ARROWLEFT];
        }

        if( event.keyCode == KC_ESCAPE )
        {
            k = &input->m_functionKeys[ESCAPE];
        }

        if( event.keyCode == KC_ENTER )
        {
            k = &input->m_functionKeys[ENTER];
        }

        if( event.keyCode == KC_SPACE )
        {
            k = &input->m_functionKeys[SPACE];
        }

        if( k != NULL && isDown != k->m_isDown )
        {
            k->m_isDown = isDown;
            k->m_transitions++;
            return true;
        }
        return false;
    }

    if( event.type == NSEventTypeScrollWheel )
    {
        input->m_mouse.m_wheel += (f32)event.scrollingDeltaY;
        return true;
    }

    return false;
}