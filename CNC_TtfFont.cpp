#include "CNC_Types.h"
#include "CNC_TtfTypes.h"
#include "CNC_Tools.h"
#include "CNC_Math.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

bool       IsTrueType( TtfFont* font );
void       ReadTableOffsets( TtfFont* font );
bool       GetPointFlag( u8 flag, point_flag bit );
void*      DecodePoints( Glyph* g, void* data, s16* dest, u32 numPoints, bool x );
void       InsertImpliedPoints( TtfFont* font );
bool       ResolveCompoundGlyphs( TtfFont* font );
void       ReadTables( TtfFont* font );
u32        GetTableOffset( TtfFont* font, const char* tag );
void       PrintGlyphData( Glyph* g );
Codepoint  Utf8ToCodepoint( const char* utf8 );
u16        CodepointToGlyphId( TtfFont* font, u16 codepoint );
void       BuildGlyphIdMap( TtfFont* font );
u16        GetGlyphId( TtfFont* font, u16 codepoint );
Glyph*     GetGlyph( TtfFont* font, u16 glyphId );

void ReadCmapTable ( TtfFont* font );
void ReadHeadTable ( TtfFont* font );
void ReadMaxpTable ( TtfFont* font );
void ReadLocaTable ( TtfFont* font );
void ReadGlyphTable( TtfFont* font );

/******************************
 * Implementation
 ******************************/
bool IsTrueType( TtfFont* font )
{
    char otto[5] = "OTTO";
    if( memcmp( &otto, &font->m_offsets, 4 ) == 0 )
    {
        return false;
    }
    else
    {
        font->m_offsets.m_sfntVersion   = BigToLittle( font->m_offsets.m_sfntVersion );
        return true;
    }
}

void ReadTableOffsets( TtfFont* font )
{
    // read the offset table
    // the offset tabel is 12 bytes so no zero bytes 
    // are appending for padding to 4 byte boundaries
    TableOffsets offsetTable;
    memcpy( &offsetTable, font->m_fontFile->m_data, sizeof( TableOffsets ) );

    font->m_offsets = offsetTable;    

    offsetTable.m_numTables     = BigToLittle( offsetTable.m_numTables );
    offsetTable.m_searchRange   = BigToLittle( offsetTable.m_searchRange );
    offsetTable.m_entrySelector = BigToLittle( offsetTable.m_entrySelector );
    offsetTable.m_rangeShift    = BigToLittle( offsetTable.m_rangeShift );

    // read the table entries
    font->m_numTables = offsetTable.m_numTables;
    font->m_tableEntries = (TableEntry*)malloc( sizeof( TableEntry ) * font->m_numTables );
}

bool GetPointFlag( u8 flag, point_flag bit )
{
    if( (flag & bit) == bit ) 
    {
        return true;
    }
    
    return false;
}

bool GetCompoundFlag( u16 flag, compound_flag bit )
{
    if( (flag & bit) == bit )
    {
        return true;
    }

    return false;
}

static f32 F2Dot14ToF32( s16 value )
{
    return (f32)value / 16384.0f;
}

static bool AppendCompoundComponent( Glyph* destination,
                                     Glyph* component,
                                     CompoundComponent* description )
{
    u32 oldNumPoints   = destination->m_numPoints;
    u32 oldNumContours = (u32)destination->m_header.m_numberOfContours;
    u32 newNumPoints   = oldNumPoints + component->m_numPoints;
    u32 newNumContours = oldNumContours + (u32)component->m_header.m_numberOfContours;

    s16* newX = (s16*)realloc( destination->m_x, sizeof( s16 ) * newNumPoints );
    if( newX == NULL && newNumPoints > 0 ) return false;
    destination->m_x = newX;

    s16* newY = (s16*)realloc( destination->m_y, sizeof( s16 ) * newNumPoints );
    if( newY == NULL && newNumPoints > 0 ) return false;
    destination->m_y = newY;

    bool* newOnCurve = (bool*)realloc( destination->m_onCurve,
                                       sizeof( bool ) * newNumPoints );
    if( newOnCurve == NULL && newNumPoints > 0 ) return false;
    destination->m_onCurve = newOnCurve;

    u8* newFlags = (u8*)realloc( destination->m_flags, sizeof( u8 ) * newNumPoints );
    if( newFlags == NULL && newNumPoints > 0 ) return false;
    destination->m_flags = newFlags;

    u16* newEndpoints = (u16*)realloc( destination->m_endPtsOfContours,
                                       sizeof( u16 ) * newNumContours );
    if( newEndpoints == NULL && newNumContours > 0 ) return false;
    destination->m_endPtsOfContours = newEndpoints;

    f32 dx = 0.0f;
    f32 dy = 0.0f;
    if( GetCompoundFlag( description->m_flags, ARGS_ARE_XY_VALUES ) )
    {
        dx = (f32)description->m_arg1;
        dy = (f32)description->m_arg2;

        if( GetCompoundFlag( description->m_flags, SCALED_COMPONENT_OFFSET ) &&
           !GetCompoundFlag( description->m_flags, UNSCALED_COMPONENT_OFFSET ) )
        {
            f32 transformedDx = description->m_xx * dx + description->m_xy * dy;
            f32 transformedDy = description->m_yx * dx + description->m_yy * dy;
            dx = transformedDx;
            dy = transformedDy;
        }
    }
    else
    {
        u32 parentPoint    = (u32)description->m_arg1;
        u32 componentPoint = (u32)description->m_arg2;
        if( parentPoint >= oldNumPoints || componentPoint >= component->m_numPoints )
            return false;

        f32 componentX = description->m_xx * component->m_x[componentPoint] +
                         description->m_xy * component->m_y[componentPoint];
        f32 componentY = description->m_yx * component->m_x[componentPoint] +
                         description->m_yy * component->m_y[componentPoint];
        dx = destination->m_x[parentPoint] - componentX;
        dy = destination->m_y[parentPoint] - componentY;
    }

    if( GetCompoundFlag( description->m_flags, ROUND_XY_TO_GRID ) )
    {
        dx = roundf( dx );
        dy = roundf( dy );
    }

    for( u32 i=0; i<component->m_numPoints; ++i )
    {
        f32 x = description->m_xx * component->m_x[i] +
                description->m_xy * component->m_y[i] + dx;
        f32 y = description->m_yx * component->m_x[i] +
                description->m_yy * component->m_y[i] + dy;

        destination->m_x[oldNumPoints + i]       = (s16)roundf( x );
        destination->m_y[oldNumPoints + i]       = (s16)roundf( y );
        destination->m_onCurve[oldNumPoints + i] = component->m_onCurve[i];
        destination->m_flags[oldNumPoints + i]   = component->m_onCurve[i] ? ON_CURVE : 0;
    }

    for( u32 i=0; i<(u32)component->m_header.m_numberOfContours; ++i )
    {
        destination->m_endPtsOfContours[oldNumContours + i] =
            (u16)(oldNumPoints + component->m_endPtsOfContours[i]);
    }

    destination->m_numPoints = newNumPoints;
    destination->m_header.m_numberOfContours = (s16)newNumContours;
    return true;
}

static bool ResolveCompoundGlyph( TtfFont* font, u32 glyphId, u8* state, u32 depth )
{
    if( glyphId >= font->m_glyphTable.m_numGlyphs ) return false;
    if( state[glyphId] == 2 ) return true;
    if( state[glyphId] == 1 ) return false;
    if( depth > font->m_maxpTable.m_maxComponentDepth + 1 ) return false;

    Glyph* glyph = &font->m_glyphTable.m_glphys[glyphId];
    if( glyph->m_header.m_numberOfContours >= 0 )
    {
        state[glyphId] = 2;
        return true;
    }

    state[glyphId] = 1;
    glyph->m_header.m_numberOfContours = 0;

    for( u32 i=0; i<glyph->m_numComponents; ++i )
    {
        CompoundComponent* description = &glyph->m_components[i];
        if( !ResolveCompoundGlyph( font, description->m_glyphId, state, depth + 1 ) )
            return false;

        Glyph* component = &font->m_glyphTable.m_glphys[description->m_glyphId];
        if( !AppendCompoundComponent( glyph, component, description ) )
            return false;
    }

    state[glyphId] = 2;
    return true;
}

bool ResolveCompoundGlyphs( TtfFont* font )
{
    u32 numGlyphs = font->m_glyphTable.m_numGlyphs;
    u8* state = (u8*)calloc( numGlyphs, sizeof( u8 ) );
    if( state == NULL ) return false;

    bool result = true;
    for( u32 glyphId=0; glyphId<numGlyphs; ++glyphId )
    {
        if( !ResolveCompoundGlyph( font, glyphId, state, 0 ) )
        {
            printf( "failed to resolve compound glyph ID: %d\n", glyphId );
            result = false;
            break;
        }
    }

    free( state );
    return result;
}

void* DecodePoints( Glyph* g, void* data, s16* dest, u32 numPoints, bool x )
{
    s16 p = 0;

    for( u32 i=0; i<numPoints; ++i )
    {
        u8 flag = g->m_flags[i];
        
        bool shortBit = false;
        bool sameBit  = false;
        
        if( x )
        {
            shortBit = GetPointFlag( flag, X_SHORT_1BYTE );
            sameBit  = GetPointFlag( flag, X_SAME_OR_POS );
        }
        else
        {
            shortBit = GetPointFlag( flag, Y_SHORT_1BYTE );
            sameBit  = GetPointFlag( flag, Y_SAME_OR_POS );
        }

        if( shortBit && sameBit )
        {
            // read one byte - positive
            u8 value = (u8)*((u8*)data);
            p += value;
            dest[i] = (s16)p;
            data = (u8*)data + 1;
        }
        if( shortBit && !sameBit )
        {
            // read one byte - negative
            s16 value = (u8)*(u8*)data;
            p += -value;
            dest[i] = p;
            data = (u8*)data + 1;
        }
        if( !shortBit && sameBit )
        {
            // value is zero - read no bytes
            dest[i] = p;
        }
        if( !shortBit && !sameBit )
        {
            // read signed big-endian - s16
            s16 value = (s16)BigToLittleU16( data, 0 );
            p += value;
            dest[i] = p;
            data = (u8*)data + 2;
        }
    }

    return data;
}

void InsertImpliedPoints( TtfFont* font )
{
    for( u32 i=0; i<font->m_glyphTable.m_numGlyphs; ++i )
    {
        Glyph*g = &font->m_glyphTable.m_glphys[i];
        
        if( g->m_header.m_emptyGlyph || g->m_header.m_numberOfContours <= 0 )
        {
            continue;
        }

        s16 numContours = g->m_header.m_numberOfContours;
        u32 realNumPoints = g->m_numPoints;
        u32 rawIndex = 0;
        for( u32 c=0; c<(u32)numContours; ++c )
        {
            u32 start = rawIndex;
            u32 end = g->m_endPtsOfContours[c];
            for( ; rawIndex<=end; ++rawIndex )
            {
                u32 next = rawIndex == end ? start : rawIndex + 1;
                if( g->m_onCurve[rawIndex] == g->m_onCurve[next] )
                    realNumPoints++;
            }
        }

        g->m_realNumPoints = realNumPoints;
        g->m_points = (v2int*)malloc( sizeof( v2int ) * realNumPoints );

        u32 pointIndex  = 0;
        u32 index       = 0;
        for( u32 c=0; c<numContours; ++c )
        {
            u16 end          = g->m_endPtsOfContours[c];
            u32 startIndex   = 0;
            bool onCurve     = false;
            bool nextOnCurve = false;

            startIndex = index;
            v2int p1;
            v2int p2;
            for( ; index<=end; ++index )
            {
                p1 = vec2( g->m_x[index], g->m_y[index] );

                if( index == end )
                {
                    onCurve     = g->m_onCurve[index];
                    nextOnCurve = g->m_onCurve[startIndex];
                    p2 = vec2( g->m_x[startIndex], g->m_y[startIndex] );
                }
                else
                {
                    onCurve     = g->m_onCurve[index];
                    nextOnCurve = g->m_onCurve[index + 1];
                    p2 = vec2( g->m_x[index+1], g->m_y[index+1] );
                }

                g->m_points[pointIndex++] = p1;

                if( onCurve == nextOnCurve )
                {
                    g->m_points[pointIndex++] = halfwayPoint( p1, p2 );
                }

                if( index == end )
                {
                    g->m_endPtsOfContours[c] = pointIndex - 1;
                }
            }
        }
    }

    printf( "inserting implied points\n------------------\n" );
    printf( "all contours contain only valid bezier path with 3 points\n" );
    printf( "\n" ) ;
}

void PrintGlyphData( Glyph* g )
{
    printf( "glyph data\n---------------\n" );
    printf( "num contours:\t%d\n", g->m_header.m_numberOfContours );
    printf( "num points:\t%d\n",   g->m_numPoints );

    s16 index1 = 0;
    s16 index2 = 0;
    for( u32 i=0; i<g->m_header.m_numberOfContours; ++i )
    {
        index2 = g->m_endPtsOfContours[i];
        printf( "contour %d:\t[%d .. %d]\n", i, index1, index2 );
        index1 = index2 + 1;
    }
    
    for( u32 i=0; i<g->m_numPoints; ++i )
    {
        printf( "point %d:\t%d | %d \t ", i, g->m_x[i], g->m_y[i] );
        if( g->m_onCurve[i] ) 
            printf( "(on curve)\n" );
        else
            printf( "(not on curve)\n" );
    }
}

Codepoint Utf8ToCodepoint( const char* utf8 )
{
    Codepoint c = {0};

    u8 byte0      = (u8)utf8[0];
    u8 byte1      = 0;
    u8 bitMask    = 0xE0; // 1110 0000
    u8 utf82bytes = 0xC0; // 1100 0000

    if( (byte0 & bitMask) == utf82bytes ) // 0xxxx xxxx -> 1 byte utf8
    {
        byte1 = (u8)utf8[1];

        u8 byte0payload = byte0 & 0x1F; // 000X XXXX;
        u8 byte1payload = byte1 & 0x3F; // 00XX XXXX;

        c.m_codepoint    = (u32)(byte0payload << 6) | (u32)(byte1payload);
        c.m_numUtf8Bytes = 2;
    }
    else if( (byte0 & 0x80) == 0 )
    {
        c.m_codepoint    = (u32)byte0;
        c.m_numUtf8Bytes = 1;
    }
    else
    {
        c.m_codepoint    = 0x00; // glyph id for .undef
        c.m_numUtf8Bytes = 0;
    }

    return c;
}

u16 CodepointToGlyphId( TtfFont* font, u16 codepoint )
{
    u16 glyphId = 0;

    CmapTable*   cmapTable = &font->m_cmapTable;
    CmapFormat4* cmap      = &font->m_cmapTable.m_format4;

    u16 segCount = cmap->m_segCountX2 / 2;

    for( u32 i=0; i<segCount; ++i )
    {
        if( codepoint > cmap->m_endCount[i] )
            continue;

        if( codepoint < cmap->m_startCount[i] )
            return 0;

        if( cmap->m_idRangeOffset[i] == 0 )
        {
            return (u16)(codepoint + cmap->m_idDelta[i]);
        }

        s32 glyphIndex =
            (cmap->m_idRangeOffset[i] / 2) +
            (codepoint - cmap->m_startCount[i]) -
            (segCount - i);

        if( glyphIndex < 0 || (u32)glyphIndex >= cmap->m_numGlyphIds )
        {
            return 0;
        }

        u16 glyphId = cmap->m_glyphIdArray[glyphIndex];

        if( glyphId != 0 )
        {
            glyphId = (u16)(glyphId + cmap->m_idDelta[i]);
        }

        if( glyphId >= font->m_maxpTable.m_numGlyphs )
            return 0;

        return glyphId;
    }
    return 0;
}

void BuildGlyphIdMap( TtfFont* font )
{
    memset( &font->m_glyphIdMap, 0x00, GLYPHID_MAP_SIZE * 2 );

    for( u16 codepoint=0; codepoint < (u16)GLYPHID_MAP_SIZE; ++codepoint)
    {
        font->m_glyphIdMap[codepoint] = CodepointToGlyphId( font, codepoint );
    }
}

u16 GetGlyphId( TtfFont* font, u16 codepoint )
{
    if( codepoint < GLYPHID_MAP_SIZE )
    {
        return font->m_glyphIdMap[codepoint];
    }

    return 0;
}

Glyph* GetGlyph( TtfFont* font, u16 glyphId )
{
    Glyph* g = NULL;

    if( glyphId > 0 )
    {
        g = &font->m_glyphTable.m_glphys[glyphId];
    }

    return g;
}

void ReadTables( TtfFont* font )
{
    TableEntry* tablememory = (TableEntry*)((u8*)font->m_fontFile->m_data + sizeof( TableOffsets ));
    for( u32 i=0; i<font->m_numTables; ++i )
    {
        TableEntry* entry = &font->m_tableEntries[i];
        memcpy( entry, tablememory, sizeof( TableEntry ) );

        //strings are not big endian encoded 
        //entry->m_tag      = BigToLittle( entry->m_tag );
        entry->m_checksum = BigToLittle( entry->m_checksum );
        entry->m_offset   = BigToLittle( entry->m_offset );
        entry->m_length   = BigToLittle( entry->m_length );

        char tag[5];
        memset( &tag, 0x0, 5 );
        memcpy( &tag, &entry->m_tag, 4 );

        printf( "tag: %s\t offset: %d\t size: %d bytes\n", (char*)&tag, entry->m_offset, entry->m_length );

        tablememory++;
    }
}

u32 GetTableOffset( TtfFont* font, const char* tag )
{
    u32 numTables = font->m_numTables;
    for( u32 i=0; i<numTables; ++i )
    {
        TableEntry* entry = &font->m_tableEntries[i];

        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;         
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
        if( memcmp( (char*)&entry->m_tag, tag, 4 ) == 0 ) return entry->m_offset;
    }

    return 0;
}

void ReadGlyphTable( TtfFont* font )
{
    u32 glyphOffset = GetTableOffset( font, GLYF_TAG );
    printf( "read glpyh:\t%d (offset)\n", glyphOffset );

    void* glyphData = (u8*)font->m_fontFile->m_data + glyphOffset;

    u32         numGlyphs = font->m_maxpTable.m_numGlyphs;
    GlyphTable* glyphs    = &font->m_glyphTable;
    glyphs->m_numGlyphs   = numGlyphs;
    glyphs->m_glphys      = (Glyph*)malloc( numGlyphs * sizeof( Glyph ) );

    for( u32 glyphId=0; glyphId<numGlyphs; ++glyphId )
    {
        Glyph* g        = &glyphs->m_glphys[glyphId];
        memset( g, 0x0, sizeof( Glyph) );

        u32 startOffset = font->m_locaTable.m_glyphTableOffsets[glyphId];
        u32 endOffset   = font->m_locaTable.m_glyphTableOffsets[glyphId+1];

        if( startOffset == endOffset )
        {
            g->m_header.m_emptyGlyph = true;
            printf( "glphy ID:\t%d - empty\n", glyphId );
            continue;
        }
        else
        {
            g->m_header.m_emptyGlyph = false;
        }

        g->m_header.m_numberOfContours = (s16)BigToLittleU16( glyphData, startOffset );
        g->m_header.m_xMin             = (s16)BigToLittleU16( glyphData, startOffset + 2);
        g->m_header.m_yMin             = (s16)BigToLittleU16( glyphData, startOffset + 4);
        g->m_header.m_xMax             = (s16)BigToLittleU16( glyphData, startOffset + 6);
        g->m_header.m_yMax             = (s16)BigToLittleU16( glyphData, startOffset + 8);

        s16 numContours = g->m_header.m_numberOfContours;
        u32 headerSize  = 10;

        if( numContours > 0)
        {
            u32 numEndpoints      = numContours;
            g->m_endPtsOfContours = (u16*)malloc( sizeof( u16) * numEndpoints );
            for( u32 i=0; i<numEndpoints; ++i )
            {
                g->m_endPtsOfContours[i] = BigToLittleU16( glyphData, startOffset + headerSize + i*2 );
            }
            g->m_instructionLength = BigToLittleU16( glyphData, startOffset + headerSize + numEndpoints * 2 );

            
            g->m_numPoints  = g->m_endPtsOfContours[numEndpoints-1] + 1;
            u32 pointOffset = startOffset + headerSize + (numEndpoints * 2) + 2 + g->m_instructionLength;

            g->m_flags   = (u8*)malloc(   sizeof( u8 )   * g->m_numPoints );
            g->m_onCurve = (bool*)malloc( sizeof( bool ) * g->m_numPoints );
            g->m_x       = (s16*)malloc(  sizeof( s16 )  * g->m_numPoints );
            g->m_y       = (s16*)malloc(  sizeof( s16 )  * g->m_numPoints );

            // decode all the flags
            u32 newNumPoints  = g->m_numPoints;
            bool onCurve      = false;
            for( u32 i=0; i<g->m_numPoints; ++i )
            {
                u8 flag = *((u8*)glyphData + pointOffset);
            
                if( GetPointFlag( flag, REPEAT_FLAG ) )
                {
                    u8 repeatCount = *((u8*)glyphData + pointOffset + 1);
                    onCurve        = GetPointFlag( flag, ON_CURVE );

                    for( u32 j=0; j<repeatCount + 1; ++j )
                    {
                        g->m_flags[i+j]   = flag;
                        g->m_onCurve[i+j] = onCurve;
                    }

                    i += repeatCount;
                    pointOffset += 2;
                }
                else
                {
                    onCurve         = GetPointFlag( flag, ON_CURVE );
                    g->m_flags[i]   = flag;
                    pointOffset    += 1;
                    g->m_onCurve[i] = onCurve;
                }
            }

            // calculate the "real" number of points including the implied points
            u32 index = 0;
            for( u32 c=0; c<numContours; ++c )
            {
                u16 end          = g->m_endPtsOfContours[c];
                u32 startIndex   = 0;
                bool onCurve     = false;
                bool nextOnCurve = false;

                startIndex = index;
                for( ; index<=end; ++index )
                {
                    if( index == end )
                    {
                        onCurve     = g->m_onCurve[index];
                        nextOnCurve = g->m_onCurve[startIndex];
                    }
                    else
                    {
                        onCurve = g->m_onCurve[index];
                        nextOnCurve = g->m_onCurve[index + 1];
                    }
                    
                    if( onCurve == nextOnCurve )
                    {
                        newNumPoints++;
                    }
                }
            }

            g->m_realNumPoints = newNumPoints;

            void* xData = (u8*)glyphData + pointOffset;
            
            // decode all coordinates
            void* data = DecodePoints( g, xData, g->m_x, g->m_numPoints, true );
            data       = DecodePoints( g, data,  g->m_y, g->m_numPoints, false );
        }
        else if( numContours < 0 )
        {
            // composite glyph
            u32 compoundOffset  = startOffset + headerSize;
            u16 flag = 0;
            do
            {
                if( compoundOffset + 4 > endOffset ) break;

                CompoundComponent component = {0};
                component.m_xx = 1.0f;
                component.m_yy = 1.0f;

                flag                = BigToLittleU16( glyphData, compoundOffset );
                component.m_flags   = flag;
                component.m_glyphId = BigToLittleU16( glyphData, compoundOffset + 2 );
                compoundOffset += 4;

                if( GetCompoundFlag( flag, ARG_1_AND_2_ARE_WORDS ) )
                {
                    if( compoundOffset + 4 > endOffset ) break;
                    if( GetCompoundFlag( flag, ARGS_ARE_XY_VALUES ) )
                    {
                        component.m_arg1 = BigToLittleS16( glyphData, compoundOffset );
                        component.m_arg2 = BigToLittleS16( glyphData, compoundOffset + 2 );
                    }
                    else
                    {
                        component.m_arg1 = BigToLittleU16( glyphData, compoundOffset );
                        component.m_arg2 = BigToLittleU16( glyphData, compoundOffset + 2 );
                    }
                    compoundOffset += 4;
                }
                else
                {
                    if( compoundOffset + 2 > endOffset ) break;
                    if( GetCompoundFlag( flag, ARGS_ARE_XY_VALUES ) )
                    {
                        component.m_arg1 = *((s8*)glyphData + compoundOffset);
                        component.m_arg2 = *((s8*)glyphData + compoundOffset + 1 );
                    }
                    else
                    {
                        component.m_arg1 = *((u8*)glyphData + compoundOffset);
                        component.m_arg2 = *((u8*)glyphData + compoundOffset + 1 );
                    }
                    compoundOffset += 2;
                }

                if( GetCompoundFlag( flag, WE_HAVE_A_SCALE ) )
                {
                    if( compoundOffset + 2 > endOffset ) break;
                    component.m_xx = component.m_yy =
                        F2Dot14ToF32( BigToLittleS16( glyphData, compoundOffset ) );
                    compoundOffset += 2;
                }
                else if( GetCompoundFlag( flag, WE_HAVE_AN_X_AND_Y_SCALE ) )
                {
                    if( compoundOffset + 4 > endOffset ) break;
                    component.m_xx = F2Dot14ToF32( BigToLittleS16( glyphData, compoundOffset ) );
                    component.m_yy = F2Dot14ToF32( BigToLittleS16( glyphData, compoundOffset + 2 ) );
                    compoundOffset += 4;
                }
                else if( GetCompoundFlag( flag, WE_HAVE_A_TWO_BY_TWO ) )
                {
                    if( compoundOffset + 8 > endOffset ) break;
                    component.m_xx = F2Dot14ToF32( BigToLittleS16( glyphData, compoundOffset ) );
                    component.m_yx = F2Dot14ToF32( BigToLittleS16( glyphData, compoundOffset + 2 ) );
                    component.m_xy = F2Dot14ToF32( BigToLittleS16( glyphData, compoundOffset + 4 ) );
                    component.m_yy = F2Dot14ToF32( BigToLittleS16( glyphData, compoundOffset + 6 ) );
                    compoundOffset += 8;
                }

                CompoundComponent* components = (CompoundComponent*)realloc(
                    g->m_components,
                    sizeof( CompoundComponent ) * (g->m_numComponents + 1) );
                if( components == NULL ) break;
                g->m_components = components;
                g->m_components[g->m_numComponents++] = component;
            }
            while( GetCompoundFlag( flag, MORE_COMPONENTS ) );

            if( GetCompoundFlag( flag, WE_HAVE_INSTRUCTIONS ) &&
                compoundOffset + 2 <= endOffset )
            {
                g->m_instructionLength = BigToLittleU16( glyphData, compoundOffset );
                compoundOffset += 2;
                if( compoundOffset + g->m_instructionLength <= endOffset )
                {
                    g->m_instructions = (u8*)malloc( g->m_instructionLength );
                    memcpy( g->m_instructions,
                            (u8*)glyphData + compoundOffset,
                            g->m_instructionLength );
                }
            }
        }
        else if( numContours == 0 )
        {
            // no outlines -> space character or similar
            printf( "glyph ID:\t%d space character\n", glyphId );
        }
    }

    ResolveCompoundGlyphs( font );

    printf( "\n" );
}

void ReadLocaTable( TtfFont* font )
{
    u32 locaOffset = GetTableOffset( font, LOCA_TAG );
    printf( "read loca:\t%d (offset)\n", locaOffset );

    void* locaData = (u8*)font->m_fontFile->m_data + locaOffset;

    LocaTable* loca = &font->m_locaTable;
    loca->m_glyphTableOffsets = (u32*)malloc( sizeof( u32 ) * (font->m_maxpTable.m_numGlyphs + 1) );

    u32 numGlyphs = font->m_maxpTable.m_numGlyphs + 1;
    for( u32 i=0; i<numGlyphs; ++i )
    {
        if( font->m_headTable.m_indexToLocFormat == 0 )
        {
            loca->m_glyphTableOffsets[i] = (u32)BigToLittleU16( (u8*)locaData, i * 2 ) * 2;
        }
        if( font->m_headTable.m_indexToLocFormat == 1 )
        {
            loca->m_glyphTableOffsets[i] = BigToLittleU32( (u8*)locaData, i * 4 );
        }
    }

    printf( "last entry:\t%d\n", loca->m_glyphTableOffsets[numGlyphs-1] );
    printf( "\n" );
}

void ReadHeadTable( TtfFont* font )
{
    u32 headOffset = GetTableOffset( font, HEAD_TAG );
    printf( "read head:\t%d (offset)\n", headOffset );

    void* headData = (u8*)font->m_fontFile->m_data + headOffset;

    HeadTable* head = &font->m_headTable;

    head->m_majorVersion        = BigToLittleU16( headData, 0 );       // 1
    head->m_minorVersion        = BigToLittleU16( headData, 2 );       // 0
    head->m_fontRevision        = (s32)BigToLittleU32( headData, 4 );  // signed 16.16 fixed-point
    head->m_checksumAdjustment  = BigToLittleU32( headData, 8 ); 
    head->m_magicNumber         = BigToLittleU32( headData, 12 );      // 0x5F0F3CF5
    head->m_flags               = BigToLittleU16( headData, 16 );              
    head->m_unitsPerEm          = BigToLittleU16( headData, 18 );         
    head->m_created             = 0; // dont read            
    head->m_modified            = 0; // dont read          
    head->m_xMin                = (s16)BigToLittleU16( headData, 36 );               
    head->m_yMin                = (s16)BigToLittleU16( headData, 38 );               
    head->m_xMax                = (s16)BigToLittleU16( headData, 40 );               
    head->m_yMax                = (s16)BigToLittleU16( headData, 42 );               
    head->m_macStyle            = BigToLittleU16( headData, 44 );           
    head->m_lowestRecPPEM       = BigToLittleU16( headData, 46 );      
    head->m_fontDirectionHint   = (s16)BigToLittleU16( headData, 48 );  // deprecated, normally 2
    head->m_indexToLocFormat    = (s16)BigToLittleU16( headData, 50 );  // 0 = short, 1 = long
    head->m_glyphDataFormat     = (s16)BigToLittleU16( headData, 52 );  // 0

    printf( "units per em:\t%d\n", head->m_unitsPerEm );
    printf( "index to loc:\t%d (0 = short, 1 = long)\n", head->m_indexToLocFormat );
    printf( "\t\t- 0: read u16 entries and multiply by 2\n" );
    printf( "\t\t- 1: read u32 entries directly\n" );
    printf( "\n" );
}

void ReadMaxpTable( TtfFont* font )
{
    u32 maxpOffset = GetTableOffset( font, MAXP_TAG );
    printf( "read maxp:\t%d (offset)\n", maxpOffset );

    void* maxpData = (u8*)font->m_fontFile->m_data + maxpOffset;
    
    MaxpTable* maxp = &font->m_maxpTable;

    maxp->m_version               = BigToLittleU32( maxpData, 0  );                
    maxp->m_numGlyphs             = BigToLittleU16( maxpData, 4  );              
    maxp->m_maxPoints             = BigToLittleU16( maxpData, 6  );              
    maxp->m_maxContours           = BigToLittleU16( maxpData, 8  );            
    maxp->m_maxCompositePoints    = BigToLittleU16( maxpData, 10 );     
    maxp->m_maxCompositeContours  = BigToLittleU16( maxpData, 12 );   
    maxp->m_maxZones              = BigToLittleU16( maxpData, 14 );               
    maxp->m_maxTwilightPoints     = BigToLittleU16( maxpData, 16 );      
    maxp->m_maxStorage            = BigToLittleU16( maxpData, 18 );             
    maxp->m_maxFunctionDefs       = BigToLittleU16( maxpData, 20 );        
    maxp->m_maxInstructionDefs    = BigToLittleU16( maxpData, 22 );     
    maxp->m_maxStackElements      = BigToLittleU16( maxpData, 24 );       
    maxp->m_maxSizeOfInstructions = BigToLittleU16( maxpData, 26 );  
    maxp->m_maxComponentElements  = BigToLittleU16( maxpData, 28 );   
    maxp->m_maxComponentDepth     = BigToLittleU16( maxpData, 30 );      

    printf( "version: \t%d\n",   ( maxp->m_version == 0x00010000 ) ? 1 : 0  );
    printf( "num glyphs:\t%d\n", maxp->m_numGlyphs );
    printf( "\n" );
}

void ReadCmapTable( TtfFont* font )
{
    u32 cmapOffset = GetTableOffset( font, CMAP_TAG );
    printf( "read cmap at offset: %d\n", cmapOffset );

    font->m_cmapTable.m_cmapOffset  = cmapOffset;
    font->m_cmapTable.m_version     = BigToLittleU16( font->m_fontFile->m_data, cmapOffset );
    font->m_cmapTable.m_numRecords  = BigToLittleU16( font->m_fontFile->m_data, cmapOffset + 2 );
    font->m_cmapTable.m_cmapRecords = (CmapRecord*)malloc( sizeof( CmapRecord ) * font->m_cmapTable.m_numRecords );

    void* encodingRecords = (u8*)font->m_fontFile->m_data + (cmapOffset + 4);
    u32   numRecords     = font->m_cmapTable.m_numRecords;
    u32   offset         = 0;

    printf( "\nplatform/endcoding pairs\n--------------------------\n" );
    u16 streamRecordSize = 8;
    for( u32 i=0; i<numRecords; ++i )
    {
        CmapRecord* record = &font->m_cmapTable.m_cmapRecords[i];
        
        record->m_platformID = BigToLittleU16( encodingRecords, offset );
        record->m_encodingID = BigToLittleU16( encodingRecords, offset + 2 );
        record->m_offset     = BigToLittleU32( encodingRecords, offset + 4 );
        memset( record->m_encodingName, 0x0, 100 );

        // platform 0, encoding 3 -> Unicode BMP
        // platform 3, encoding 1 -> Windows Unicode BMP
        if( record->m_platformID == 0 && record->m_encodingID == 3 )
        {
            printf( "%d: Unicode BMP\n", i );
            memcpy( record->m_encodingName, "Unicode BMP", StringLength( "Unicode BMP" ) );
        }
        if( record->m_platformID == 0 && record->m_encodingID == 4 )
        {
            printf( "%d: Unicode full repertoire\n", i );
            memcpy( record->m_encodingName, "Unicode full repertoire", StringLength( "Unicode full repertoire" ) );
        }
        if( record->m_platformID == 0 && record->m_encodingID == 5 )
        {
            printf( "%d: Unicode variation sequence\n", i );
            memcpy( record->m_encodingName, "Unicode variation sequence", StringLength( "Unicode variation sequence" ) );
        }
        if( record->m_platformID == 3 && record->m_encodingID == 1 )
        {
            printf( "%d: Windows Unicode BMP\n", i );
            memcpy( record->m_encodingName, "Windows Unicode BMP", StringLength( "Windows Unicode BMP" ) );
        }
        if( record->m_platformID == 3 && record->m_encodingID == 10 )
        {
            printf( "%d: Windows Unicode full repertoire\n", i );
            memcpy( record->m_encodingName, "Windows Unicode full repertoire", StringLength( "Windows Unicode full repertoire" ) );
        }

        encodingRecords = (u8*)encodingRecords + streamRecordSize;
    }

    printf( "\n" );

    // read the format for all the found encodings
    void* cmapTable = (u8*)font->m_fontFile->m_data + (cmapOffset);
    for( u32 i=0; i<numRecords; ++i )
    {
        CmapRecord* record       = &font->m_cmapTable.m_cmapRecords[i];
        u32         recordOffset = record->m_offset;
        u16         format       = BigToLittleU16( cmapTable, recordOffset );
        printf( "encoding:\t%s\n", record->m_encodingName );
        printf( "cmap format:\t%d\n", format );

        if( format == CMAP_FORMAT_4 )
        {
            CmapFormat4* format4 = &font->m_cmapTable.m_format4;
            format4->m_format         = CMAP_FORMAT_4;
            format4->m_length         = BigToLittleU16( cmapTable, recordOffset + 2 );
            format4->m_language       = BigToLittleU16( cmapTable, recordOffset + 4 );

            format4->m_segCountX2     = BigToLittleU16( cmapTable, recordOffset + 6 );
            format4->m_searchRange    = BigToLittleU16( cmapTable, recordOffset + 8 );
            format4->m_entrySelector  = BigToLittleU16( cmapTable, recordOffset + 10 );
            format4->m_rangeShift     = BigToLittleU16( cmapTable, recordOffset + 12 );
            recordOffset              = recordOffset + 14;

            if( format4->m_segCountX2 % 2 != 0 || format4->m_segCountX2 == 0 )
            {
                printf( "ERROR: parsing cmap format 4\n" );
                return;
            }

            u16 segCount = format4->m_segCountX2 / 2;

            format4->m_endCount       = (u16*)malloc( sizeof( u16 ) * segCount );  
            format4->m_startCount     = (u16*)malloc( sizeof( u16 ) * segCount );
            format4->m_idDelta        = (u16*)malloc( sizeof( u16 ) * segCount );
            format4->m_idRangeOffset  = (u16*)malloc( sizeof( u16 ) * segCount );
            for( u32 i=0; i<segCount; ++i )
            {
                format4->m_endCount[i] = BigToLittleU16( cmapTable, recordOffset );
                recordOffset += 2;
            }

            format4->m_reservedPad = BigToLittleU16( cmapTable, recordOffset );
            recordOffset +=2;

            for( u32 i=0; i<segCount; ++i )
            {
                format4->m_startCount[i] = BigToLittleU16( cmapTable, recordOffset );
                recordOffset += 2;
            }
            for( u32 i=0; i<segCount; ++i )
            {
                format4->m_idDelta[i] = BigToLittleU16( cmapTable, recordOffset );
                recordOffset += 2;
            }
            for( u32 i=0; i<segCount; ++i )
            {
                format4->m_idRangeOffset[i] = BigToLittleU16( cmapTable, recordOffset );
                recordOffset += 2;
            }

            u32 format4header = 16 + 8 * segCount;
            u32 numGlyphIds   = (format4->m_length - format4header) / sizeof( u16 );
            format4->m_numGlyphIds  = numGlyphIds;
            format4->m_glyphIdArray = (u16*)malloc( sizeof( u16 ) * numGlyphIds );
            for( u32 i=0; i<numGlyphIds; ++i )
            {
                format4->m_glyphIdArray[i] = BigToLittleU16( cmapTable, recordOffset );
                recordOffset += 2;
            }

            printf( "length: \t%d\n", format4->m_length );
            printf( "language:\t%d (should be 0)\n", format4->m_language );
            printf( "seg count:\t%d\n", segCount );
            printf( "num glyphids:\t%d\n", numGlyphIds );
            printf( "\n" );

            break;
        }

        if( format == CMAP_FORMAT_12 )
        {
            // no-op
        }
    }

}
