#ifndef CNC_TOOLS_H
#define CNC_TOOLS_H

#include "CNC_Types.h"

u32 BigToLittle( u32 bigEndian );
u32 BigToLittleU32( void* stream, u32 offset );
u16 BigToLittle( u16 bigEndian );
u16 BigToLittleU16( void* stream, u32 offset );
u32 StringLength( const char* string );

/*****************************
 * IMPLEMENTATION
 *****************************/
u32 BigToLittle( u32 bigEndian )
{
    u32 little = 0x0;

    little = ((bigEndian & 0xff000000) >> 24) |
             ((bigEndian & 0x00ff0000) >>  8) |
             ((bigEndian & 0x0000ff00) <<  8) |
             ((bigEndian & 0x000000ff) << 24);

    return little;
}

u32 BigToLittleU32( void* stream, u32 offset )
{
    u8* data = (u8*)stream + offset;
    
    u32 bigEndian = *((u32*)data);
    u32 little = 0x0;

    little = ((bigEndian & 0xff000000) >> 24) |
             ((bigEndian & 0x00ff0000) >>  8) |
             ((bigEndian & 0x0000ff00) <<  8) |
             ((bigEndian & 0x000000ff) << 24);

    return little;
}

u16 BigToLittle( u16 bigEndian )
{
    u16 little = 0x0;

    little = ((bigEndian & 0xff00) >> 8 ) |
             ((bigEndian & 0x00ff) << 8 ); 

    return little;
}

u16 BigToLittleU16( void* stream, u32 offset )
{
    u8* data = (u8*)stream + offset;
    
    u16 bigEndian = *((u16*)data);
    u16 little = 0x0;

    little = ((bigEndian & 0xff00) >> 8 ) |
             ((bigEndian & 0x00ff) << 8 ); 

    return little;
}

u32 StringLength( const char* string )
{
    char terminator = 0x0;
    u32  length     = 0;
    u32  index      = 0;
    
    while( string[index] != terminator )
    {
        length++;
        index++;
    }

    return length;
}

#endif//CNC_TOOLS_H
