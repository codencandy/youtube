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

TtfFont* PlatformLoadFont( File* ttfFontFile, void* renderer )
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

    u32       numGlyphs   = font->m_glyphTable.m_numGlyphs;
    u32       numPoints   = 0;
    u32       numContours = 0;
    GpuGlyph* gpuGlyphs   = (GpuGlyph*)malloc( sizeof( GpuGlyph ) * numGlyphs );

    for( u32 i=0; i<numGlyphs; ++i )
    {
        Glyph*    g   = &font->m_glyphTable.m_glyphs[i];
        GpuGlyph* gpu = &gpuGlyphs[i];

        gpu->m_numPoints        = g->m_realNumPoints;
        gpu->m_pointIndex       = numPoints;
        numPoints              += g->m_realNumPoints;

        gpu->m_numContours      = g->m_header.m_numberOfContours;
        gpu->m_contourIndex     = numContours;
        numContours            += gpu->m_numContours;
    }

    v2*  points   = (v2*)malloc( sizeof( v2 ) * numPoints );
    u32* contours = (u32*)malloc( sizeof( u32 ) * numContours );

    u32 pointIndex   = 0;
    u32 contourIndex = 0;
    for( u32 i=0; i<numGlyphs; ++i )
    {
        Glyph*    g   = &font->m_glyphTable.m_glyphs[i];
        GpuGlyph* gpu = &gpuGlyphs[i];

        u32 gNumPoints   = gpu->m_numPoints;
        u32 gNumContours = g->m_header.m_numberOfContours;

        for( u32 j=0; j<gNumPoints; ++j )
        {
            points[pointIndex] = g->m_points[j];
            pointIndex++;
        }

        for( u32 k=0; k<gNumContours; ++k )
        {
            contours[contourIndex] = g->m_endPtsOfContours[k];
            contourIndex++;
        }
    }

    PlatformUploadGlyphs( renderer, gpuGlyphs, numGlyphs, points, numPoints, contours, numContours );
    
    free( gpuGlyphs );
    free( points );
    free( contours );

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
    BoundingBox boxes[textLength];
    u32 numBoxes = 0;

    char* textPointer = (char*)text;
    for( u32 i=0; i<textLength; )
    {
        Codepoint codepoint = Utf8ToCodepoint( textPointer );
        u16       glyphId   = GetGlyphId( font, codepoint.m_codepoint );
        Glyph*    g         = GetGlyph( font, glyphId );
        
        if( codepoint.m_codepoint == '\n' )
        {
            xOffset = position.x;
            f32 lineAdvance = ( font->m_hheaTable.m_ascender - 
                                font->m_hheaTable.m_descender + font->m_hheaTable.m_lineGap) * 
                                fontScale;
            baseline += lineAdvance;
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
            v2 boxOrigin   = vec2( g->m_header.m_xMin * fontScale, g->m_header.m_yMin * fontScale);
            v2 boxSize     = vec2( (f32)(g->m_header.m_xMax - g->m_header.m_xMin) * fontScale, (f32)(g->m_header.m_yMax - g->m_header.m_yMin) * fontScale );
            v2 boxPosition = vec2( boxOrigin.x + xOffset, baseline - boxOrigin.y - boxSize.y );
            
            boxes[numBoxes].m_position    = boxPosition;
            boxes[numBoxes].m_size        = boxSize;
            boxes[numBoxes].m_glyphOrigin = vec2( (f32)g->m_header.m_xMin,
                                                  -(f32)g->m_header.m_yMax );
            boxes[numBoxes].m_glyphId     = glyphId;
            boxes[numBoxes].m_fontScale   = fontScale;
            ++numBoxes;
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

    if( numBoxes > 0 )
    {
        PlatformUploadBoxes( renderer, boxes, numBoxes );
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
    services->f_uploadGlyphs            = &PlatformUploadGlyphs;
    services->f_uploadBoxes             = &PlatformUploadBoxes;
    services->f_renderText              = &PlatformRenderText;

    return services;
}
