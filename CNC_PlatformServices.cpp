#include "CNC_PlatformServices.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "libs/stb_image.h"
#include "CNC_Tools.h"

Image* PlatformLoadImage( const char* imagePath )
{
    Image* image  = (Image*)malloc( sizeof( Image ) );
    image->m_data = stbi_load( imagePath, &image->m_width, &image->m_height, &image->m_channels, 4 );

    return image;
}

File* PlatformLoadFile( const char* filePath )
{
    File* file = (File*)malloc( sizeof( File ) );
    FILE* f    = fopen( filePath, "rb" );

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

Font* PlatformLoadFont( File* ttfFontFile )
{
    Font* font = (Font*)malloc( sizeof( Font ) );

    // read the offset table
    // the offset tabel is 12 bytes so no zero bytes 
    // are appending for padding to 4 byte boundaries
    TtfOffsetTable offsetTable;
    memcpy( &offsetTable, ttfFontFile->m_data, sizeof( TtfOffsetTable ) );

    char otto[5] = "OTTO";
    if( memcmp( &otto, &offsetTable, 4 ) == 0 )
    {
        printf( "OpenType Font\n" );
    }
    else
    {
        offsetTable.m_sfntVersion   = BigToLittle( offsetTable.m_sfntVersion );
        printf( "TrueType Font\n" );
    }

    offsetTable.m_numTables     = BigToLittle( offsetTable.m_numTables );
    offsetTable.m_searchRange   = BigToLittle( offsetTable.m_searchRange );
    offsetTable.m_entrySelector = BigToLittle( offsetTable.m_entrySelector );
    offsetTable.m_rangeShift    = BigToLittle( offsetTable.m_rangeShift );

    // read the table entries
    font->m_numTables = offsetTable.m_numTables;
    font->m_tableEntries = (TableEntry*)malloc( sizeof( TableEntry ) * font->m_numTables );

    TableEntry* tablememory = (TableEntry*)((u8*)ttfFontFile->m_data + sizeof( TtfOffsetTable ));
    for( u32 i=0; i<font->m_numTables; ++i )
    {
        TableEntry* entry = &font->m_tableEntries[i];
        memcpy( entry, tablememory, sizeof( TableEntry ) );

        //strings are not big endian encoded 
        //entry->m_tag      = BigToLittle( entry->m_tag );
        entry->m_checksum = BigToLittle( entry->m_checksum );
        entry->m_offset   = BigToLittle( entry->m_offset );
        entry->m_length   = BigToLittle( entry->m_length );

        char tag[5];
        memset( &tag, 0x0, 5 );
        memcpy( &tag, &entry->m_tag, 4 );

        printf( "tag: %s\n", (char*)&tag );
        printf( "offset: %d\n", entry->m_offset );
        printf( "length: %d\n\n", entry->m_length );

        tablememory++;
    }
    return font;
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

    return services;
}