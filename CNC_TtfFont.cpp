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
    for( u32 i=0; i<numRecords; ++i )
    {
        CmapRecord* record = &font->m_cmapTable.m_cmapRecords[i];
        
        record->m_platformID = BigToLittleU16( encodingRecords, offset );
        record->m_encodingID = BigToLittleU16( encodingRecords, offset + 2 );
        record->m_offset     = BigToLittleU32( encodingRecords, offset + 4 );

        // platform 0, encoding 3 -> Unicode BMP
        // platform 3, encoding 1 -> Windows Unicode BMP
        if( record->m_platformID == 0 && record->m_encodingID == 3 )
        {
            printf( "%d: Unicode BMP\n", i );
        }
        if( record->m_platformID == 0 && record->m_encodingID == 4 )
        {
            printf( "%d: Unicode full repertoire\n", i );
        }
        if( record->m_platformID == 0 && record->m_encodingID == 5 )
        {
            printf( "%d: Unicode variation squenece, format 14\n", i );
        }
        if( record->m_platformID == 3 && record->m_encodingID == 1 )
        {
            printf( "%d: Windows Unicode BMP, often format 4\n", i );
        }
        if( record->m_platformID == 3 && record->m_encodingID == 10 )
        {
            printf( "%d: Windows Unicode full repertoire, often format 12\n", i );
        }

        encodingRecords = (u8*)encodingRecords + sizeof( CmapRecord );
    }

}