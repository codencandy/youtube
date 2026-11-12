#ifndef CNC_TEXTDEMO_H
#define CNC_TEXTDEMO_H

#include "CNC_TtfTypes.h"
#include "CNC_Application.h"

typedef struct TextDemo : Application
{
    TtfFont* m_montserrat;
    TtfFont* m_bladerunner;
    
} TextDemo;

#endif//CNC_TEXTDEMO_H