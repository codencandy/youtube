#ifndef CNC_PLATFORMSERVICES_H
#define CNC_PLATFORMSERVICES_H

#include "CNC_Types.h"

Image*    PlatformLoadImage( const char* imagePath );
File*     PlatformLoadFile( const char* filePath, void* renderer );
TtfFont*  PlatformLoadFont( File* ttfFontFile );

void   PlatformUploadParticles( void* renderer, Particle* particles, u32 numParticles );
u32    PlatformUploadImage( void* renderer, Image* image );
void   PlatformUpdateImage( void* renderer, Image* image );
u32    PlatformUploadGlyphs( void* renderer, GpuGlyph* glyphs, u32 numGlphys, v2* points, u32 numPoints, u32* contours, u32 numContours );
void   PlatformUploadBoxes( void* renderer, BoundingBox* boxes, u32 numBoxes, u32 fontId, color c );

void   PlatformRenderParticles( void* renderer, u32 numParticles, u32 snowMask, u32 skyMask );
void   PlatformRenderImage( void* renderer, u32 textureId, u32 numInstances = 1 );
void   PlatformRenderText ( void* renderer, void* app, TtfFont* font, const char* text, v2 position, f32 size, color c );
void   PlatformRenderRect( void*renderer, v2 pos, f32 width, f32 height, color c);
void   PlatformRoundedRect( void* renderer, v2 pos, f32 width, f32 height, f32 radius, color c );
void   PlatformRenderCircle( void* renderer, v2 center, f32 radius, color c );
void   PlatformRenderLine(   void* renderer, v2 start, v2 end, f32 width, color c );

void   PlatformSetWindowProperties( void* renderer, WindowProperties* properties );
void   PlatformSetWindowSize( void* renderer, v2int size );
// Current content size in logical points.
v2     PlatformGetWindowSize( void* renderer );

typedef struct PlatformServices
{
    Image*   (*f_loadImage)(const char*);
    File*    (*f_loadFile)(const char*);
    TtfFont* (*f_loadFont)(File*, void*);
    u32      (*f_uploadImage)(void*, Image*);
    void     (*f_uploadParticles)(void*, Particle*, u32);
    void     (*f_renderImage)(void*, u32, u32 );
    void     (*f_renderParticles)(void*, u32, u32 ,u32);
    void     (*f_updateImage)(void*, Image*);
    void     (*f_renderRect)(void*, v2, f32, f32, color);
    void     (*f_roundedRect)(void*, v2, f32, f32, f32, color);
    void     (*f_renderCircle)(void*, v2, f32, color );
    void     (*f_renderLine)(void*, v2, v2, f32, color);
    u32      (*f_uploadGlyphs)(void*, GpuGlyph*, u32, v2*, u32, u32*, u32);
    void     (*f_uploadBoxes)(void*, BoundingBox*, u32, u32, color);
    void     (*f_renderText)(void*, void*, TtfFont*, const char*, v2, f32, color );
    void     (*f_windowProperties)(void*,WindowProperties*);
    void     (*f_setWindowSize)(void*, v2int);

} PlatformServices;

PlatformServices* CreatePlatformServices();

#endif//CNC_PLATFORMSERVICES_H
