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
    void PlatformUploadParticles( void* renderer, Particle* particles, u32 numParticles );
    void PlatformRenderImage( void* renderer, u32 textureId );
    void PlatformRenderParticles( void* renderer, u32 numParticles, u32 snowMask, u32 skyMask );
    void PlatformUpdateImage( void* renderer, Image* image );

 */

PlatformServices* CreatePlatformServices()
{
    PlatformServices* services = (PlatformServices*)malloc( sizeof( PlatformServices ) );

    services->f_loadImage       = &PlatformLoadImage;
    services->f_uploadImage     = &PlatformUploadImage;
    services->f_uploadParticles = &PlatformUploadParticles;
    services->f_renderImage     = &PlatformRenderImage;
    services->f_renderParticles = &PlatformRenderParticles;
    services->f_updateImage     = &PlatformUpdateImage;

    return services;
}