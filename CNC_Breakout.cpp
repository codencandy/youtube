#include "CNC_Breakout.h"
#include "CNC_PlatformServices.h"
#include "CNC_Math.h"
#include "CNC_Constants.h"

#include <stdio.h>

void Print( const char* text )
{
    printf( "%s", text ); fflush( stdout );
}

void LoadBreakout( Breakout* app )
{
    u32 rows         = 8;
    u32 bricksPerRow = 14;
    u32 numBricks    = rows * bricksPerRow;

    f32 brickWidth   = (f32)CNC_WINDOW_WIDTH / 14.0f;
    f32 brickHeight  = (f32)(CNC_WINDOW_HEIGHT / 2.0f) / 8.0f;

    app->m_bricks = (Brick*)malloc( sizeof( Brick ) * numBricks );

    f32 x = 0.0f;
    f32 y = 0.0f;

    for( u32 i=0; i<rows; ++i )
    {
        for( u32 j=0; j<bricksPerRow; ++j )
        {
            Brick* b = &app->m_bricks[i*j];

            b->m_position = vec2( x, y );
            b->m_size     = vec2( brickWidth, brickHeight );
            b->m_hit      = false;

            x+= brickWidth;
        }

        y+= brickHeight;
        x = 0.0f;
    }

    app->m_paddle.m_size     = vec2( 100.0f, 30.0f );
    app->m_paddle.m_position = vec2( (CNC_WINDOW_WIDTH / 2.0f) - 50.0f, CNC_WINDOW_HEIGHT - 80.0f );

    x = app->m_paddle.m_position.x;
    y = app->m_paddle.m_position.y;

    app->m_paddleImg = PlatformLoadImage( "res/paddle.png" );
    app->m_paddleImg->m_modelData.m_modelMatrix = translationMatrix( x, y );
    app->m_paddleImg->m_modelData.m_pivotMatrix = identityMatrix();
    app->m_paddleImg->m_textureId = PlatformUploadImage( app->m_renderer, app->m_paddleImg );

    app->m_ballImg  = PlatformLoadImage( "res/ball.png" );
    app->m_ballImg->m_modelData.m_modelMatrix = translationMatrix( x + app->m_paddle.m_size.x, y - 50.0f );
    app->m_ballImg->m_modelData.m_pivotMatrix = identityMatrix();
    app->m_ballImg->m_textureId = PlatformUploadImage( app->m_renderer, app->m_ballImg );
}

void UpdateBreakout( Breakout* app )
{
    if( LeftKey(app->m_input) )  app->m_paddle.m_position.x -= 3.5f;
    if( RightKey(app->m_input) ) app->m_paddle.m_position.x += 3.5f;

    f32 x = app->m_paddle.m_position.x;
    f32 y = app->m_paddle.m_position.y;

    app->m_paddleImg->m_modelData.m_modelMatrix = translationMatrix( x, y );
    PlatformUpdateImage( app->m_renderer, app->m_paddleImg );
}

void RenderBreakout( Breakout* app )
{
    PlatformRenderImage( app->m_renderer, app->m_paddleImg->m_textureId );
    PlatformRenderImage( app->m_renderer, app->m_ballImg->m_textureId );
}
