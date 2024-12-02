#ifndef CNC_PLATFORMSERVICES_H
#define CNC_PLATFORMSERVICES_H

#include "CNC_Types.h"

Image* PlatformLoadImage( const char* imagePath );
u32    PlatformUploadImage( void* renderer, Image* image );
void   PlatformRenderImage( void* renderer, u32 textureId );

#endif//CNC_PLATFORMSERVICES_H