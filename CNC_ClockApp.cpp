#include <stddef.h>
#include <time.h>

#include "CNC_Memory.h"
#include "CNC_ClockApp.h"
#include "CNC_PlatformServices.h"
#include "CNC_Math.h"

#include "CNC_CandyEngine.cpp"

Application* LoadApplication( PlatformServices* services, void* renderer )
{
    ClockApp* app = (ClockApp*)malloc( sizeof( ClockApp ) );

    app->m_renderer    = renderer;
    app->m_services    = services;

    app->m_background  = cnc::LoadImage( app, "res/clock_bgr.png" );
    app->m_hourHand    = cnc::LoadImage( app, "res/clock_hours.png" );
    app->m_minuteHand  = cnc::LoadImage( app, "res/clock_minutes.png" );
    app->m_knob        = cnc::LoadImage( app, "res/clock_knob.png" );
    app->m_highlight   = cnc::LoadImage( app, "res/clock_highlight.png" );

    return app;
}

void UpdateApplication( Application* application )
{
    ClockApp*         app      = (ClockApp*)application;
    PlatformServices* services = app->m_services;

    time_t rawTime;
    tm*    timeInfo;

    time( &rawTime );
    timeInfo = localtime( &rawTime );

    app->m_hours   = (timeInfo->tm_hour) % 12;
    app->m_minutes = timeInfo->tm_min;
}

void RenderApplication( Application* application )
{
    ClockApp*         app      = (ClockApp*)application;
    PlatformServices* services = app->m_services;
    
    v2                center   = vec2(  app->m_background->m_width / 2.0f,  app->m_background->m_height / 2.0f );
    v2                knobP    = vec2(  app->m_knob->m_width       / 2.0f,  app->m_knob->m_height / 2.0f );
    v2                minuteR  = vec2( -app->m_minuteHand->m_width / 2.0f, -app->m_minuteHand->m_height );
    v2                hourR    = vec2( -app->m_hourHand->m_width   / 2.0f, -app->m_hourHand->m_height );

    f32 minuteAngle = (180.0f / 30.0f)         * app->m_minutes;
    f32 minsPerHour = ((180.0f / 6.0) / 60.0f) * app->m_minutes;
    f32 hourAngle   = ((180.0f / 6.0f)         * app->m_hours) + minsPerHour;

    cnc::DrawImage( app, app->m_background, vec2( 0.0f, 0.0f ) );
    cnc::DrawImage( app, app->m_hourHand,   center,          hourR, toRadians( -hourAngle) );
    cnc::DrawImage( app, app->m_minuteHand, center,        minuteR, toRadians( -minuteAngle ) );
    cnc::DrawImage( app, app->m_knob,       center - knobP );
}
