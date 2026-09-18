#include "CNC_Types.h"
#include "CNC_TtfTypes.h"
#include "CNC_Tools.h"
#include <string.h>
#include <stdio.h>

bool IsTrueType( TtfFont* font );
void ReadTableOffsets( TtfFont* font );
void ReadTables( TtfFont* font );
u32  GetTableOffset( TtfFont* font, const char* tag );
void ReadCmapTable( TtfFont* font );

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
        u16         recordOffset = record->m_offset;
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

            u32 numGlyphIds = (format4->m_length - recordOffset) / sizeof( u16 );
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
        }

        if( format == CMAP_FORMAT_12 )
        {
            // no-op
        }
    }

}