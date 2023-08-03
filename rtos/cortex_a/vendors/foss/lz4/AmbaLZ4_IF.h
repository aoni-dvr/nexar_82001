#ifndef AMBA_LZ4_IF_H_
#define AMBA_LZ4_IF_H_
#include "AmbaTypes.h"
#include "AmbaWrap.h"

// Return code definition
#define LZ4_OK (0U)
#define LZ4_NG_1 (1U) // invalid param
#define LZ4_NG_2 (2U) // internal error

//#define TILE_BASE_VIGNETTE_CALIBRATION

typedef struct {
	INT16 X;
    INT16 Y;
} AMBA_LZ4_GRID_POINT_s;

//For Vignette Table Compresssion
#if defined(TILE_BASE_VIGNETTE_CALIBRATION)
UINT32 AmbaLZ4_VigGetTblDecWorkSize(UINT32 *WorkSize);
UINT32 AmbaLZ4_VigGetTblEncWorkSize(UINT32 *WorkSize);
UINT32 AmbaLZ4_VigGetTblEncBound(INT32 TableCount, const INT32 SrcTableLen[], UINT32 *SizeBound);
UINT32 AmbaLZ4_VigTblEncode(INT32 TableCount, const UINT16* const SrcTable[],
    const INT32 SrcTableWidth[], const INT32 SrcTableHeight[],
    INT8 *Dst, INT32 DstInputSize,
    INT32 DstOutputSize[],
    INT16 *Workplace);
UINT32 AmbaLZ4_VigTblDecode(INT32 TableCount,
    INT8 *Input,
    const INT32 InputSize[],
    const INT32 SrcTableWidth[], const INT32 SrcTableHeight[],
    UINT16* const DstVigTable[],    
    INT16 *Workplace);
#endif

//For Blend Table Compresssion
UINT32 AmbaLZ4_BlendGetTblEncWorkSize(UINT32 *WorkSize);
UINT32 AmbaLZ4_BlendGetTblEncBound(INT32 TableCount, const INT32 SrcTableLen[], UINT32 *SizeBound);
UINT32 AmbaLZ4_BlendTblDecode(INT32 TableCount, UINT8 *Input,
    const INT32 InputSize[], UINT8* const DstBlendTable[], const INT32 DstBlendTableSize[]);
UINT32 AmbaLZ4_BlendTblEncode(INT32 TableCount, const UINT8* const SrcTable[],
    const INT32 SrcTablelen[], INT8 *Dst, INT32 DstInputSize,
    INT32 DstOutputSize[],
    const INT16* Workplace);

//For Warp Table Compresssion
UINT32 AmbaLZ4_WarpGetTblDecWorkSize(UINT32 *WorkSize);
UINT32 AmbaLZ4_WarpGetTblEncWorkSize(UINT32 *WorkSize);
UINT32 AmbaLZ4_WarpGetTblEncBound(INT32 TableCount,
    const INT32 TableHgridNum[], const INT32 TableVgridNum[], UINT32 *SizeBound);
UINT32 AmbaLZ4_WarpTblEncode(INT32 TableCount,
    const AMBA_LZ4_GRID_POINT_s* const SrcTable[],
    const INT32 SrcTableHgridNum[], const INT32 SrcTableVgridNum[],
    INT8 *Dst, UINT32 DstInputSize,
    INT32 DstOutputXSize[], INT32 DstOutputYSize[], 
    INT16 *Workplace);
UINT32 AmbaLZ4_WarpTblDecode(INT32 TableCount,
    INT8 *Input,
    const INT32 InputXSize[], const INT32 InputYSize[], 
    const INT32 SrcTableHgridNum[], const INT32 SrcTableVgridNum[],
    AMBA_LZ4_GRID_POINT_s* const DstWarpTable[],    
    INT16 *Workplace);

typedef struct {
	UINT32 StartX;
	UINT32 StartY;
	UINT32 Width;
	UINT32 Height;
} AMBA_LZ4_WIN_s;

typedef struct {
	UINT32 Width;
	UINT32 Height;
} AMBA_LZ4_TBL_SIZE_s;

UINT32 AmbaLZ4_TblDecodeSegmentToWin(INT32 TableCount, const UINT8 *Input,
    UINT8* const DstTable[], const AMBA_LZ4_TBL_SIZE_s DstTableSize[], const AMBA_LZ4_WIN_s DstWinInfo[],
    INT32 SegmentSize, void *CachedWorkplace);
UINT32 AmbaLZ4_TblDecodeSegment(INT32 TableCount, const UINT8 *Input,
    UINT8* const DstTable[], INT32 SegmentSize, void *CachedWorkplace);
UINT32 AmbaLZ4_TblEncodeSegment(INT32 TableCount, const UINT8* const SrcTable[],
    const INT32 SrcTablelen[], INT8 *Dst, INT32 DstInputSize,
    INT32 DstOutputSize[],
    INT32 SegmentSize,
    const INT16* Workplace);

UINT32 AmbaLZ4_DeCompressFile(UINT8 *pSrc, UINT8 *pDst, UINT32 SrcSize, UINT32 DstSize);

#endif
