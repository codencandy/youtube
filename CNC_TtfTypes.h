#ifndef CNC_TTFTYPES_H
#define CNC_TTFTYPES_H

#include "CNC_Types.h"

#define GDEF_TAG "GDEF"
#define GPOS_TAG "GPOS"
#define GSUB_TAG "GSUB"
#define OS2_TAG  "OS/2"
#define STAT_TAG "STAT"
#define CMAP_TAG "cmap"
#define CVT_TAG  "cvt"
#define FPGM_TAG "fpgm"
#define GASP_TAG "gasp"
#define GLYF_TAG "glyf"
#define HEAD_TAG "head"
#define HHEA_TAG "hhea"
#define HMTX_TAG "hmtx"
#define LOCA_TAG "loca"
#define MAXP_TAG "maxp"
#define NAME_TAG "name"
#define POST_TAG "post"
#define PREP_TAG "prep"

typedef struct TableOffsets
{
    u32 m_sfntVersion;
    u16 m_numTables;
    u16 m_searchRange;
    u16 m_entrySelector;
    u16 m_rangeShift;

} TableOffsets;

typedef struct TableEntry
{
    u32 m_tag;
    u32 m_checksum;
    u32 m_offset;
    u32 m_length;

} TableEntry;

typedef struct GlyphTable
{

} GlyphTable;

typedef struct CmapFormat4
{
    u16 m_format;
    u16 m_length;
    u16 m_language;

    u16 m_segCountX2;
    u16 m_searchRange;
    u16 m_entrySelector;
    u16 m_rangeShift;

    u16* m_endCount;      // u16 * segCount
    u16  m_reservedPad;
    u16* m_startCount;    // u16 * segCount
    u16* m_idDelta;       // u16 * segCount
    u16* m_idRangeOffset; // u16 * segCount

    u16* m_glyphIdArray;
    
} CmapFormat4;

typedef struct CmapRecord
{
    u16 m_platformID;
    u16 m_encodingID;
    u32 m_offset;

} CmapRecord;

typedef struct CmapTable
{
    u32           m_cmapOffset;
    u16           m_version;
    u16           m_numRecords;
    CmapRecord*   m_cmapRecords;

} CmapTable;

typedef struct MaxpTable
{

} MaxpTable;

typedef struct TtfFont
{
    File*        m_fontFile;
    TableOffsets m_offsets;

    u32          m_numTables;
    TableEntry*  m_tableEntries;

    GlyphTable   m_glyphTable;
    CmapTable    m_cmapTable;
    MaxpTable    m_maxpTable;

} TtfFont;

#endif//CNC_TTFTYPES_H
