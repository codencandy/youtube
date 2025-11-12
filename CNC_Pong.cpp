#include "CNC_Pong.h"

Application* LoadApplication( cnc::MemoryPool* pool, cnc::MemoryPool* transient, cnc::UserInput* input, PlatformServices* services, void* renderer, void* ui )
{
    Pong* pong = ALLOC_STRUCT( pool, Pong );

    InitApplication( pong, pool, transient, input, services, renderer, ui );

    Colour white          = colour( 1.0f, 1.0f, 1.0f, 1.0f );
    Colour red            = colour( 1.0f, 0.0f, 0.0f, 1.0f );
    Colour grey           = colour( 0.3f, 0.3f, 0.3f, 1.0f );
    v2     center         = vec2( pong->m_screenSize.x / 2.0f, pong->m_screenSize.y / 2.0f );
    v2     padSize        = vec2( 20.0f, 200.0f );
    v2     padLeft        = vec2( 50.0f,                        pong->m_screenSize.y / 2.0f - padSize.y / 2.0f );
    v2     padRight       = vec2( pong->m_screenSize.x - 50.0f, pong->m_screenSize.y / 2.0f - padSize.y / 2.0f );
    v2     wallSize       = vec2( pong->m_screenSize.x, 20.0f );
    f32    ballRadius     = 20.0f;

    pong->m_colliders     = NULL;
    pong->m_ball          = cnc::CreateCircle( center, ballRadius, white );
    pong->m_padLeft       = cnc::CreateRectangle( padLeft, padSize, red );
    pong->m_padRight      = cnc::CreateRectangle( padRight, padSize, red );
    pong->m_wallTop       = cnc::CreateRectangle( vec2( 0.0f, 0.0f ), wallSize, grey );
    pong->m_wallBottom    = cnc::CreateRectangle( vec2( 0.0f, pong->m_screenSize.y - 20.0f ), wallSize, grey );
    pong->m_grid          = cnc::CreateGrid( pool, pong->m_screenSize, 20.0f, &pong->m_numGridLines );
    pong->m_friction      = 0.0f;
    pong->m_restitution   = 1.0f;

    cnc::InitCollider( &pong->m_ball,       false, center,                            vec2(500.0f, 10.0f ),  0.0f,  0.1f );
    cnc::InitCollider( &pong->m_padLeft,    false, padLeft  + (padSize/2.0f),         vec2(0.0f, 0.0f),      0.0f, 10.0f );
    cnc::InitCollider( &pong->m_padRight,   false, padRight + (padSize/2.0f),         vec2(0.0f, 0.0f),      0.0f, 10.0f );
    cnc::InitCollider( &pong->m_wallTop,    true,  vec2(0.0f,0.0f) + (wallSize/2.0f), vec2(0.0f,0.0f),       0.0f, 10.0f );
    cnc::InitCollider( &pong->m_wallBottom, true,  vec2( 0.0f, pong->m_screenSize.y - 20.0f ) + ( wallSize / 2.0f ), vec2(0.0f,0.0f), 0.0f, 10.0f );

    arrput( pong->m_colliders, &pong->m_ball );
    arrput( pong->m_colliders, &pong->m_padLeft );
    arrput( pong->m_colliders, &pong->m_padRight );
    arrput( pong->m_colliders, &pong->m_wallTop );
    arrput( pong->m_colliders, &pong->m_wallBottom );

    pong->m_leftBounds  = 0.0f - ballRadius;
    pong->m_rightBounds = pong->m_screenSize.x + ballRadius;

    cnc::CreateCtrlValue( pong, FLOAT_VALUE, SLIDER, "friction",       &pong->m_friction );
    cnc::CreateCtrlValue( pong, FLOAT_VALUE, SLIDER, "restitution",    &pong->m_restitution );
    cnc::CreateCtrlValue( pong, FLOAT_VALUE, SLIDER, "ball mass",      &pong->m_ball.m_mass );
    cnc::CreateCtrlValue( pong, FLOAT_VALUE, SLIDER, "pad left mass",  &pong->m_padLeft.m_mass );
    cnc::CreateCtrlValue( pong, FLOAT_VALUE, SLIDER, "pad right mass", &pong->m_padRight.m_mass );

    return pong;
}

void UpdateApplication( Application* application )
{
    Pong*      app        = (Pong*)application;
    cnc::UserInput* input = application->m_input;
    u32        shapeCount = arrlen( app->m_colliders );
    Ball*      ball       = &app->m_ball;
    Pad*       leftPad    = &app->m_padLeft;
    Pad*       rightPad   = &app->m_padRight;

    // right pad movement
    if( KeyDown( input, cnc::KEY_DOWN ) )
    {
        rightPad->m_position.y += 1.0f;
        rightPad->m_center.y   += 1.0f;
    }
    else if( KeyDown( input, cnc::KEY_UP ) )
    {
        rightPad->m_position.y -= 1.0f;
        rightPad->m_center.y   -= 1.0f;
    }
    
    // left pad movement
    if( KeyDown( input, cnc::KEY_S ) )
    {
        leftPad->m_position.y += 1.0f;
        leftPad->m_center.y   += 1.0f;
    } 
    else if( KeyDown( input, cnc::KEY_W ) )
    {
        leftPad->m_position.y -= 1.0f;
        leftPad->m_center.y   -= 1.0f;
    }
    
    // ball movement
    ball->m_position = ball->m_position + (ball->m_velocity * app->m_timeInfo.m_dt);
    ball->m_center   = ball->m_position;

    // simulation loop
    for( u32 i = 0; i < shapeCount; ++i ) 
    {
        for( u32 j = i + 1; j < shapeCount; ++j ) 
        {
            Contact c = cnc::DetectCollision( app->m_colliders[i], app->m_colliders[j] );
            if( c.m_colliding ) 
            {
                cnc::ResolveCollision( app->m_colliders[i], app->m_colliders[j], c, app->m_restitution, app->m_friction );
            }
        }
    }

    // reset the ball in case of out of bounds
    if( ball->m_center.x < app->m_leftBounds || ball->m_center.x > app->m_rightBounds )
    {
        ball->m_position = vec2( app->m_screenSize.x / 2.0f, app->m_screenSize.y / 2.0f );
        ball->m_velocity = vec2( 500.0f, 10.0f );
        ball->m_center   = ball->m_position;
    }
}

void RenderApplication( Application* app )
{
    Pong* pong = (Pong*)app;
    
    cnc::DrawGrid     ( pong, pong->m_grid, pong->m_numGridLines );
    cnc::DrawRectangle( pong, pong->m_wallTop );
    cnc::DrawRectangle( pong, pong->m_wallBottom );
    cnc::DrawCircle   ( pong, pong->m_ball );
    cnc::DrawRectangle( pong, pong->m_padLeft );
    cnc::DrawRectangle( pong, pong->m_padRight );

    cnc::ShowDebugValue( pong, FLOAT_VALUE, TEXT_BOX, "speed", cnc::length( pong->m_ball.m_velocity ) );
}
