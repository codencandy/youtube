#ifndef CNC_CLOCKAPP_H
#define CNC_CLOCKAPP_H

#include "CNC_Types.h"
#include "CNC_Application.h"

typedef struct ClockApp : Application
{
    Image* m_background;
    Image* m_hourHand;
    Image* m_minuteHand;
    Image* m_knob;
    Image* m_highlight;

    u32    m_hours;

} ClockApp;

#endif//CNC_CLOCKAPP_H