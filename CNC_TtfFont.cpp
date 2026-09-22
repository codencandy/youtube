#include "CNC_Types.h"
#include "CNC_TtfTypes.h"
#include "CNC_Tools.h"
#include <string.h>
#include <stdio.h>

bool  IsTrueType( TtfFont* font );
void  ReadTableOffsets( TtfFont* font );
bool  GetPointFlag( u8 flag, point_flag bit );
void* DecodePoints( Glyph* g, void* data, s16* dest, u32 numPoints, bool x );
void  ReadTables( TtfFont* font );
u32   GetTableOffset( TtfFont* font, const char* tag );
void  PrintGlyphData( Glyph* g );

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

void PrintGlyphData( Glyph* g )
{
    printf( "glyph data\n---------------\n" );
    printf( "num points:\t%d\n", g->m_numPoints );
    
    for( u32 i=0; i<g->m_numPoints; ++i )
    {
        printf( "point:\t%d | %d ", g->m_x[i], g->m_y[i] );
        if( g->m_onCurve ) 
            printf( "(on curve)\n" );
        else
            printf( "(not on curve)\n" );
    }
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
        u32 startOffset = font->m_locaTable.m_glyphTableOffsets[glyphId];
        u32 endOffset   = font->m_locaTable.m_glyphTableOffsets[glyphId+1];

        if( startOffset == endOffset )
        {
            memset( g, 0x0, sizeof( Glyph) );
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
            for( u32 i=0; i<g->m_numPoints; ++i )
            {
                u8 flag = *((u8*)glyphData + pointOffset);
                if( GetPointFlag( flag, REPEAT_FLAG ) )
                {
                    u8 repeatCount = *((u8*)glyphData + pointOffset + 1);
                    for( u32 j=0; j<repeatCount + 1; ++j )
                    {
                        g->m_flags[i+j]   = flag;
                        g->m_onCurve[i+j] = GetPointFlag( flag, ON_CURVE );
                    }
                    i += repeatCount;
                    pointOffset += 2;
                }
                else
                {
                    g->m_flags[i] = flag;
                    pointOffset += 1;
                    g->m_onCurve[i] = GetPointFlag( flag, ON_CURVE );
                }

            }

            void* xData = (u8*)glyphData + pointOffset;
            
            // decode all coordinates
            void* data = DecodePoints( g, xData, g->m_x, g->m_numPoints, true );
            data       = DecodePoints( g, data,  g->m_y, g->m_numPoints, false );
        }
        else if( numContours < 0 )
        {
            // composite glyph
        }
        else if( numContours == 0 )
        {
            // no outlines -> space character or similar
            printf( "glyph ID:\t%d space character\n", glyphId );
        }

    }

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