#ifndef CNC_PLATFORMSERVICES_H
#define CNC_PLATFORMSERVICES_H

#include "CNC_Types.h"
#include "CNC_TtfTypes.h"

Image* PlatformLoadImage( const char* imagePath );
File*  PlatformLoadFile( const char* filePath );
Font*  PlatformLoadFont( File* ttfFontFile );

void   PlatformUploadParticles( void* renderer, Particle* particles, u32 numParticles );
void   PlatformRenderParticles( void* renderer, u32 numParticles, u32 snowMask, u32 skyMask );

u32    PlatformUploadImage( void* renderer, Image* image );
void   PlatformRenderImage( void* renderer, u32 textureId, u32 numInstances = 1 );
void   PlatformUpdateImage( void* renderer, Image* image );

void   PlatformRenderRect(   void* renderer, v2 pos, f32 width, f32 height, color c );
void   PlatformRenderCircle( void* renderer, v2 center, f32 radius, color c );
void   PlatformRenderLine(   void* renderer, v2 start, v2 end, f32 width, color c );

typedef struct PlatformServices
{
    Image*(*f_loadImage)(const char*);
    File* (*f_loadFile)(const char*);
    Font* (*f_loadFont)(File*);
    u32   (*f_uploadImage)(void*, Image*);
    void  (*f_uploadParticles)(void*, Particle*, u32);
    void  (*f_renderImage)(void*, u32, u32 );
    void  (*f_renderParticles)(void*, u32, u32 ,u32);
    void  (*f_updateImage)(void*, Image*);
    void  (*f_renderRect)(void*, v2, f32, f32, color);
    void  (*f_renderCircle)(void*, v2, f32, color );
    void  (*f_renderLine)(void*, v2, v2, f32, color);

} PlatformServices;

PlatformServices* CreatePlatformServices();

#endif//CNC_PLATFORMSERVICES_H