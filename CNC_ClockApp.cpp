#include "CNC_ClockApp.h"
#include "CNC_PlatformServices.h"
#include "CNC_Math.h"
#include <time.h>

Application* LoadApplication( PlatformServices* services, void* renderer )
{
    ClockApp* app = (ClockApp*)malloc( sizeof( ClockApp ) );
    app->m_services = services;
    app->m_renderer = renderer;

    app->m_background  = services->f_loadImage( "res/clock_bgr.png" );
    app->m_hourHand    = services->f_loadImage( "res/clock_hours.png" );
    app->m_minuteHand  = services->f_loadImage( "res/clock_minutes.png" );
    app->m_knob        = services->f_loadImage( "res/clock_knob.png" );
    app->m_highlight   = services->f_loadImage( "res/clock_highlight.png" );

    ModelData bgrData   = {0};
    ModelData hourData  = {0};
    ModelData minData   = {0};
    ModelData knobData  = {0};
    ModelData highlight = {0};

    f32 centerX = (f32)app->m_background->m_width  / 2.0f;
    f32 centerY = (f32)app->m_background->m_height / 2.0f;

    f32 hourHeight      = app->m_hourHand->m_height - 15.0f;
    f32 hourWidth       = app->m_hourHand->m_width;
    f32 minHeight       = app->m_minuteHand->m_height - 15.0f;
    f32 minWidth        = app->m_minuteHand->m_width;
    f32 knobHeight      = app->m_knob->m_height;
    f32 knobWidth       = app->m_knob->m_width;
    f32 hightlightWidth = app->m_highlight->m_width;

    bgrData.m_pivotMatrix   = identityMatrix(); 
    hourData.m_pivotMatrix  = translationMatrix( -hourWidth / 2.0f, -hourHeight );
    minData.m_pivotMatrix   = translationMatrix( -minWidth / 2.0f,  -minHeight );
    knobData.m_pivotMatrix  = identityMatrix();
    highlight.m_pivotMatrix = translationMatrix( -hightlightWidth / 2.0f, -(minHeight + 52.0f ) );

    bgrData.m_modelMatrix  = identityMatrix();
    hourData.m_modelMatrix = translationMatrix(  centerX, centerY );
    minData.m_modelMatrix  = translationMatrix(  centerX, centerY) ;
    knobData.m_modelMatrix = translationMatrix(  centerX - knobWidth / 2.0f, centerY - knobHeight / 2.0f );
    highlight.m_modelMatrix = translationMatrix( centerX, centerY );

    highlight.m_rotation    = toRadians( -(180.0f / 6.0f) );

    app->m_background->m_modelData = bgrData;
    app->m_hourHand->m_modelData   = hourData;
    app->m_minuteHand->m_modelData = minData;
    app->m_knob->m_modelData       = knobData;
    app->m_highlight->m_modelData  = highlight;

    app->m_background->m_textureId = services->f_uploadImage( app->m_renderer, app->m_background );
    app->m_hourHand->m_textureId   = services->f_uploadImage( app->m_renderer, app->m_hourHand );
    app->m_minuteHand->m_textureId = services->f_uploadImage( app->m_renderer, app->m_minuteHand );
    app->m_knob->m_textureId       = services->f_uploadImage( app->m_renderer, app->m_knob );
    app->m_highlight->m_textureId  = services->f_uploadImage( app->m_renderer, app->m_highlight );

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

    f32 hours   = (timeInfo->tm_hour) % 12;
    f32 minutes = timeInfo->tm_min;

    f32 hourAngle   = (180.0f / 6.0f) * hours;
    f32 minuteAngle = (180.0f / 30.0f) * minutes;
    f32 minsPerHour = ((180.0f / 6.0) / 60.0f) * minutes;

    app->m_hourHand->m_modelData.m_rotation   = toRadians( -(hourAngle + minsPerHour) );
    app->m_minuteHand->m_modelData.m_rotation = toRadians( -minuteAngle );

    services->f_updateImage( app->m_renderer, app->m_hourHand );
    services->f_updateImage( app->m_renderer, app->m_minuteHand );

    app->m_hours = hours;
}

void RenderApplication( Application* application )
{
    ClockApp*         app      = (ClockApp*)application;
    PlatformServices* services = app->m_services;

    services->f_renderImage( app->m_renderer, app->m_background->m_textureId, 1 );
    services->f_renderImage( app->m_renderer, app->m_hourHand->m_textureId, 1 );
    services->f_renderImage( app->m_renderer, app->m_minuteHand->m_textureId, 1 );
    services->f_renderImage( app->m_renderer, app->m_knob->m_textureId, 1 );
    services->f_renderImage( app->m_renderer, app->m_highlight->m_textureId, app->m_hours );
}