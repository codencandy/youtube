#ifndef CNC_TEXTDEMO_H
#define CNC_TEXTDEMO_H

#define CNC_CANDY_IMPLEMENTATION
#include "CNC_CandyEngine.h"

typedef struct TextDemo : Application
{
    TtfFont* m_montserrat;
    TtfFont* m_bladerunner;
    Image*   m_background;

} TextDemo;

#endif//CNC_TEXTDEMO_H
