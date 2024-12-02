#include "CNC_ClockApp.h"
#include "CNC_PlatformServices.h"

void LoadClockApp( ClockApp* app )
{
    app->m_background              = PlatformLoadImage( "res/clock_bgr.png" );
    app->m_background->m_textureId = PlatformUploadImage( app->m_renderer, app->m_background );

    app->m_hourHand                = PlatformLoadImage( "res/clock_hours.png" );
    app->m_hourHand->m_textureId   = PlatformUploadImage( app->m_renderer, app->m_hourHand );

    app->m_minuteHand              = PlatformLoadImage( "res/clock_minutes.png" );
    app->m_minuteHand->m_textureId = PlatformUploadImage( app->m_renderer, app->m_minuteHand );

    app->m_knob                    = PlatformLoadImage( "res/clock_knob.png" );
    app->m_knob->m_textureId       = PlatformUploadImage( app->m_renderer, app->m_knob );
}

void UpdateClockApp( ClockApp* app )
{

}

void RenderClockApp( ClockApp* app )
{
    PlatformRenderImage( app->m_renderer, app->m_background->m_textureId );
    PlatformRenderImage( app->m_renderer, app->m_hourHand->m_textureId );
    PlatformRenderImage( app->m_renderer, app->m_minuteHand->m_textureId );
    PlatformRenderImage( app->m_renderer, app->m_knob->m_textureId );
}