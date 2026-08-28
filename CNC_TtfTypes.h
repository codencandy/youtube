#ifndef CNC_TTFTYPES_H
#define CNC_TTFTYPES_H

#include "CNC_Types.h"

typedef struct TtfOffsetTable
{
    u32 m_sfntVersion;
    u16 m_numTables;
    u16 m_searchRange;
    u16 m_entrySelector;
    u16 m_rangeShift;

} TtfOffsetTable;

typedef struct TableEntry
{
    u32 m_tag;
    u32 m_checksum;
    u32 m_offset;
    u32 m_length;

} TableEntry;

typedef struct Font
{
    File m_fontFile;

    u32         m_numTables;
    TableEntry* m_tableEntries;

} Font;

#endif//CNC_TTFTYPES_H
