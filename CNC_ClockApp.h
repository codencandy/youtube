#ifndef CNC_CLOCKAPP_H
#define CNC_CLOCKAPP_H

#define CNC_CANDYENGINE_IMPLEMENTATION
#include "engine/CNC_CandyEngine.h"

typedef struct ClockApp : Application
{
    Image* m_background;
    Image* m_hourHand;
    Image* m_minuteHand;
    Image* m_knob;
    Image* m_highlight;

    u32    m_hours;
    u32    m_minutes;

} ClockApp;

#endif//CNC_CLOCKAPP_H