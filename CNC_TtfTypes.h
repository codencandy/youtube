#ifndef CNC_TTFTYPES_H
#define CNC_TTFTYPES_H

#include "CNC_Types.h"

#define GDEF_TAG       "GDEF"
#define GPOS_TAG       "GPOS"
#define GSUB_TAG       "GSUB"
#define OS2_TAG        "OS/2"
#define STAT_TAG       "STAT"
#define CMAP_TAG       "cmap"
#define CVT_TAG        "cvt"
#define FPGM_TAG       "fpgm"
#define GASP_TAG       "gasp"
#define GLYF_TAG       "glyf"
#define HEAD_TAG       "head"
#define HHEA_TAG       "hhea"
#define HMTX_TAG       "hmtx"
#define LOCA_TAG       "loca"
#define MAXP_TAG       "maxp"
#define NAME_TAG       "name"
#define POST_TAG       "post"
#define PREP_TAG       "prep"

#define CMAP_FORMAT_4   4
#define CMAP_FORMAT_12  12

enum point_flag
{
    ON_CURVE          = 0x01,
    X_SHORT_1BYTE     = 0x02,
    Y_SHORT_1BYTE     = 0x04,
    REPEAT_FLAG       = 0x08,
    X_SAME_OR_POS     = 0x10,
    Y_SAME_OR_POS     = 0x20,
    MAY_OVERLAP       = 0x40,
    ERROR_POINT_FLAG  = 0x80
};

typedef struct Codepoint
{
    u8  m_numUtf8Bytes;
    u16 m_codepoint;
    
} Codepoint;

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

typedef struct GlyphHeader
{
    s16  m_numberOfContours;
    s16  m_xMin;
    s16  m_yMin;
    s16  m_xMax;
    s16  m_yMax;

    bool m_emptyGlyph;

} GlyphHeader;

typedef struct Glyph
{
    GlyphHeader m_header;

    u16*        m_endPtsOfContours;
    u16         m_instructionLength;
    u8*         m_instructions;

    u32         m_numPoints;
    u8*         m_flags;
    bool*       m_onCurve;
    s16*        m_x;
    s16*        m_y;

} Glyph;

typedef struct GlyphTable
{
    u32    m_numGlyphs;
    Glyph* m_glphys;

} GlyphTable;

typedef struct LocaTable
{
    u32* m_glyphTableOffsets;
    
} LocaTable;

typedef struct HeadTable
{
    u16 m_majorVersion;       // 1
    u16 m_minorVersion;       // 0
    s32 m_fontRevision;       // signed 16.16 fixed-point
    u32 m_checksumAdjustment; 
    u32 m_magicNumber;        // 0x5F0F3CF5
    u16 m_flags;              
    u16 m_unitsPerEm;         
    s64 m_created;            
    s64 m_modified;           
    s16 m_xMin;               
    s16 m_yMin;               
    s16 m_xMax;               
    s16 m_yMax;               
    u16 m_macStyle;           
    u16 m_lowestRecPPEM;      
    s16 m_fontDirectionHint;  // deprecated, normally 2
    s16 m_indexToLocFormat;   // 0 = short, 1 = long
    s16 m_glyphDataFormat;    // 0

} HeadTable;

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

    u16  m_numGlyphIds;
    u16* m_glyphIdArray;
    
} CmapFormat4;

typedef struct CmapRecord
{
    u16  m_platformID;
    u16  m_encodingID;
    u32  m_offset;
    char m_encodingName[100];

} CmapRecord;

typedef struct CmapTable
{
    u32           m_cmapOffset;
    u16           m_version;
    u16           m_numRecords;
    CmapRecord*   m_cmapRecords;

    CmapFormat4   m_format4;

} CmapTable;

typedef struct MaxpTable
{
    u32 m_version;                // offset  0: 0x00010000
    u16 m_numGlyphs;              // offset  4
    u16 m_maxPoints;              // offset  6
    u16 m_maxContours;            // offset  8
    u16 m_maxCompositePoints;     // offset 10
    u16 m_maxCompositeContours;   // offset 12
    u16 m_maxZones;               // offset 14
    u16 m_maxTwilightPoints;      // offset 16
    u16 m_maxStorage;             // offset 18
    u16 m_maxFunctionDefs;        // offset 20
    u16 m_maxInstructionDefs;     // offset 22
    u16 m_maxStackElements;       // offset 24
    u16 m_maxSizeOfInstructions;  // offset 26
    u16 m_maxComponentElements;   // offset 28
    u16 m_maxComponentDepth;      // offset 30

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
    HeadTable    m_headTable;
    LocaTable    m_locaTable;

    f32          m_fontScale;

} TtfFont;

#endif//CNC_TTFTYPES_H
