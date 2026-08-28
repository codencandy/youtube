#ifndef CNC_TEXTDEMO_H
#define CNC_TEXTDEMO_H

#include "CNC_Types.h"
#include "CNC_TtfTypes.h"
#include "CNC_Application.h"

typedef struct TextDemo : Application
{
    File* m_fontFile;
    Font* m_ttfFont;
    
} TextDemo;

#endif//CNC_TEXTDEMO_H