#ifndef CNC_TIMER_H
#define CNC_TIMER_H

#include "CNC_Types.h"
#include <time.h>

typedef struct Timer
{
    timespec m_start;
    timespec m_current;
    f64      m_seconds;
    f64      m_millis;

} Timer;

void StartTimer( Timer* t );
void UpdateTimer( Timer* t );

void StartTimer( Timer* t )
{
    clock_gettime( CLOCK_MONOTONIC, &t->m_start );

    t->m_current = t->m_start;
    t->m_seconds = 0;
    t->m_millis  = 0;
}

void UpdateTimer( Timer*t )
{
    clock_gettime( CLOCK_MONOTONIC, &t->m_current );
    
    s64 seconds  = t->m_current.tv_sec  - t->m_start.tv_sec;
    s64 nanos    = t->m_current.tv_nsec - t->m_start.tv_nsec;
    t->m_seconds = (f64)seconds + (f64)nanos / 1000000000.0;
    t->m_millis  = (f64)seconds * 1000.0 + (f64)nanos / 1000000.0;;
}

#endif//CNC_TIMER_H
