#ifndef CNC_CLOCKAPP_H
#define CNC_CLOCKAPP_H

#include "CNC_Types.h"

typedef struct ClockApp
{
    void* m_renderer;
    Image* m_clockBgr;
    Image* m_clockKnob;
    Image* m_clockHourHand;
    Image* m_clockMinutesHand;

} ClockApp;

void LoadClockApp( ClockApp* app );
void UpdateClockApp( ClockApp* app );
void RenderClockApp( ClockApp* app );

#endif//CNC_CLOCKAPP_H