#ifndef CNC_PLATFORMSERVICES_H
#define CNC_PLATFORMSERVICES_H

#include "CNC_Types.h"

Image* PlatformLoadImage( const char* imagePath );
u32    PlatformUploadImage( void* renderer, Image* image );
void   PlatformRenderImage( void* renderer, u32 textureId, u32 numInstances = 1, render_type type = CNC_IMAGE );
void   PlatformUpdateImage( void* renderer, Image* image );
void   PlatformRenderParticles( void* renderer, u32 maskId );

#endif//CNC_PLATFORMSERVICES_H