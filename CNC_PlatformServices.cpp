#include "CNC_PlatformServices.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "libs/stb_image.h"
#include "CNC_Tools.h"
#include "CNC_Types.h"
#include "CNC_Constants.h"
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

    ReadTables     ( font );
    ReadCmapTable  ( font );
    ReadMaxpTable  ( font );
    ReadHeadTable  ( font );
    ReadLocaTable  ( font );
    ReadGlyphTable ( font );
    BuildGlyphIdMap( font );

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
                         void*       s,
                         TtfFont*    font, 
                         const char* text, 
                         v2          position, 
                         f32         size )
{
    PlatformServices* services = (PlatformServices*)s;
    f32    fontScale = (f32)(size / font->m_headTable.m_unitsPerEm);
    f32    xOffset   = position.x;
    f32    yOffset   = position.y;

    u32 textLength = StringLength( text );
    char* textPointer = (char*)text;
    for( u32 i=0; i<textLength; )
    {
        Codepoint codepoint = Utf8ToCodepoint( textPointer );
        u16       glyphId   = GetGlyphId( font, codepoint.m_codepoint );
        Glyph*    g         = GetGlyph( font, glyphId );

        if( xOffset > CNC_WINDOW_WIDTH )
        {
            xOffset = position.x;
            yOffset += size;
        }
        
        if( g != NULL )
        {
            for( u32 i=0; i<g->m_numPoints; ++i )
            {
                services->f_renderCircle( renderer, vec2( g->m_x[i] * fontScale + xOffset, g->m_y[i] * fontScale + yOffset ), 3.0f, rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );
            }
            xOffset += g->m_header.m_xMax * fontScale;
        }
    
        textPointer += codepoint.m_numUtf8Bytes;
        i           += codepoint.m_numUtf8Bytes;
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

    services->f_loadImage       = &PlatformLoadImage;
    services->f_loadFile        = &PlatformLoadFile;
    services->f_loadFont        = &PlatformLoadFont;
    services->f_uploadImage     = &PlatformUploadImage;
    services->f_uploadParticles = &PlatformUploadParticles;
    services->f_renderImage     = &PlatformRenderImage;
    services->f_renderParticles = &PlatformRenderParticles;
    services->f_updateImage     = &PlatformUpdateImage;
    services->f_renderRect      = &PlatformRenderRect;
    services->f_renderCircle    = &PlatformRenderCircle;
    services->f_renderLine      = &PlatformRenderLine;
    services->f_renderText      = &PlatformRenderText;

    return services;
}