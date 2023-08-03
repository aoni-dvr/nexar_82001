
#include "lz4.h"
#include "lz4hc.h"

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaDef.h"

#define LZ4_OK      (0U)
#define LZ4_NG_1    (1U) // invalid param
#define LZ4_NG_2    (2U) // internal error

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

    //AmbaPrint_PrintUInt5("## MagicNb = 0x%X", FrmFmt.MagicNb, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("## FlgVer = %d", FrmFmt.FlgVer, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("## FlgBIndp = %d", FrmFmt.FlgBIndp, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("## FlgBCkSm = %d", FrmFmt.FlgBCkSm, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("## FlgCtSize = %d", FrmFmt.FlgCtSize, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("## FlgCtCkSm = %d", FrmFmt.FlgCtCkSm, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("## FlgCtDict = %d", FrmFmt.FlgCtDict, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("## BdBMaxSz = %d", FrmFmt.BdBMaxSz, 0U, 0U, 0U, 0U);

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
                    Rval |= AmbaWrap_memcpy(pDData, pSData, DcSize);
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
