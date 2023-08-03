
#if defined(__unix__) || defined(_WIN32) || defined(WIN32)
#define DIFF_OPTIMIZE (0)
#else
#define DIFF_OPTIMIZE (1) // For misrac, force to disable
#endif

#include "AmbaLZ4_IF.h"
#include "lz4.h"
#include "lz4hc.h"

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaDef.h"

#define MAX_WARP_TBL_H_GRID_NUM (256UL)
#define MAX_WARP_TBL_V_GRID_NUM (192UL)
#define MAX_WARP_TBL_LEN (MAX_WARP_TBL_H_GRID_NUM * MAX_WARP_TBL_V_GRID_NUM)

inline static UINT32 lz4_memcpy(void *dest, const void *source, SIZE_t num)
{
    extern void neon_memcpy(void *dest, const void *source, SIZE_t num) GNU_WEAK_SYMBOL;

    typedef void (*PFN_MEMCPY_t)(void *dest, const void *source, SIZE_t num);

    UINT32        RetVal = LZ4_OK;
    PFN_MEMCPY_t  pMemCpy = neon_memcpy;

    if (pMemCpy != NULL) {
        pMemCpy(dest, source, num);
    } else {
        RetVal |= AmbaWrap_memcpy(dest, source, num);
        if (RetVal != LZ4_OK) {
            RetVal = LZ4_NG_2;
        }
    }

    return RetVal;
}

//#include "AmbaCalib_Def.h"
//#include "AmbaCalib_WarpIF.h"

// AVM_BLEND_MAX_SIZE
//#include "AmbaCalib_AVMIF.h"

#if defined(TILE_BASE_VIGNETTE_CALIBRATION)
#include "AmbaCalib_VignetteDef.h"
#endif

void WarpTbl_RowDiff(INT16 *WarpVector, INT16 *WarpDiffVector, INT32 HorGridNum, INT32 VerGridNum);
void WarpTbl_RowDiffRecover(INT16 *WarpVector, INT16 *WarpDiffVector, INT32 HorGridNum, INT32 VerGridNum);

#if defined(TILE_BASE_VIGNETTE_CALIBRATION)
void VigTbl_RowDiff(INT16 *VigVal, INT16 *VigDiffVal, INT32 Width, INT32 Height);
void VigTbl_RowDiffRecover(INT16 *VigVal, INT16 *VigDiffVal, INT32 Width, INT32 Height);
#define VIGNETTE_MAX_SIZE_INTERNAL (((MAX_VIG_TBL_LEN + 8UL ) >> 3UL ) << 3UL)
#endif

// Address pointer casting
typedef struct{
    union {
        const UINT8 *const_uint8_ptr;
        UINT8 *uint8_ptr;
        INT8 *int8_ptr;
        char *char_ptr;
        UINT16 *uint16_ptr;
        INT16 *int16_ptr;
        UINT32 *uint32_ptr;
        INT32 *int32_ptr;
        void *void_ptr;
    };
} LZ4_MISRA_PTR;

static inline UINT32 ctx_check_blend_table_length(UINT32 Length)
{
    UINT32 RetVal = LZ4_OK;

    if (Length > LZ4_MAX_INPUT_SIZE) {
        RetVal = LZ4_NG_1;
    }
    return RetVal;
}

#if defined(TILE_BASE_VIGNETTE_CALIBRATION)
static inline UINT32 ctx_check_vignette_table_length(UINT32 Length)
{
    UINT32 RetVal = LZ4_OK;

    if (Length > VIGNETTE_MAX_SIZE_INTERNAL) {
        RetVal = LZ4_NG_1;
    }

    return RetVal;
}

static inline UINT32 ctx_check_vignette_table_gridNum(UINT32 Width, UINT32 Height)
{
    UINT32 RetVal = LZ4_OK;

    if (Width > 65U || Height > 49U) {
        RetVal = LZ4_NG_1;
    }

    return RetVal;
}
#endif

static inline UINT32 ctx_check_warp_table_gridNum(UINT32 HGridNum, UINT32 VGridNum)
{
    UINT32 RetVal = LZ4_OK;

    if (HGridNum > 82U || VGridNum > 70U) {
        RetVal = LZ4_NG_1;
    }

    return RetVal;
}

static inline UINT32 ctx_check_valid_pointer(const void* Ptr)
{
    UINT32 RetVal = LZ4_OK;

    if (Ptr == NULL) {
        RetVal = LZ4_NG_1;
    }

    return RetVal;
}

UINT32 AmbaLZ4_BlendGetTblEncWorkSize(UINT32 *WorkSize)
{
    UINT32 RetVal = LZ4_OK;

    RetVal |= ctx_check_valid_pointer(WorkSize);

    if ( RetVal == LZ4_OK ) {
        *WorkSize = LZ4_sizeofStateHC();
    }

    return RetVal;
}

UINT32 AmbaLZ4_BlendGetTblEncBound(INT32 TableCount, const INT32 SrcTableLen[], UINT32 *SizeBound)
{
    UINT32 RetVal = LZ4_OK;
    INT32 TableIdx;

    RetVal |= ctx_check_valid_pointer(SizeBound);
    RetVal |= ctx_check_valid_pointer(SrcTableLen);

    if ( RetVal == LZ4_OK ) {
        *SizeBound = 0;
        for (TableIdx = 0; TableIdx < TableCount; TableIdx++) {
            RetVal |= ctx_check_blend_table_length(SrcTableLen[TableIdx]);

            if ( RetVal == LZ4_OK ) {
                *SizeBound += LZ4_compressBound(SrcTableLen[TableIdx]);
            } else {
                break;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaLZ4_BlendTblDecode(INT32 TableCount, UINT8 *Input,
    const INT32 InputSize[], UINT8* const DstBlendTable[], const INT32 DstBlendTableSize[])
{
    UINT32 RetVal = LZ4_OK;
    INT32 TblIdx;
    INT32 DecompressedSize;
    INT32 InputBuffOffset;

    RetVal |= ctx_check_valid_pointer(Input);
    RetVal |= ctx_check_valid_pointer(InputSize);
    RetVal |= ctx_check_valid_pointer(DstBlendTableSize);

    if ( RetVal == LZ4_OK ) {
        InputBuffOffset = 0;
        for (TblIdx = 0; TblIdx < TableCount; TblIdx++) {
            RetVal |= ctx_check_valid_pointer(DstBlendTable[TblIdx]);

            if ( RetVal == LZ4_OK ) {
                DecompressedSize = LZ4_decompress_safe(
                (char*)Input + InputBuffOffset,
                (char*)DstBlendTable[TblIdx], InputSize[TblIdx], DstBlendTableSize[TblIdx]);
                if( DecompressedSize == 0 ) {
                    RetVal = LZ4_NG_2;
                    break;
                }
                InputBuffOffset += InputSize[TblIdx];
            } else {
                break;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaLZ4_BlendTblEncode(INT32 TableCount, const UINT8* const SrcTable[],
    const INT32 SrcTablelen[], INT8 *Dst, INT32 DstInputSize,
    INT32 DstOutputSize[],
    const INT16* Workplace)
{
    UINT32 RetVal = LZ4_OK;
    INT32 bufOffset;
    INT32 EncodeSize;
    INT32 TblIdx;
    INT32 DstRemainSize;

    RetVal |= ctx_check_valid_pointer(SrcTablelen);
    RetVal |= ctx_check_valid_pointer(Dst);
    RetVal |= ctx_check_valid_pointer(DstOutputSize);
    RetVal |= ctx_check_valid_pointer(Workplace);

    if ( RetVal == LZ4_OK ) {
        bufOffset = 0;
        DstRemainSize = DstInputSize;

        for (TblIdx = 0; TblIdx < TableCount; TblIdx++) {
            RetVal |= ctx_check_valid_pointer(SrcTable[TblIdx]);

            if ( RetVal == LZ4_OK ) {
                EncodeSize = LZ4_compress_HC_Feed_workSpace((void*)SrcTable[TblIdx],
                (void*)(Dst + bufOffset), SrcTablelen[TblIdx],DstRemainSize, 9, Workplace);
                if( EncodeSize == 0 ) {
                    RetVal = LZ4_NG_2;
                    break;
                }
                bufOffset += EncodeSize;
                DstRemainSize -= EncodeSize;
                DstOutputSize[TblIdx] = EncodeSize;
            } else {
                break;
            }
       }
    }

    return RetVal;
}

UINT32 AmbaLZ4_WarpGetTblDecWorkSize(UINT32 *WorkSize)
{
    UINT32 RetVal = LZ4_OK;

    RetVal |= ctx_check_valid_pointer(WorkSize);

    if ( RetVal == LZ4_OK ) {
        *WorkSize = MAX_WARP_TBL_LEN*sizeof(INT16)*4;
    }

    return RetVal;
}

UINT32 AmbaLZ4_WarpGetTblEncWorkSize(UINT32 *WorkSize)
{
    UINT32 RetVal = LZ4_OK;

    RetVal |= ctx_check_valid_pointer(WorkSize);

    if ( RetVal == LZ4_OK ) {
        *WorkSize = (MAX_WARP_TBL_LEN*sizeof(INT16)*4) + LZ4_sizeofStateHC();
    }

    return RetVal;
}

UINT32 AmbaLZ4_WarpGetTblEncBound(INT32 TableCount,
    const INT32 TableHgridNum[], const INT32 TableVgridNum[], UINT32 *SizeBound)
{
    UINT32 RetVal = LZ4_OK;
    INT32 TableIdx;

    RetVal |= ctx_check_valid_pointer(SizeBound);
    RetVal |= ctx_check_valid_pointer(TableHgridNum);
    RetVal |= ctx_check_valid_pointer(TableVgridNum);

    if ( RetVal == LZ4_OK ) {
        *SizeBound = 0;

        for (TableIdx = 0; TableIdx<TableCount; TableIdx++) {

            RetVal |= ctx_check_warp_table_gridNum(TableHgridNum[TableIdx], TableVgridNum[TableIdx]);

            if ( RetVal == LZ4_OK ) {
                *SizeBound += LZ4_compressBound(TableHgridNum[TableIdx]*TableVgridNum[TableIdx]*sizeof(INT16)*2);
            } else {
                break;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaLZ4_WarpTblEncode(INT32 TableCount,
    const AMBA_LZ4_GRID_POINT_s* const SrcTable[],
    const INT32 SrcTableHgridNum[], const INT32 SrcTableVgridNum[],
    INT8 *Dst, UINT32 DstInputSize,
    INT32 DstOutputXSize[], INT32 DstOutputYSize[],
    INT16 *Workplace)
{
    UINT32 RetVal = LZ4_OK;
    INT32 BufOffset;

    INT32 DstRemainSize;

    INT16 *WarpXVector = NULL;
    INT16 *WarpYVector = NULL;
    INT16 *WarpXDiffVector = NULL;
    INT16 *WarpYDiffVector = NULL;

    INT16 *LZ4Workplace = NULL;

    INT32 TblIdx;

    RetVal |= ctx_check_valid_pointer(SrcTableHgridNum);
    RetVal |= ctx_check_valid_pointer(SrcTableVgridNum);
    RetVal |= ctx_check_valid_pointer(Dst);
    RetVal |= ctx_check_valid_pointer(DstOutputXSize);
    RetVal |= ctx_check_valid_pointer(DstOutputYSize);
    RetVal |= ctx_check_valid_pointer(Workplace);

    if ( RetVal == LZ4_OK ) {
        WarpXVector = Workplace;
        WarpYVector = &Workplace[MAX_WARP_TBL_LEN];
        WarpXDiffVector = &Workplace[MAX_WARP_TBL_LEN*2];
        WarpYDiffVector = &Workplace[MAX_WARP_TBL_LEN*3];
        LZ4Workplace = &Workplace[MAX_WARP_TBL_LEN*4];

        BufOffset = 0;
        DstRemainSize = DstInputSize;

        for (TblIdx = 0; TblIdx < TableCount; TblIdx++) {
            UINT32 HorGridNum, VerGridNum;
            INT32 EncodeSize;
            const AMBA_LZ4_GRID_POINT_s *WarpVector;
            UINT32 I;

            RetVal |= ctx_check_valid_pointer(SrcTable[TblIdx]);
            RetVal |= ctx_check_warp_table_gridNum(SrcTableHgridNum[TblIdx], SrcTableVgridNum[TblIdx]);

            if ( RetVal == LZ4_OK ) {
                HorGridNum = SrcTableHgridNum[TblIdx];
                VerGridNum = SrcTableVgridNum[TblIdx];
                WarpVector = SrcTable[TblIdx];

                for (I = 0U; I < (HorGridNum*VerGridNum); I++) {
                    WarpXVector[I] = WarpVector[I].X;
                    WarpYVector[I] = WarpVector[I].Y;
                }

                // Copy First row
                RetVal |= lz4_memcpy(WarpXDiffVector, WarpXVector, sizeof(INT16)*HorGridNum);
                RetVal |= lz4_memcpy(WarpYDiffVector, WarpYVector, sizeof(INT16)*HorGridNum);

            #if (defined(DIFF_OPTIMIZE)) && (DIFF_OPTIMIZE == 1)
                WarpTbl_RowDiff(WarpXVector, WarpXDiffVector, HorGridNum, VerGridNum);
                WarpTbl_RowDiff(WarpYVector, WarpYDiffVector, HorGridNum, VerGridNum);
            #else
                {
                    UINT32 J;
                    for (I = 1; I < VerGridNum; I++) {
                        for (J = 0; J < HorGridNum; J++) {
                            WarpXDiffVector[(I*HorGridNum) + J] =
                                WarpXVector[((I-1)*HorGridNum) + J] - WarpXVector[(I*HorGridNum) + J];
                        }
                    }
                    for (I = 1; I < VerGridNum; I++) {
                        for (J = 0; J < HorGridNum; J++) {
                            WarpYDiffVector[(I*HorGridNum) + J] =
                                WarpYVector[((I-1)*HorGridNum) + J] - WarpYVector[(I*HorGridNum) + J];
                        }
                    }
                }
            #endif
                DstOutputXSize[TblIdx] = 0;
                DstOutputYSize[TblIdx] = 0;
                EncodeSize = LZ4_compress_HC_Feed_workSpace((void*)WarpXDiffVector,
                    (void*)(Dst + BufOffset),
                    HorGridNum * VerGridNum * sizeof(INT16),
                    DstRemainSize, 9, LZ4Workplace);
                if( EncodeSize == 0 ) {
                    RetVal = LZ4_NG_2;
                    break;
                }
                BufOffset += EncodeSize;
                DstRemainSize -= EncodeSize;
                DstOutputXSize[TblIdx] = EncodeSize;
                EncodeSize = LZ4_compress_HC_Feed_workSpace((void*)WarpYDiffVector,
                    (void*)(Dst + BufOffset),
                    HorGridNum * VerGridNum * sizeof(INT16),
                    DstRemainSize, 9, LZ4Workplace);
                if( EncodeSize == 0 ) {
                    RetVal = LZ4_NG_2;
                    break;
                }
                BufOffset += EncodeSize;
                DstRemainSize -= EncodeSize;
                DstOutputYSize[TblIdx] = EncodeSize;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaLZ4_WarpTblDecode(INT32 TableCount,
    INT8 *Input,
    const INT32 InputXSize[], const INT32 InputYSize[],
    const INT32 SrcTableHgridNum[], const INT32 SrcTableVgridNum[],
    AMBA_LZ4_GRID_POINT_s* const DstWarpTable[],
    INT16 *Workplace)
{
    UINT32 RetVal = LZ4_OK;

    INT16 *WarpXVector = NULL;
    INT16 *WarpYVector = NULL;
    INT16 *WarpXDiffVector = NULL;
    INT16 *WarpYDiffVector = NULL;

    INT8 *EncBuf;
    INT32 TblIdx;

    RetVal |= ctx_check_valid_pointer(Input);
    RetVal |= ctx_check_valid_pointer(InputXSize);
    RetVal |= ctx_check_valid_pointer(InputYSize);
    RetVal |= ctx_check_valid_pointer(SrcTableHgridNum);
    RetVal |= ctx_check_valid_pointer(SrcTableVgridNum);
    RetVal |= ctx_check_valid_pointer(Workplace);

    if ( RetVal == LZ4_OK ) {
        WarpXVector = Workplace;
        WarpYVector = &Workplace[MAX_WARP_TBL_LEN];
        WarpXDiffVector = &Workplace[MAX_WARP_TBL_LEN*2];
        WarpYDiffVector = &Workplace[MAX_WARP_TBL_LEN*3];

        EncBuf = Input;
        for (TblIdx=0; TblIdx < TableCount; TblIdx++) {
            UINT32 DecompressedSize;
            UINT32 I;
            UINT32 HorGridNum, VerGridNum;
            AMBA_LZ4_GRID_POINT_s* WarpTable;// = DstWarpTable[TblIdx];

            RetVal |= ctx_check_valid_pointer(DstWarpTable[TblIdx]);
            RetVal |= ctx_check_warp_table_gridNum(SrcTableHgridNum[TblIdx], SrcTableVgridNum[TblIdx]);

            if ( RetVal == LZ4_OK ) {
                HorGridNum = SrcTableHgridNum[TblIdx];
                VerGridNum = SrcTableVgridNum[TblIdx];

                DecompressedSize = LZ4_decompress_safe((char*)EncBuf,
                    (char*)WarpXDiffVector,
                    InputXSize[TblIdx], MAX_WARP_TBL_LEN * sizeof(INT16));
                if( DecompressedSize <= 0) {
                    RetVal = LZ4_NG_2;
                    break;
                }
                EncBuf += InputXSize[TblIdx];

                DecompressedSize = LZ4_decompress_safe((char*)EncBuf,
                    (char*)WarpYDiffVector,
                    InputYSize[TblIdx], MAX_WARP_TBL_LEN * sizeof(INT16));
                if( DecompressedSize <= 0) {
                    RetVal = LZ4_NG_2;
                    break;
                }
                EncBuf += InputYSize[TblIdx];

                // Recover Y direction differential
                RetVal |= lz4_memcpy(WarpXVector, WarpXDiffVector, sizeof(INT16) * HorGridNum);
                RetVal |= lz4_memcpy(WarpYVector, WarpYDiffVector, sizeof(INT16) * HorGridNum);

            #if (defined(DIFF_OPTIMIZE)) && (DIFF_OPTIMIZE == 1)
                WarpTbl_RowDiffRecover(WarpXVector, WarpXDiffVector, HorGridNum, VerGridNum);
                WarpTbl_RowDiffRecover(WarpYVector, WarpYDiffVector, HorGridNum, VerGridNum);
            #else
                {
                    UINT32 J;
                    for (I = 1; I < VerGridNum; I++) {
                        for (J = 0; J < HorGridNum; J++) {
                            WarpXVector[(I*HorGridNum) + J] =
                            WarpXVector[((I-1)*HorGridNum) + J] - WarpXDiffVector[(I*HorGridNum) + J];
                        }
                    }
                    for (I = 1; I < VerGridNum; I++) {
                        for (J = 0; J < HorGridNum; J++) {
                            WarpYVector[(I*HorGridNum) + J] =
                            WarpYVector[((I-1)*HorGridNum) + J] - WarpYDiffVector[(I*HorGridNum) + J];
                        }
                    }
                }
            #endif

                WarpTable = DstWarpTable[TblIdx];
                for (I = 0; I < HorGridNum*VerGridNum; I++) {
                    WarpTable[I].X = WarpXVector[I];
                    WarpTable[I].Y = WarpYVector[I];
                }
            } else {
                break;
            }
        }
    }

    return RetVal;
}

#if defined(TILE_BASE_VIGNETTE_CALIBRATION)
UINT32 AmbaLZ4_VigGetTblDecWorkSize(UINT32 *WorkSize)
{
    UINT32 RetVal = LZ4_OK;

    RetVal |= ctx_check_valid_pointer(WorkSize);

    if ( RetVal == LZ4_OK ) {
        *WorkSize = VIGNETTE_MAX_SIZE_INTERNAL*2*sizeof(INT16);
    }

    return RetVal;
}

UINT32 AmbaLZ4_VigGetTblEncWorkSize(UINT32 *WorkSize)
{
    UINT32 RetVal = LZ4_OK;

    RetVal |= ctx_check_valid_pointer(WorkSize);

    if ( RetVal == LZ4_OK ) {
        *WorkSize = (VIGNETTE_MAX_SIZE_INTERNAL*2*sizeof(INT16)) + LZ4_sizeofStateHC();
    }

    return RetVal;
}

UINT32 AmbaLZ4_VigGetTblEncBound(INT32 TableCount, const INT32 SrcTableLen[], UINT32 *SizeBound)
{
    UINT32 RetVal = LZ4_OK;
    INT32 TableIdx;

    RetVal |= ctx_check_valid_pointer(SrcTableLen);
    RetVal |= ctx_check_valid_pointer(SizeBound);
    RetVal |= ctx_check_valid_pointer(SizeBound);

    if ( RetVal == LZ4_OK ) {
        *SizeBound = 0;

        for (TableIdx = 0; TableIdx<TableCount; TableIdx++) {

            RetVal |= ctx_check_vignette_table_length(SrcTableLen[TableIdx]);

            if ( RetVal == LZ4_OK ) {
                *SizeBound += LZ4_compressBound(
                    SrcTableLen[TableIdx]*sizeof(INT16));
            } else {
                break;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaLZ4_VigTblEncode(INT32 TableCount, const UINT16* const SrcTable[],
    const INT32 SrcTableWidth[], const INT32 SrcTableHeight[],
    INT8 *Dst, INT32 DstInputSize,
    INT32 DstOutputSize[],
    INT16 *Workplace)
{
    UINT32 RetVal = LZ4_OK;
    INT32 BufOffset;
    INT32 EncodeSize;
    INT32 TblIdx;
    UINT32 Width, Height;

    INT32 DstRemainSize;

    //Working Buffer
    INT16 *VigVal = NULL;
    INT16 *VigDiffVal = NULL;
    INT16 *LZ4Workplace = NULL;

    RetVal |= ctx_check_valid_pointer(SrcTableWidth);
    RetVal |= ctx_check_valid_pointer(SrcTableHeight);
    RetVal |= ctx_check_valid_pointer(Dst);
    RetVal |= ctx_check_valid_pointer(DstOutputSize);
    RetVal |= ctx_check_valid_pointer(Workplace);

    if ( RetVal == LZ4_OK ) {
        VigVal = Workplace;
        VigDiffVal = &Workplace[VIGNETTE_MAX_SIZE_INTERNAL];
        LZ4Workplace = &Workplace[VIGNETTE_MAX_SIZE_INTERNAL*2];

        BufOffset = 0;
        DstRemainSize = DstInputSize;

        for (TblIdx = 0; TblIdx < TableCount; TblIdx++) {
            UINT16 *Src;

            RetVal |= ctx_check_valid_pointer(SrcTable[TblIdx]);
            RetVal |= ctx_check_vignette_table_gridNum(SrcTableWidth[TblIdx], SrcTableHeight[TblIdx]);

            if ( RetVal == LZ4_OK ) {
                Width = SrcTableWidth[TblIdx];
                Height = SrcTableHeight[TblIdx];
                Src = SrcTable[TblIdx];

                RetVal |= lz4_memcpy(VigVal, Src, Width*Height*sizeof(INT16));

                // Copy First row
                RetVal |= lz4_memcpy(VigDiffVal, VigVal, sizeof(INT16)*Width);

                #if (defined(DIFF_OPTIMIZE)) && (DIFF_OPTIMIZE == 1)
                    VigTbl_RowDiff(VigVal, VigDiffVal, Width, Height);
                #else
                {
                    UINT32 I, J;
                    for (I = 1; I < Height; I++) {
                        for (J = 0; J < Width; J++) {
                            VigDiffVal[(I*Width) + J] = VigVal[((I-1)*Width) + J] - VigVal[(I*Width) + J];
                        }
                    }
                }
                #endif

                DstOutputSize[TblIdx] = 0;
                EncodeSize = LZ4_compress_HC_Feed_workSpace((void*)VigDiffVal,
                    (void*)(Dst + BufOffset), Width * Height * sizeof(INT16),
                    DstRemainSize, 9, LZ4Workplace);
                if( EncodeSize == 0 ) {
                    RetVal = LZ4_NG_2;
                    break;
                }
                BufOffset += EncodeSize;
                DstRemainSize -= EncodeSize;
                DstOutputSize[TblIdx] = EncodeSize;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaLZ4_VigTblDecode(INT32 TableCount,
    INT8 *Input,
    const INT32 InputSize[],
    const INT32 SrcTableWidth[], const INT32 SrcTableHeight[],
    UINT16* const DstVigTable[],
    INT16 *Workplace)
{
    UINT32 RetVal = LZ4_OK;

    //Working Buffer
    INT16 *VigVal = NULL;
    INT16 *VigDiffVal = NULL;

    INT8 *EncBuf;
    INT32 TblIdx;

    RetVal |= ctx_check_valid_pointer(Input);
    RetVal |= ctx_check_valid_pointer(InputSize);
    RetVal |= ctx_check_valid_pointer(SrcTableWidth);
    RetVal |= ctx_check_valid_pointer(SrcTableHeight);
    RetVal |= ctx_check_valid_pointer(Workplace);

    if ( RetVal == LZ4_OK ) {
        VigVal = Workplace;
        VigDiffVal = &Workplace[VIGNETTE_MAX_SIZE_INTERNAL];

        EncBuf = Input;
        for (TblIdx=0; TblIdx < TableCount; TblIdx++) {
            INT32 DecompressedSize;
            INT32 Width, Height;

            RetVal |= ctx_check_valid_pointer(DstVigTable[TblIdx]);
            RetVal |= ctx_check_vignette_table_gridNum(SrcTableWidth[TblIdx], SrcTableHeight[TblIdx]);

            if ( RetVal == LZ4_OK ) {
                Width = SrcTableWidth[TblIdx];
                Height = SrcTableHeight[TblIdx];

                DecompressedSize = LZ4_decompress_safe((char*)EncBuf,
                    (char*)VigDiffVal,
                    InputSize[TblIdx],
                    VIGNETTE_MAX_SIZE_INTERNAL * sizeof(UINT16));
                if( DecompressedSize <= 0) {
                    RetVal = LZ4_NG_2;;
                    break;
                }

                // Recover Y direction differential
                RetVal |= lz4_memcpy(VigVal, VigDiffVal, sizeof(INT16)*Width);

            #if (defined(DIFF_OPTIMIZE)) && (DIFF_OPTIMIZE == 1)
                VigTbl_RowDiffRecover(VigVal, VigDiffVal, Width, Height);
            #else
                {
                    INT32 I, J;
                    for (I = 1; I < Height; I++) {
                        for (J = 0; J < Width; J++) {
                             VigVal[(I*Width) + J] = VigVal[((I-1)*Width) + J] - VigDiffVal[(I*Width) + J];
                        }
                    }
                }
            #endif

                RetVal |= lz4_memcpy(DstVigTable[TblIdx], VigVal, Width * Height * sizeof(INT16));
                EncBuf += InputSize[TblIdx];
            } else {
                break;
            }
        }
    }

    return RetVal;
}
#endif
UINT32 AmbaLZ4_TblDecodeSegment(INT32 TableCount, const UINT8 *Input,
    UINT8* const DstTable[], INT32 SegmentSize, void *CachedWorkplace)
{
    UINT32 RetVal = LZ4_OK;
    INT32 TblIdx;
    INT32 DecompressedSize;
    INT32 TblBufOffset;
    RetVal |= ctx_check_valid_pointer(Input);

    if ( RetVal == LZ4_OK ) {
        TblBufOffset = 0;
        for (TblIdx = 0; TblIdx < TableCount; TblIdx++) {
            RetVal |= ctx_check_valid_pointer(DstTable[TblIdx]);
            if ( RetVal == LZ4_OK ) {
                UINT32 NumOfSegments;
                UINT32 RemainSegments;
                UINT32 EncodeSegmentSize;
                UINT32 EncodeDataStartOffset;
                UINT32 EncodeDataOffset;
                UINT32 SegIdx;
                INT32  DstBufOffset;

                NumOfSegments = *(UINT32*)(Input + TblBufOffset);
                RemainSegments = NumOfSegments;
                EncodeDataStartOffset = sizeof(UINT32) + 2*NumOfSegments*sizeof(UINT32);
                EncodeDataOffset = EncodeDataStartOffset;
                SegIdx = 0;
                DstBufOffset = 0;
                while ( RemainSegments > 0 ) {
                    EncodeSegmentSize = *(UINT32*)(Input + TblBufOffset + sizeof(UINT32) + 2*SegIdx*sizeof(UINT32));
                    DecompressedSize = LZ4_decompress_safe((char*)Input + TblBufOffset + EncodeDataOffset,(char*)CachedWorkplace, EncodeSegmentSize, SegmentSize);
                    if( DecompressedSize <= 0 ) {
                       return LZ4_NG_1;
                    }
                    RetVal |= lz4_memcpy(DstTable[TblIdx] + DstBufOffset, CachedWorkplace, DecompressedSize);
                    EncodeDataOffset += EncodeSegmentSize;
                    DstBufOffset += DecompressedSize;
                    RemainSegments--;
                    SegIdx++;
                }
                TblBufOffset += EncodeDataOffset;
            } else {
                break;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaLZ4_TblDecodeSegmentToWin(INT32 TableCount, const UINT8 *Input,
    UINT8* const DstTable[], const AMBA_LZ4_TBL_SIZE_s DstTableSize[], const AMBA_LZ4_WIN_s DstWinInfo[],
    INT32 SegmentSize, void *CachedWorkplace)
{
    UINT32 RetVal = LZ4_OK;
    INT32 TblIdx;
    INT32 DecompressedSize;
    UINT32 TobeCopiedSize;
    UINT32 WinXOffset;
    INT32 TblBufOffset;

    RetVal |= ctx_check_valid_pointer(Input);
    RetVal |= ctx_check_valid_pointer(DstTableSize);
    RetVal |= ctx_check_valid_pointer(DstWinInfo);
    if ( RetVal == LZ4_OK ) {
        TblBufOffset = 0;
        for (TblIdx = 0; TblIdx < TableCount; TblIdx++) {
            RetVal |= ctx_check_valid_pointer(DstTable[TblIdx]);
            if ( ((DstWinInfo[TblIdx].StartX + DstWinInfo[TblIdx].Width) > DstTableSize[TblIdx].Width) ||
                ((DstWinInfo[TblIdx].StartY + DstWinInfo[TblIdx].Height) > DstTableSize[TblIdx].Height) ) {
                RetVal = LZ4_NG_1;
            }
            if ( RetVal == LZ4_OK ) {
                UINT32 NumOfSegments;
                UINT32 RemainSegments;
                UINT32 EncodeSegmentSize;
                UINT32 EncodeDataStartOffset;
                UINT32 EncodeDataOffset;
                UINT32 SegIdx;
                INT32  DstBufOffset;
                UINT32 StrideToNextLine;

                StrideToNextLine = DstTableSize[TblIdx].Width - DstWinInfo[TblIdx].Width;
                NumOfSegments = *(UINT32*)(Input + TblBufOffset);
                RemainSegments = NumOfSegments;
                EncodeDataStartOffset = sizeof(UINT32) + 2*NumOfSegments*sizeof(UINT32);
                EncodeDataOffset = EncodeDataStartOffset;
                SegIdx = 0;
                DstBufOffset = DstWinInfo[TblIdx].StartY*DstTableSize[TblIdx].Width + DstWinInfo[TblIdx].StartX;
                WinXOffset = 0;

                while ( RemainSegments > 0 ) {
                    UINT32 CachedWorkplaceOffset;

                    EncodeSegmentSize = *(UINT32*)(Input + TblBufOffset + sizeof(UINT32) + 2*SegIdx*sizeof(UINT32));
                    DecompressedSize = LZ4_decompress_safe(
                        (char*)Input + TblBufOffset + EncodeDataOffset,
                        (char*)CachedWorkplace, EncodeSegmentSize, SegmentSize);
                    if( DecompressedSize <= 0 ) {
                        return LZ4_NG_1;
                    }
                    TobeCopiedSize = DecompressedSize;
                    CachedWorkplaceOffset = 0;
                    while( TobeCopiedSize > 0 ) {
                        if ( TobeCopiedSize >= (DstWinInfo[TblIdx].Width - WinXOffset) ) {
                            INT32 TruncatedSize;

                            // Copy to end of line, and change to next line
                            TruncatedSize = DstWinInfo[TblIdx].Width - WinXOffset;
                            RetVal |= lz4_memcpy(DstTable[TblIdx] + DstBufOffset, ((char*)CachedWorkplace) + CachedWorkplaceOffset, TruncatedSize);

                            WinXOffset = 0;
                            CachedWorkplaceOffset += TruncatedSize;
                            DstBufOffset += (TruncatedSize + StrideToNextLine);
                            TobeCopiedSize -= TruncatedSize;
                        } else {
                            // Last line
                            RetVal |= lz4_memcpy(DstTable[TblIdx] + DstBufOffset, ((char*)CachedWorkplace) + CachedWorkplaceOffset, TobeCopiedSize);

                            WinXOffset += TobeCopiedSize;
                            DstBufOffset += TobeCopiedSize;
                            TobeCopiedSize = 0;
                        }
                    }

                    EncodeDataOffset += EncodeSegmentSize;
                    RemainSegments--;
                    SegIdx++;
                }
                TblBufOffset += EncodeDataOffset;
            } else {
                break;
            }
        }
    }
    return RetVal;
}

UINT32 AmbaLZ4_TblEncodeSegment(INT32 TableCount, const UINT8* const SrcTable[],
    const INT32 SrcTablelen[], INT8 *Dst, INT32 DstInputSize,
    INT32 DstOutputSize[],
    INT32 SegmentSize,
    const INT16* Workplace)
{
    UINT32 RetVal = LZ4_OK;
    INT32 TblBufOffset;
    UINT32 EncodeSize;
    INT32 TblIdx;
    INT32 DstRemainSize;

    RetVal |= ctx_check_valid_pointer(SrcTablelen);
    RetVal |= ctx_check_valid_pointer(Dst);
    RetVal |= ctx_check_valid_pointer(DstOutputSize);
    RetVal |= ctx_check_valid_pointer(Workplace);

    if ( RetVal == LZ4_OK ) {
        TblBufOffset = 0;
        DstRemainSize = DstInputSize;
        for (TblIdx = 0; TblIdx<TableCount; TblIdx++) {
            RetVal |= ctx_check_valid_pointer(SrcTable[TblIdx]);
            if ( RetVal == LZ4_OK ) {
                INT32 SrcRemainSize;
                UINT32 NumOfSegments;
                INT32 EncSegmentSize;
                UINT32 SrcOffset;
                UINT32 DstDataStartOffset;
                UINT32 DstDataOffset;
                UINT32 SegIdx;

                NumOfSegments = (SrcTablelen[TblIdx] + SegmentSize - 1) / SegmentSize;
                RetVal |= lz4_memcpy(Dst + TblBufOffset, &NumOfSegments, sizeof(UINT32));
                DstDataStartOffset = sizeof(UINT32) + 2*sizeof(UINT32)*NumOfSegments;
                DstDataOffset = DstDataStartOffset;
                DstRemainSize -= DstDataStartOffset;
                SrcRemainSize = SrcTablelen[TblIdx];
                SrcOffset = 0;
                SegIdx = 0;

                while ( SrcRemainSize > 0 ) {
                    if ( SrcRemainSize >= SegmentSize ) {
                        EncSegmentSize = SegmentSize;
                    } else {
                        EncSegmentSize = SrcRemainSize;
                    }

                    EncodeSize = LZ4_compress_HC_Feed_workSpace((void*)(SrcTable[TblIdx] + SrcOffset),
                        (void*)(Dst + TblBufOffset + DstDataOffset), EncSegmentSize,
                        DstRemainSize, 9, Workplace);

                    if( EncodeSize == 0 ) {
                        return LZ4_NG_1;
                    }
                    SrcRemainSize -= EncSegmentSize;
                    SrcOffset += EncSegmentSize;
                    DstDataOffset += EncodeSize;
                    DstRemainSize -= EncodeSize;
                    RetVal |= lz4_memcpy(Dst + TblBufOffset + sizeof(UINT32) + 2*SegIdx*sizeof(UINT32), &EncodeSize, sizeof(UINT32));
                    RetVal |= lz4_memcpy(Dst + TblBufOffset + sizeof(UINT32) + (2*SegIdx + 1)*sizeof(UINT32), &EncSegmentSize, sizeof(UINT32));
                    SegIdx++;
                }
                TblBufOffset += DstDataOffset;
                DstOutputSize[TblIdx] = DstDataOffset;
            } else {
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 Get4Bytes(UINT8 *pData)
{
    UINT32  Val = 0U;

    Val |= pData[3];
    Val <<= 8U;
    Val |= pData[2];
    Val <<= 8U;
    Val |= pData[1];
    Val <<= 8U;
    Val |= pData[0];

    return Val;
}

UINT32 AmbaLZ4_DeCompressFile(UINT8 *pSrc, UINT8 *pDst, UINT32 SrcSize, UINT32 DstSize)
{
    #define HDR_BDMAX_64KB      (4U)
    #define BLK_UNCOMPRESS_BIT  (0x80000000U)
    #define MAX_BLK_SIZE        (0x10000)

    extern void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3
                                                     , UINT32 Arg4, UINT32 Arg5);

    typedef struct {
        UINT32  MagicNb;
        UINT8   FlgVer;
        UINT8   FlgBIndp;
        UINT8   FlgBCkSm;
        UINT8   FlgCtSize;
        UINT8   FlgCtCkSm;
        UINT8   FlgCtDict;
        UINT8   BdBMaxSz;
        UINT8   HC;
    } LZ4_FRAME_FMT_s;

    UINT8            *pSData = pSrc, Temp1B;
    UINT8            *pDData = pDst;
    UINT32           Rval, Temp4B;
    UINT32           CurSrcSize = 0U, CurDstSize = 0U;
    INT32            BlkSize, DcSize;
    LZ4_FRAME_FMT_s  FrmFmt;

    /* MagicNb(4Bytes), 0x184D2204 */
    FrmFmt.MagicNb = Get4Bytes(&(pSData[0]));
    /* FLG(1Byte) */
    Temp1B = pSData[4];
    FrmFmt.FlgVer    = (Temp1B & 0xC0U) >> 6U;
    FrmFmt.FlgBIndp  = (Temp1B & 0x20U) >> 5U;
    FrmFmt.FlgBCkSm  = (Temp1B & 0x10U) >> 4U;
    FrmFmt.FlgCtSize = (Temp1B & 0x08U) >> 3U;
    FrmFmt.FlgCtCkSm = (Temp1B & 0x04U) >> 2U;
    FrmFmt.FlgCtDict = (Temp1B & 0x01U);
    /* BD(1Byte), 4-64KB, 5-256KB, 6-1MB, 7-4MB, support 64KB only */
    Temp1B = pSData[5];
    FrmFmt.BdBMaxSz = (Temp1B & 0x70U) >> 4U;
    /* HC(1Byte) */
    FrmFmt.HC = pSData[6];

#if 0
    AmbaPrint_PrintUInt5("## MagicNb = 0x%X", FrmFmt.MagicNb, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("## FlgVer = %d", FrmFmt.FlgVer, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("## FlgBIndp = %d", FrmFmt.FlgBIndp, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("## FlgBCkSm = %d", FrmFmt.FlgBCkSm, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("## FlgCtSize = %d", FrmFmt.FlgCtSize, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("## FlgCtCkSm = %d", FrmFmt.FlgCtCkSm, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("## FlgCtDict = %d", FrmFmt.FlgCtDict, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("## BdBMaxSz = %d", FrmFmt.BdBMaxSz, 0U, 0U, 0U, 0U);
#endif

    Rval = 0U;
    if (FrmFmt.BdBMaxSz != HDR_BDMAX_64KB) {
        //AmbaPrint_PrintUInt5("## just support 64kb max block data size", 0U, 0U, 0U, 0U, 0U);
    } else {
        pSData = &(pSData[7]);
        do {
            Temp4B  = Get4Bytes(&(pSData[0]));
            BlkSize = ClearBits(Temp4B, BLK_UNCOMPRESS_BIT);
            pSData = &(pSData[4]);

            if (BlkSize != 0U) {
                if (0U < CheckBits(Temp4B, BLK_UNCOMPRESS_BIT)) {
                    DcSize = BlkSize;
                    Rval |= lz4_memcpy(pDData, pSData, DcSize);
                } else {
                    DcSize = LZ4_decompress_safe(pSData, pDData, BlkSize, MAX_BLK_SIZE);
                }

                Rval = LZ4_NG_2;
                if (0 < DcSize) {
                    CurSrcSize += BlkSize;
                    CurDstSize += DcSize;
                    if ((CurSrcSize <= SrcSize) && (CurDstSize <= DstSize)) {
                        pSData = &(pSData[BlkSize]);
                        pDData = &(pDData[DcSize]);
                        Rval = LZ4_OK;
                    }
                }

                if (Rval != LZ4_OK) {
                    //AmbaPrint_PrintUInt5("## something wrong", 0U, 0U, 0U, 0U, 0U);
                    break;
                }
            }
        } while (BlkSize != 0U);

        if (Rval == LZ4_OK) {
            Rval = CurDstSize;
        }
    }

    return Rval;
}
