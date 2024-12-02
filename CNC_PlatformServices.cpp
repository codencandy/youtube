#include "CNC_PlatformServices.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "libs/stb_image.h"

Image* PlatformLoadImage( const char* imagePath )
{
    Image* image = (Image*)malloc( sizeof( Image ) );

    image->m_data = stbi_load( imagePath, &image->m_width, &image->m_height, &image->m_channels, 4 );

    return image;
}

/* implemented in the Renderer 

    u32  PlatformUploadImage( void* renderer, Image* image );
    void PlatformRenderImage( void* renderer, u32 textureId );

 */