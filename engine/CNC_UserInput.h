#ifndef CNC_USERINPUT_H
#define CNC_USERINPUT_H

#include "CNC_Types.h"
#include "CNC_Memory.h"

#define CNC_NUM_KEYS 8

namespace cnc
{
    enum key_code
    {
        KEY_DOWN  = 0,
        KEY_UP    = 1,
        KEY_LEFT  = 2,
        KEY_RIGHT = 3,
        KEY_W     = 4,
        KEY_A     = 5,
        KEY_S     = 6,
        KEY_D     = 7
    };

    typedef struct Key
    {
        key_code m_code;
        bool     m_down;

    } Key;

    typedef struct UserInput
    {
        Key m_keys[CNC_NUM_KEYS];

    } UserInput;

    UserInput* CreateUserInput( cnc::MemoryPool* pool )
    {
        UserInput* input = ALLOC_STRUCT( pool, UserInput );

        input->m_keys[0].m_code = KEY_DOWN;
        input->m_keys[1].m_code = KEY_UP;
        input->m_keys[2].m_code = KEY_LEFT;
        input->m_keys[3].m_code = KEY_RIGHT;
        input->m_keys[4].m_code = KEY_W;
        input->m_keys[5].m_code = KEY_A;
        input->m_keys[6].m_code = KEY_S;
        input->m_keys[7].m_code = KEY_D;

        for( u32 i=0; i<CNC_NUM_KEYS; ++i )
        {
            input->m_keys[i].m_down = false;
        }
        
        return input;
    }

    bool KeyDown( UserInput* input, key_code code )
    {
        for( u32 i=0; i<CNC_NUM_KEYS; ++i )
        {
            if( input->m_keys[i].m_code == code )
            {
                return input->m_keys[i].m_down;
            }
        }

        return false;
    }

    void SetKeyDown( UserInput* input, key_code code )
    {
        for( u32 i=0; i<CNC_NUM_KEYS; ++i )
        {
            if( input->m_keys[i].m_code == code )
            {
                input->m_keys[i].m_down = true;
                return;
            }
        }
    }

    void SetKeyUp( UserInput* input, key_code code )
    {
        for( u32 i=0; i<CNC_NUM_KEYS; ++i )
        {
            if( input->m_keys[i].m_code == code )
            {
                input->m_keys[i].m_down = false;
                return;
            }
        }
    }
}

#endif//CNC_USERINPUT_H