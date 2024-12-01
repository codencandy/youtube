#include "CNC_ClockApp.h"
#include "CNC_PlatformServices.h"
#include <time.h>

#define CNC_PI 3.14159265359

f32 degreesToRadian( f32 degrees )
{
    f32 oneDegree = CNC_PI / 180.0f;
    f32 radians   = degrees * oneDegree;

    return radians;
}

void LoadClockApp( ClockApp* app )
{
    u32 windowWidth  = 600;
    u32 windowHeight = 600;
    u32 centerX      = windowWidth  / 2;
    u32 centerY      = windowHeight / 2;

    app->m_clockBgr         = PlatformLoadImage( "res/clock_bgr.png" );
    app->m_clockKnob        = PlatformLoadImage( "res/clock_knob.png" );
    app->m_clockHourHand    = PlatformLoadImage( "res/clock_hours.png" );
    app->m_clockMinutesHand = PlatformLoadImage( "res/clock_minutes.png" );

    u32 knobPositionX = centerX - (app->m_clockKnob->m_width  / 2);
    u32 knobPositionY = centerY - (app->m_clockKnob->m_height / 2);

    app->m_clockBgr->m_modelMatrix         = transformationMatrix( 0.0f, 0.0f );
    app->m_clockKnob->m_modelMatrix        = transformationMatrix( knobPositionX, knobPositionY );
    app->m_clockHourHand->m_modelMatrix    = transformationMatrix( centerX, centerY );
    app->m_clockMinutesHand->m_modelMatrix = transformationMatrix( centerX, centerY );

    f32 hrWidth   = (f32)app->m_clockHourHand->m_width;
    f32 hrHeight  = (f32)app->m_clockHourHand->m_height;
    f32 minHeight = (f32)app->m_clockMinutesHand->m_height;
    app->m_clockHourHand->m_pivotMatrix    = transformationMatrix( -hrWidth/2.0f, -(hrHeight-15.0f) );
    app->m_clockBgr->m_pivotMatrix         = transformationMatrix( 0.0f, 0.0f );
    app->m_clockKnob->m_pivotMatrix        = transformationMatrix( 0.0f, 0.0f );
    app->m_clockMinutesHand->m_pivotMatrix = transformationMatrix( -hrWidth/2.0f, -(minHeight-15.0f) );

    app->m_clockBgr->m_rotation         = 0.0f;
    app->m_clockKnob->m_rotation        = 0.0f;
    app->m_clockHourHand->m_rotation    = 0.0f;
    app->m_clockMinutesHand->m_rotation = 0.0f;
    
    app->m_clockBgr->m_textureId         = PlatformUploadImage( app->m_renderer, app->m_clockBgr );
    app->m_clockKnob->m_textureId        = PlatformUploadImage( app->m_renderer, app->m_clockKnob );
    app->m_clockHourHand->m_textureId    = PlatformUploadImage( app->m_renderer, app->m_clockHourHand );
    app->m_clockMinutesHand->m_textureId = PlatformUploadImage( app->m_renderer, app->m_clockMinutesHand );
}

void UpdateClockApp( ClockApp* app )
{
    time_t rawtime;
    tm*    timeinfo;

    time( &rawtime );
    timeinfo    = localtime ( &rawtime );
    u32 hour    = timeinfo->tm_hour;
    u32 minutes = timeinfo->tm_min;
    
    f32 hourAngle   = (360.0f / 12.0f) * (hour % 12) + ((360.0f / 12.0f) / 60.0f) * minutes;
    f32 minuteAngle = (360.0f / 60.0f) * minutes;

    app->m_clockHourHand->m_rotation    = degreesToRadian(-hourAngle);
    app->m_clockMinutesHand->m_rotation = degreesToRadian(-minuteAngle);

    PlatformUpdateImage( app->m_renderer, app->m_clockHourHand );
    PlatformUpdateImage( app->m_renderer, app->m_clockMinutesHand );
}

void RenderClockApp( ClockApp* app )
{
    PlatformRenderImage( app->m_renderer, app->m_clockBgr->m_textureId );
    PlatformRenderImage( app->m_renderer, app->m_clockHourHand->m_textureId );
    PlatformRenderImage( app->m_renderer, app->m_clockMinutesHand->m_textureId );
    PlatformRenderImage( app->m_renderer, app->m_clockKnob->m_textureId );
}