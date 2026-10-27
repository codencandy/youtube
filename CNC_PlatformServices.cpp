#include "CNC_PlatformServices.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "libs/stb_image.h"
#include "CNC_Tools.h"
#include "CNC_Types.h"
#include "CNC_Math.h"
#include "CNC_Constants.h"
#include "CNC_Application.h"
#include "CNC_TtfFont.cpp"

Image* PlatformLoadImage( const char* imagePath )
{
    Image* image  = (Image*)malloc( sizeof( Image ) );
    image->m_data = stbi_load( imagePath, &image->m_width, &image->m_height, &image->m_channels, 4 );

    return image;
}

File* PlatformLoadFile( const char* filePath )
{
    File* file = (File*)malloc( sizeof( File ) );

    u32 length = StringLength( filePath );
    memcpy( file->m_filename, filePath, length );

    FILE* f = fopen( filePath, "rb" );
    if( f != NULL )
    {
        fseek( f, 0, SEEK_END );
        file->m_sizeInBytes = ftell( f );
        rewind( f );

    
        file->m_data = malloc( file->m_sizeInBytes );
        memset( file->m_data, 0x00, file->m_sizeInBytes );
        fread( file->m_data, 1, file->m_sizeInBytes, f );
        fclose( f );
    }

    return file;
}

TtfFont* PlatformLoadFont( File* ttfFontFile )
{
    TtfFont* font = (TtfFont*)malloc( sizeof( TtfFont ) );

    font->m_fontFile = ttfFontFile;
    ReadTableOffsets( font );

    if( IsTrueType( font ) )
    {
        printf( "file: %s\nTrueType(yes/no):\tyes\n", ttfFontFile->m_filename );
    }
    else
    {
        printf( "file: %s\nTrueType(yes/no):\tno\n", ttfFontFile->m_filename );
        return NULL;
    }

    ReadTables         ( font );
    ReadCmapTable      ( font );
    ReadMaxpTable      ( font );
    ReadHeadTable      ( font );
    ReadLocaTable      ( font );
    ReadHheaTable      ( font );
    ReadGlyphTable     ( font );
    BuildGlyphIdMap    ( font );
    ReadHtmxTable      ( font );
    InsertImpliedPoints( font );

    PrintGlyphData( &font->m_glyphTable.m_glphys[0] );
    Codepoint test = Utf8ToCodepoint( "Ä" );
    u16 glyphId    = GetGlyphId( font, test.m_codepoint );
    printf( "utf8 input:\t%s\n", "Ä" );
    printf( "codepoint:\t%x\n",   test.m_codepoint );
    printf( "num bytes:\t%d\n",   test.m_numUtf8Bytes );
    printf( "glyph id:\t%d\n",    glyphId );

    return font;
}

void PlatformRenderText( void*       renderer, 
                         void*       app,
                         TtfFont*    font, 
                         const char* text, 
                         v2          position, 
                         f32         size )
{
    Application*      application = (Application*)app;
    PlatformServices* services    = application->m_services;
    Timer*            t           = &application->m_timer;
    f32               fontScale   = (f32)(size / font->m_headTable.m_unitsPerEm);
    f32               xOffset     = position.x;
    f32               baseline    = position.y;

    u32 textLength = StringLength( text );
    char* textPointer = (char*)text;
    for( u32 i=0; i<textLength; )
    {
        Codepoint codepoint = Utf8ToCodepoint( textPointer );
        u16       glyphId   = GetGlyphId( font, codepoint.m_codepoint );
        Glyph*    g         = GetGlyph( font, glyphId );
        
        if( codepoint.m_codepoint == '\n' )
        {
            xOffset = position.x;
            baseline += size * 1.2f;
            textPointer += codepoint.m_numUtf8Bytes;
            i           += codepoint.m_numUtf8Bytes;
            continue;
        }
        
        if( g == NULL )
        {
            textPointer += codepoint.m_numUtf8Bytes;
            i           += codepoint.m_numUtf8Bytes;
            continue;
        }
        
        f32 advance = g->m_horMetric.m_advanceWith * fontScale;
        if( !g->m_header.m_emptyGlyph && g->m_header.m_numberOfContours > 0 )
        {
            s16 index1     = 0;
            s16 startIndex = 0;
            v2  p1, p2, p3;

            for( u32 i=0; i<g->m_header.m_numberOfContours; ++i )
            {
                s16 endIndex   = g->m_endPtsOfContours[i];
                s16 startIndex = index1;
                v2  offset     = vec2( xOffset, baseline );
                for( ; index1+2<=endIndex; index1 += 2 )
                {
                    p1 = ((v2)g->m_points[index1]   * fontScale) + offset;
                    p2 = ((v2)g->m_points[index1+1] * fontScale) + offset;
                    p3 = ((v2)g->m_points[index1+2] * fontScale) + offset;
                    
                    f32 d = distance( p1, p3 );
                    f32 t = 0.0f;
                    f32 numBeziers = d / 2.0f;
                    for( u32 b=0; b<numBeziers; ++b )
                    {
                        v2 p = bezier( p1, p2, p3, t );
                        services->f_renderCircle( renderer, p, 1.5f, WHITE );
                        t += 1.0/numBeziers;
                    }
                }

                p1 = ((v2)g->m_points[index1]     * fontScale) + offset;
                p2 = ((v2)g->m_points[index1+1]   * fontScale) + offset;
                p3 = ((v2)g->m_points[startIndex] * fontScale) + offset;
                
                f32 d = distance( p1, p3 );
                f32 t = 0.0f;
                f32 numBeziers = d / 2.0f;
                for( u32 b=0; b<numBeziers; ++b )
                {
                    v2 p = bezier( p1, p2, p3, t );
                    services->f_renderCircle( renderer, p, 1.5f, WHITE );
                    t += 1.0f/numBeziers;
                }
                
                // go to start of next contour
                index1 = endIndex + 1;
            }
        }

        xOffset     += advance;
        textPointer += codepoint.m_numUtf8Bytes;
        i           += codepoint.m_numUtf8Bytes;

        if( xOffset > CNC_WINDOW_WIDTH )
        {
            xOffset = position.x;
            baseline += size * 1.2f;
        }
    }
}

u32 renderIntersections( v2 p1, v2 p2, v2 p3, v2 ray, void* renderer, PlatformServices* services )
{
    f32 dotRadius        = 5.0f;
    f32 t1               = -1.0f;
    f32 t2               = -1.0f;
    u32 numIntersections = 0;

    bezierIntersection( p1, p2, p3, &t1, &t2, ray.y );
    if( t1 >= 0 && t1 < 1.0f )
    {
        v2 p = bezier( p1, p2, p3, t1 );
        if( p.x >= ray.x )
        {
            services->f_renderCircle( renderer, p, dotRadius, YELLOW );
            numIntersections++;
        }
    }

    if( t2 >= 0 && t2 < 1.0f )
    {
        v2 p = bezier( p1, p2, p3, t2 );
        if( p.x >= ray.x )
        {
            services->f_renderCircle( renderer, p, dotRadius, YELLOW );
            numIntersections++;
        }
    }

    return numIntersections;
}

void PlatformRenderTextIntersections( void*       renderer, 
                                      void*       app, 
                                      TtfFont*    font, 
                                      const char* text, 
                                      v2          position, 
                                      f32         size )
{
    static u32 scanline = 0;

    Application*      application = (Application*)app;
    PlatformServices* services    = application->m_services;

    char* textPointer = (char*)text;
    u32   textLength  = StringLength( text );
    u32   width       = CNC_WINDOW_WIDTH;
    u32   height      = CNC_WINDOW_HEIGHT;
    f32   radius      = CNC_WINDOW_WIDTH / 150;
    u32   numSteps    = CNC_WINDOW_HEIGHT / (radius * 2.0);
    u32   step        = 0;
    f32   fontScale   = (f32)(size / font->m_headTable.m_unitsPerEm);

    if( KeyPressed( application->m_input, ARROW_DOWN ) )
    {
        scanline++;
    }
    if( KeyPressed( application->m_input, ARROW_UP ) )
    {
        scanline--;
    }
    step = scanline % numSteps;
    step = step * (radius * 2.0f );
    
    f32   h         = radius;
    color c         = GREY;
    f32   baselineY = 0;
    f32   dotRadius = radius * 1.2f;
    for( ; h<height; h+=(radius*2.0f) )
    {
        if( h >= 0.7*CNC_WINDOW_HEIGHT && h<0.73*CNC_WINDOW_HEIGHT )
        {
            baselineY = h;
            break;
        }
    }

    v2 pos = position;
    pos.y = baselineY;
    PlatformRenderText( renderer, app, font, text, pos, size );

    for( h=radius; h<=CNC_WINDOW_HEIGHT; h+=(radius*2.0f) )
    {
        for( f32 w=radius; w < width; w+=(radius*2.0f) )
        {
            if( h >= 0.7*CNC_WINDOW_HEIGHT && h<0.73*CNC_WINDOW_HEIGHT )
            {
                c = RED;
            }
            else if( (h - step) <= radius && (h-step) > 0 )
            {
                c = LIGHT_BLUE;
            }
            services->f_renderCircle( renderer, vec2( w, h ), radius * 0.6f, c );

            if( (h-step) <= radius && (h-step) > 0)
            {
                u32 advance = 0;
                char* text  = textPointer;
                for( u32 j=0; j<textLength; )
                {
                    Codepoint codepoint = Utf8ToCodepoint( text );
                    u16       glyphId   = GetGlyphId( font, codepoint.m_codepoint );
                    Glyph*    g         = GetGlyph( font, glyphId );

                    u32 numContours      = g->m_header.m_numberOfContours;
                    u32 numIntersections = 0;
                    s16 index1           = 0;
                    v2  p1, p2, p3;

                    f32 t1 = -1.0f;
                    f32 t2 = -1.0f;
                    v2  offset = vec2( position.x + advance, baselineY );
                    v2  ray    = vec2( w, h );

                    for( u32 i=0; i<g->m_header.m_numberOfContours; ++i )
                    {
                        s16 endIndex   = g->m_endPtsOfContours[i];
                        s16 startIndex = index1;
                        for( ; index1+2<=endIndex; index1 += 2 )
                        {
                            p1 = ((v2)g->m_points[index1]   * fontScale) + offset;
                            p2 = ((v2)g->m_points[index1+1] * fontScale) + offset;
                            p3 = ((v2)g->m_points[index1+2] * fontScale) + offset;
                            
                            numIntersections += renderIntersections( p1, p2, p3, ray, renderer, services );
                        }

                        p1 = ((v2)g->m_points[index1]     * fontScale) + offset;
                        p2 = ((v2)g->m_points[index1+1]   * fontScale) + offset;
                        p3 = ((v2)g->m_points[startIndex] * fontScale) + offset;
                        
                        numIntersections += renderIntersections( p1, p2, p3, ray, renderer, services );

                        // go to start of next contour
                        index1 = endIndex + 1;  
                    }

                    if( (numIntersections % 2) != 0 )
                    {
                        services->f_renderCircle( renderer, vec2( w, h ), 5.0f, YELLOW );
                    }
                    
                    text        += codepoint.m_numUtf8Bytes;
                    j           += codepoint.m_numUtf8Bytes;
                    advance     += (g->m_horMetric.m_advanceWith * fontScale);
                }
            }
        }
        c= GREY;
    }
}

/* implemented in the Renderer 

    u32  PlatformUploadImage( void* renderer, Image* image );
    void PlatformUploadParticles( void* renderer, Particle* particles, u32 numParticles );
    void PlatformRenderImage( void* renderer, u32 textureId );
    void PlatformRenderParticles( void* renderer, u32 numParticles, u32 snowMask, u32 skyMask );
    void PlatformUpdateImage( void* renderer, Image* image );

 */

PlatformServices* CreatePlatformServices()
{
    PlatformServices* services = (PlatformServices*)malloc( sizeof( PlatformServices ) );

    services->f_loadImage               = &PlatformLoadImage;
    services->f_loadFile                = &PlatformLoadFile;
    services->f_loadFont                = &PlatformLoadFont;
    services->f_uploadImage             = &PlatformUploadImage;
    services->f_uploadParticles         = &PlatformUploadParticles;
    services->f_renderImage             = &PlatformRenderImage;
    services->f_renderParticles         = &PlatformRenderParticles;
    services->f_updateImage             = &PlatformUpdateImage;
    services->f_renderRect              = &PlatformRenderRect;
    services->f_renderCircle            = &PlatformRenderCircle;
    services->f_renderLine              = &PlatformRenderLine;
    services->f_renderText              = &PlatformRenderText;
    services->f_renderTextIntersections = &PlatformRenderTextIntersections;

    return services;
}