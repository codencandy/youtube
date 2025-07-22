#ifndef CNC_PLATFORMSERVICES_H
#define CNC_PLATFORMSERVICES_H

#include "CNC_Types.h"

Image* PlatformLoadImage( const char* imagePath );
u32    PlatformUploadImage( void* renderer, Image* image );
void   PlatformUploadParticles( void* renderer, Particle* particles, u32 numParticles );
void   PlatformRenderImage( void* renderer, u32 textureId, u32 numInstances = 1 );
void   PlatformRenderParticles( void* renderer, u32 numParticles, u32 snowMask, u32 skyMask );
void   PlatformUpdateImage( void* renderer, Image* image );

typedef struct PlatformServices
{
    Image*(*f_loadImage)(const char*);
    u32   (*f_uploadImage)(void*, Image*);
    void  (*f_uploadParticles)(void*, Particle*, u32);
    void  (*f_renderImage)(void*, u32, u32 );
    void  (*f_renderParticles)(void*, u32, u32 ,u32);
    void  (*f_updateImage)(void*, Image*);

} PlatformServices;

PlatformServices* CreatePlatformServices();

#endif//CNC_PLATFORMSERVICES_H