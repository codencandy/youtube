#ifndef CNC_CHRISTMAS_H
#define CNC_CHRISTMAS_H

#include "CNC_Types.h"

typedef struct ChristmasApp
{
    void*  m_renderer;
    Image* m_foreground;
    Image* m_landscape;

} ChristmasApp;

void LoadChristmas( ChristmasApp* app );
void UpdateChristmas( ChristmasApp* app );
void RenderChristmas( ChristmasApp* app );

#endif//CNC_CHRISTMAS_H
