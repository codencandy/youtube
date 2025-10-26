#include "CNC_ClockApp.h"

Application* LoadApplication( cnc::MemoryPool* pool, cnc::MemoryPool* transient, cnc::UserInput* input, PlatformServices* services, void* renderer, void* ui )
{
    ClockApp* app = ALLOC_STRUCT( pool, ClockApp );

    InitApplication( app, pool, transient, input, services, renderer, ui );

    app->m_background  = cnc::CreateImage( app, "res/clock_bgr.png" );
    app->m_hourHand    = cnc::CreateImage( app, "res/clock_hours.png" );
    app->m_minuteHand  = cnc::CreateImage( app, "res/clock_minutes.png" );
    app->m_knob        = cnc::CreateImage( app, "res/clock_knob.png" );
    app->m_highlight   = cnc::CreateImage( app, "res/clock_highlight.png" );

    return app;
}

void UpdateApplication( Application* application )
{
    ClockApp*         app      = (ClockApp*)application;
    PlatformServices* services = app->m_services;

    app->m_hours   = app->m_timeInfo.m_hours;
    app->m_minutes = app->m_timeInfo.m_minute;
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
    cnc::DrawImage( app, app->m_hourHand,   center,          hourR, cnc::toRadians( -hourAngle) );
    cnc::DrawImage( app, app->m_minuteHand, center,        minuteR, cnc::toRadians( -minuteAngle ) );
    cnc::DrawImage( app, app->m_knob,       center - knobP );
}
