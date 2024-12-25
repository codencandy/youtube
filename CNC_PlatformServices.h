#ifndef CNC_PLATFORMSERVICES_H
#define CNC_PLATFORMSERVICES_H

#include "CNC_Types.h"

Image* PlatformLoadImage( const char* imagePath );
u32    PlatformUploadImage( void* renderer, Image* image );
void   PlatformUploadParticles( void* renderer, Particle* particles, u32 numParticles );
void   PlatformRenderImage( void* renderer, u32 textureId, u32 numInstances = 1 );
void   PlatformRenderParticles( void* renderer, u32 numParticles, u32 snowMask, u32 skyMask );
void   PlatformUpdateImage( void* renderer, Image* image );

#endif//CNC_PLATFORMSERVICES_H