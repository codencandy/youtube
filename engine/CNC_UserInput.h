#ifndef CNC_USERINPUT_H
#define CNC_USERINPUT_H

#include "CNC_Types.h"
#include "CNC_Memory.h"

#define CNC_NUM_KEYS 4

namespace cnc
{
    enum key_code
    {
        KEY_DOWN  = 1,
        KEY_UP    = 2,
        KEY_LEFT  = 3,
        KEY_RIGHT = 4
    };

    typedef struct Key
    {
        key_code m_code;
        bool     m_up;
        bool     m_down;

    } Key;

    typedef struct UserInput
    {
        Key m_keys[CNC_NUM_KEYS];

    } UserInput;

    UserInput* CreateUserInput( cnc::MemoryPool* pool )
    {
        UserInput* input = ALLOC_STRUCT( pool, UserInput );
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
}

#endif//CNC_USERINPUT_H