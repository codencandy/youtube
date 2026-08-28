#ifndef CNC_TOOLS_H
#define CNC_TOOLS_H

#include "CNC_Types.h"

u32 BigToLittle( u32 bigEndian );
u16 BigToLittle( u16 bigEndian );

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

u16 BigToLittle( u16 bigEndian )
{
    u16 little = 0x0;

    little = ((bigEndian & 0xff00) >> 8 ) |
             ((bigEndian & 0x00ff) << 8 ); 

    return little;
}

#endif//CNC_TOOLS_H
