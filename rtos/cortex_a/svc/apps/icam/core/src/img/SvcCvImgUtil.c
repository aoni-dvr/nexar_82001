/**
 *  @file SvcCvImgUtil.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc application cv image utility
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcCvImgUtil.h"

static inline void RescaleBitMapLine_Rept(UINT8 *pSrc, UINT32 SrcWidth,
                                          UINT8 *pDst, UINT32 DstWidth,
                                          UINT8 *pHorizantalMap,
                                          UINT32 HorizantalMapLen,
                                          UINT8 ColorOffset);
static inline void RescaleBitMapLine_U8TOU32(UINT8 *pSrc, UINT32 SrcWidth,
                                             UINT32 *pDst, UINT32 DstWidth,
                                             UINT8 *pHorizantalMap,
                                             UINT32 HorizantalMapLen,
                                             UINT32 *pClut,
                                             UINT32 NumColor);

/**
 *  Find out the best/close scaling table to meet user target
 *  @param[in] ScaleValue Scale value
 *  @param[out] ppTable Scale table
 *  @param[out] pTableLength Scale table size
 */
void SvcCvImgUtil_GetBitMapScaleTable(DOUBLE ScaleValue, UINT8 **ppTable, UINT32 *pTableLength)
{
    DOUBLE TargetVgap = 0.0, CompareVgap = 999999.0;
    UINT32 i, Idx = 0;
    static UINT8  Map_64_45[64]    = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0,
                                      1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
                                      1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
                                      1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0};

    static UINT8  Map_160_113[160] = {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
                                      1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
                                      1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
                                      0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
                                      1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
                                      1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
                                      1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
                                      1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
                                      0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
                                      1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0};

    static UINT8  Map_4_3[4]       = {1, 1, 1, 0};

    static UINT8  Map_32_45[32]    = {2, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2,
                                      1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1};

    static UINT8  Map_113_160[113] = {1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2,
                                      1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1,
                                      2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2,
                                      1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2,
                                      1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1,
                                      2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2,
                                      1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2,
                                      1};

    static UINT8  Map_16_45[16]    = {3, 3, 3, 3, 2, 3, 3, 3, 3, 2, 3, 3, 3, 3, 2, 3};

    static UINT8  Map_8_45[8]      = {6, 5, 6, 6, 5, 6, 5, 6};

    static UINT8  Map_128_85[128]  = {1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      0, 1};

    static UINT8  Map_4_15[4]      = {4, 4, 4, 3};

    static UINT8  Map_16_17[17]    = {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1};

    static UINT8  Map_1_1[4]       = {1, 1, 1, 1};

    static UINT8  Map_4_30[4]      = {8, 8, 8, 6};

    static UINT8  Map_22_15[22]    = {1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                                      1, 0, 1, 1, 0, 1, 1, 0, 1, 1,
                                      0, 1};

    static UINT8  Map_33_23[33]    = {1, 1, 0, 1, 1, 0, 1, 1, 1, 0,
                                      1, 1, 0, 1, 1, 0, 1, 1, 1, 0,
                                      1, 1, 0, 1, 1, 0, 1, 1, 1, 0,
                                      1, 1, 0};

    static UINT8  Map_5_6[5]       = {1, 1, 1, 1, 2};

    static UINT8  Map_32_27[32]    = {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
                                      0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1};

    #define SVC_CV_PIXEL_MAP_NUMBER_MAX     (16U)
    static SVC_BIT_MAP_PIXEL_MAP_s BitScaleMap[SVC_CV_PIXEL_MAP_NUMBER_MAX] =
    {
        [0] = {.Value = 0.6640625/* 85/128*/, .pTable = Map_128_85 , .TableLength = (UINT32)sizeof(Map_128_85) },
        [1] = {.Value = 0.681818 /* 15/22 */, .pTable = Map_22_15  , .TableLength = (UINT32)sizeof(Map_22_15)  },
        [2] = {.Value = 0.696969 /* 23/33 */, .pTable = Map_33_23  , .TableLength = (UINT32)sizeof(Map_33_23)  },
        [3] = {.Value = 0.703125 /* 45/64 */, .pTable = Map_64_45  , .TableLength = (UINT32)sizeof(Map_64_45)  },
        [4] = {.Value = 0.70625  /*113/160*/, .pTable = Map_160_113, .TableLength = (UINT32)sizeof(Map_160_113)},
        [5] = {.Value = 0.75     /*  3/4  */, .pTable = Map_4_3    , .TableLength = (UINT32)sizeof(Map_4_3)    },
        [6] = {.Value = 0.84375  /* 27/32 */, .pTable = Map_32_27  , .TableLength = (UINT32)sizeof(Map_32_27)  },
        [7] = {.Value = 0.9411   /* 16/17 */, .pTable = Map_16_17  , .TableLength = (UINT32)sizeof(Map_16_17)  },
        [8] = {.Value = 1.00     /* 1     */, .pTable = Map_1_1    , .TableLength = (UINT32)sizeof(Map_1_1)    },
        [9] = {.Value = 1.40625  /* 45/32 */, .pTable = Map_32_45  , .TableLength = (UINT32)sizeof(Map_32_45)  },
        [10]= {.Value = 1.2      /* 6/5   */, .pTable = Map_5_6    , .TableLength = (UINT32)sizeof(Map_5_6)    },
        [11]= {.Value = 1.415929 /*160/113*/, .pTable = Map_113_160, .TableLength = (UINT32)sizeof(Map_113_160)},
        [12]= {.Value = 2.8125   /* 45/16 */, .pTable = Map_16_45  , .TableLength = (UINT32)sizeof(Map_16_45)  },
        [13]= {.Value = 3.75     /* 15/4  */, .pTable = Map_4_15   , .TableLength = (UINT32)sizeof(Map_4_15)   },
        [14]= {.Value = 5.625    /* 45/8  */, .pTable = Map_8_45   , .TableLength = (UINT32)sizeof(Map_8_45)   },
        [15]= {.Value = 7.5      /* 30/4  */, .pTable = Map_4_30   , .TableLength = (UINT32)sizeof(Map_4_30)   }
    };

    AmbaMisra_TouchUnused(Map_64_45);
    AmbaMisra_TouchUnused(Map_160_113);
    AmbaMisra_TouchUnused(Map_4_3);
    AmbaMisra_TouchUnused(Map_32_45);
    AmbaMisra_TouchUnused(Map_113_160);
    AmbaMisra_TouchUnused(Map_16_45);
    AmbaMisra_TouchUnused(Map_8_45);
    AmbaMisra_TouchUnused(Map_128_85);
    AmbaMisra_TouchUnused(Map_16_17);
    AmbaMisra_TouchUnused(Map_1_1);
    AmbaMisra_TouchUnused(Map_4_30);
    AmbaMisra_TouchUnused(Map_4_15);
    AmbaMisra_TouchUnused(Map_22_15);
    AmbaMisra_TouchUnused(Map_33_23);
    AmbaMisra_TouchUnused(Map_5_6);
    AmbaMisra_TouchUnused(Map_32_27);

    for(i= 0U; i< SVC_CV_PIXEL_MAP_NUMBER_MAX; i++) {
        if (0U == i) {
            if (ScaleValue > BitScaleMap[i].Value) {
                TargetVgap = ScaleValue - BitScaleMap[i].Value;
            } else {
                TargetVgap = BitScaleMap[i].Value - ScaleValue;
            }
            Idx = i;
        }
        if (ScaleValue == BitScaleMap[i].Value) {/* Value Hit */
            Idx = i;
            break;
        } else {
            if (ScaleValue > BitScaleMap[i].Value) {
                CompareVgap = ScaleValue - BitScaleMap[i].Value;
            } else {
                CompareVgap = BitScaleMap[i].Value - ScaleValue;
            }
            if (TargetVgap > CompareVgap) {
                TargetVgap = CompareVgap;
                Idx = i;
            }
        }
    }
    *ppTable      = BitScaleMap[Idx].pTable;
    *pTableLength = BitScaleMap[Idx].TableLength;
    AmbaPrint_PrintUInt5("SvcCvImgUtil_GetBitMapScaleTable()ScaleTable Idx = %d, TableSize = %d", Idx, *pTableLength, 0U, 0U, 0U);
}

/**
 *  Rescale bitmap to specific size with skipping or repeating
 *  @param[in] pCfg Rescale configuration
 *  @return 0-OK, 1-NG
 */
UINT32 SvcCvImgUtil_RescaleBitMap_Rept(SVC_BIT_MAP_RESCALE_REPT_s *pCfg)
{
    UINT32  RetVal           = SVC_OK;
    UINT32  SrcWidth         = pCfg->SrcWidth;
    UINT32  SrcHeight        = pCfg->SrcHeight;
    UINT32  SrcPitch         = pCfg->SrcPitch;
    UINT32  DstWidth         = pCfg->DstWidth;
    UINT32  DstHeight        = pCfg->DstHeight;
    UINT32  DstPitch         = pCfg->DstPitch;
    UINT32  HorizantalMapLen = pCfg->HorizantalMapLen;
    UINT32  VerticalMapLen   = pCfg->VerticalMapLen;
    UINT8   ColorOffset      = pCfg->ColorOffset;
    UINT8  *pHorizantalMap   = pCfg->pHorizantalMap;
    UINT8  *pVerticalMap     = pCfg->pVerticalMap;
    UINT32  SrcStartY        = pCfg->SrcStartY;
    UINT32 i, j;
    UINT32 Dst_i = 0, End = 0;
    UINT32 Remainder, RepeatCnt;
    UINT8  *pSrcBufAddr, *pDstBufAddr;
    ULONG   SrcBufAddr, DstBufAddr;
    UINT8 *pRepeatLine = NULL;

    AmbaMisra_TouchUnused(pCfg);
    AmbaMisra_TouchUnused(pHorizantalMap);
    AmbaMisra_TouchUnused(pVerticalMap);
    AmbaMisra_TouchUnused(pRepeatLine);

#if 0
    AmbaPrint_PrintUInt5("[SvcCvImgUtil_RescaleBitMap_Rept]: Src %u %u %u, 0x%x", SrcWidth, SrcHeight, SrcPitch, (UINT32) pCfg->pSrc, 0U);
    AmbaPrint_PrintUInt5("Dst %u %u %u, 0x%x", DstWidth, DstHeight, DstPitch, (UINT32)pCfg->pDst, 0U);
    AmbaPrint_PrintUInt5("PixelMap len %u 0x%x, LineMap len %u 0x%x", HorizantalMapLen, (UINT32) pHorizantalMap, VerticalMapLen, (UINT32) pVerticalMap, 0U);
    AmbaPrint_Flush();
#endif

    if ((NULL != pCfg->pSrc) &&
        (NULL != pCfg->pDst) &&
        (NULL != pCfg->pHorizantalMap) &&
        (NULL != pCfg->pVerticalMap)) {
        /* First line */
        AmbaMisra_TypeCast(&SrcBufAddr, &pCfg->pSrc);
        AmbaMisra_TypeCast(&DstBufAddr, &pCfg->pDst);

        for (i = 0; i < SrcHeight; i++) {
            Remainder = (i % VerticalMapLen);
            RepeatCnt = pVerticalMap[Remainder];

            //AmbaPrint_PrintUInt5("#i = %u, Rem %u, Rep %u", i, Remainder, RepeatCnt, 0U, 0U);

            for (j = 0; j < RepeatCnt; j++) {
                //AmbaPrint_PrintUInt5(" j = %u, Dst_i = %u, pSrc 0x%x, pDst 0x%x",
                //                        j, Dst_i, (UINT32)SrcBufAddr, (UINT32)DstBufAddr, 0U);
                if (i >= SrcStartY) {
                    if (j == 0U) {
                        AmbaMisra_TypeCast(&pSrcBufAddr, &SrcBufAddr);
                        AmbaMisra_TypeCast(&pDstBufAddr, &DstBufAddr);
                        RescaleBitMapLine_Rept(pSrcBufAddr,
                                               SrcWidth,
                                               pDstBufAddr,
                                               DstWidth,
                                               pHorizantalMap,
                                               HorizantalMapLen,
                                               ColorOffset);
                        AmbaMisra_TypeCast(&pRepeatLine, &DstBufAddr);
                    } else {
                        /* Copy from previous line */
                        AmbaMisra_TypeCast(&pDstBufAddr, &DstBufAddr);
                        AmbaSvcWrap_MisraMemcpy(pDstBufAddr, pRepeatLine, DstWidth);
                    }
                }

                /* Move to next line */
                DstBufAddr += DstPitch;
                Dst_i++;
                if (Dst_i >= DstHeight) {
                    End = 1U;
                    break;
                }
            }

            if (End != 1U) {
                /* Move to next line */
                SrcBufAddr += SrcPitch;
            } else {
                break;
            }
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Rescale bitmap to specific size with skipping or repeating, and color conversion
 *  @param[in] pCfg Rescale configuration
 *  @return 0-OK, 1-NG
 */
UINT32 SvcCvImgUtil_RescaleBitMap_Idx2Color(SVC_BIT_MAP_RESCALE_REPT_s *pCfg)
{
    UINT32  RetVal           = SVC_OK;
    UINT32  SrcWidth         = pCfg->SrcWidth;
    UINT32  SrcHeight        = pCfg->SrcHeight;
    UINT32  SrcPitch         = pCfg->SrcPitch;
    UINT32  DstWidth         = pCfg->DstWidth;
    UINT32  DstHeight        = pCfg->DstHeight;
    UINT32  DstPitch         = pCfg->DstPitch;
    UINT32  HorizantalMapLen = pCfg->HorizantalMapLen;
    UINT32  VerticalMapLen   = pCfg->VerticalMapLen;
    UINT8  *pHorizantalMap   = pCfg->pHorizantalMap;
    UINT8  *pVerticalMap     = pCfg->pVerticalMap;
    UINT32 *pClut            = pCfg->pClut;
    UINT32  NumColor         = pCfg->NumColor;
    UINT32  SrcStartY        = pCfg->SrcStartY;
    UINT32  i, j;
    UINT32  Dst_i = 0, End = 0;
    UINT32  Remainder, RepeatCnt;
    UINT8  *pSrcBufAddr;
    UINT32 *pDstBufU32;
    ULONG   SrcBufAddr, DstBufAddr;
    UINT8  *pRepeatLine = NULL;

    AmbaMisra_TouchUnused(pCfg);
    AmbaMisra_TouchUnused(pHorizantalMap);
    AmbaMisra_TouchUnused(pVerticalMap);
    AmbaMisra_TouchUnused(pRepeatLine);
    AmbaMisra_TouchUnused(pClut);

#if 0
    AmbaPrint_PrintUInt5("[SvcCvImgUtil_RescaleBitMap_C2C]: Src %u %u %u, 0x%x", SrcWidth, SrcHeight, SrcPitch, (UINT32) pCfg->pSrc, 0U);
    AmbaPrint_PrintUInt5("Dst %u %u %u, 0x%x", DstWidth, DstHeight, DstPitch, (UINT32)pCfg->pDst, 0U);
    AmbaPrint_PrintUInt5("PixelMap len %u 0x%x, LineMap len %u 0x%x", HorizantalMapLen, (UINT32) pHorizantalMap, VerticalMapLen, (UINT32) pVerticalMap, 0U);
    AmbaPrint_Flush();
#endif

    if ((NULL != pCfg->pSrc) &&
        (NULL != pCfg->pDst) &&
        (NULL != pCfg->pHorizantalMap) &&
        (NULL != pCfg->pVerticalMap) &&
        (NULL != pCfg->pClut)) {
        /* First line */
        AmbaMisra_TypeCast(&SrcBufAddr, &pCfg->pSrc);
        AmbaMisra_TypeCast(&DstBufAddr, &pCfg->pDst);

        for (i = 0; i < SrcHeight; i++) {
            Remainder = (i % VerticalMapLen);
            RepeatCnt = pVerticalMap[Remainder];

            //AmbaPrint_PrintUInt5("#i = %u, Rem %u, Rep %u", i, Remainder, RepeatCnt, 0U, 0U);

            for (j = 0; j < RepeatCnt; j++) {
                //AmbaPrint_PrintUInt5(" j = %u, Dst_i = %u, pSrc 0x%x, pDst 0x%x",
                //                        j, Dst_i, (UINT32)SrcBufAddr, (UINT32)DstBufAddr, 0U);
                if (i >= SrcStartY) {
                    if (j == 0U) {
                        AmbaMisra_TypeCast(&pSrcBufAddr, &SrcBufAddr);
                        AmbaMisra_TypeCast(&pDstBufU32, &DstBufAddr);
                        RescaleBitMapLine_U8TOU32(pSrcBufAddr,
                                                  SrcWidth,
                                                  pDstBufU32,
                                                  DstWidth,
                                                  pHorizantalMap,
                                                  HorizantalMapLen,
                                                  pClut, NumColor);
                        AmbaMisra_TypeCast(&pRepeatLine, &DstBufAddr);
                    } else {
                        /* Copy from previous line */
                        AmbaMisra_TypeCast(&pDstBufU32, &DstBufAddr);
                        AmbaSvcWrap_MisraMemcpy(pDstBufU32, pRepeatLine, (UINT32)(DstWidth << 2U));
                    }
                }

                /* Move to next line */
                DstBufAddr += DstPitch;
                Dst_i++;
                if (Dst_i >= DstHeight) {
                    End = 1U;
                    break;
                }
            }

            if (End != 1U) {
                /* Move to next line */
                SrcBufAddr += SrcPitch;
            } else {
                break;
            }
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static inline void RescaleBitMapLine_Rept(UINT8 *pSrc, UINT32 SrcWidth,
                                          UINT8 *pDst, UINT32 DstWidth,
                                          UINT8 *pHorizantalMap,
                                          UINT32 HorizantalMapLen,
                                          UINT8 ColorOffset)
{
#if 0
    UINT32 i, j;
    UINT32 Dst_i = 0, End = 0;
    UINT32 Remainder, RepeatCnt;

    AmbaMisra_TouchUnused(pSrc);
    AmbaMisra_TouchUnused(pHorizantalMap);

    for (i = 0; i < SrcWidth; i++) {
        Remainder = (i % HorizantalMapLen);
        RepeatCnt = pHorizantalMap[Remainder];
        for (j = 0; j < RepeatCnt; j++) {
            pDst[Dst_i] = pSrc[i] + ColorOffset;
            Dst_i++;
            if (Dst_i >= DstWidth) {
                End = 1;
                break;
            }
        }

        if (End == 1U) {
            break;
        }
    }
#else
    UINT32 i, j;
    UINT32 Dst_i = 0;
    UINT32 Remainder = 0U, RepeatCnt;
    UINT32 Space;

    AmbaMisra_TouchUnused(pSrc);
    AmbaMisra_TouchUnused(pHorizantalMap);

    for (i = 0; i < SrcWidth; i++) {
        RepeatCnt = pHorizantalMap[Remainder];
        Remainder++;
        if (Remainder >= HorizantalMapLen) {
            Remainder = 0U;
        }

        if (RepeatCnt > 0U) {
            Space = DstWidth - Dst_i;
            if (RepeatCnt < Space) {
                for (j = 0; j < RepeatCnt; j++) {
                    pDst[Dst_i] = pSrc[i] + ColorOffset;
                    Dst_i++;
                }
            } else {
                for (j = 0; j < Space; j++) {
                    pDst[Dst_i] = pSrc[i] + ColorOffset;
                    Dst_i++;
                }
                break;
            }
        }
    }
#endif
}

static inline void RescaleBitMapLine_U8TOU32(UINT8 *pSrc, UINT32 SrcWidth,
                                             UINT32 *pDst, UINT32 DstWidth,
                                             UINT8 *pHorizantalMap,
                                             UINT32 HorizantalMapLen,
                                             UINT32 *pClut,
                                             UINT32 NumColor)
{
#if 0
    UINT32 i, j;
    UINT32 Dst_i = 0, End = 0;
    UINT32 Remainder, RepeatCnt;
    UINT32 ColorIdx;

    (void) pSrc;
    (void) pHorizantalMap;
    (void) pClut;

    for (i = 0; i < SrcWidth; i++) {
        Remainder = (i % HorizantalMapLen);
        RepeatCnt = pHorizantalMap[Remainder];
        for (j = 0; j < RepeatCnt; j++) {
            ColorIdx = pSrc[i];
            if (ColorIdx >= NumColor) {
                ColorIdx = 0U; /* index0 as default color */
            }
            pDst[Dst_i] = pClut[ColorIdx];
            Dst_i++;
            if (Dst_i >= DstWidth) {
                End = 1;
                break;
            }
        }

        if (End == 1U) {
            break;
        }
    }
#else
    UINT32 i, j;
    UINT32 Dst_i = 0;
    UINT32 Remainder = 0U, RepeatCnt;
    UINT32 NewColorIdx;
    UINT32 ColorIdx = 0xFFFFFFFFU, Color = 0U;
    UINT32 Space;

    AmbaMisra_TouchUnused(pSrc);
    AmbaMisra_TouchUnused(pHorizantalMap);
    AmbaMisra_TouchUnused(pClut);
    AmbaMisra_TouchUnused(&NumColor);

    for (i = 0; i < SrcWidth; i++) {
        RepeatCnt = pHorizantalMap[Remainder];
        Remainder++;
        if (Remainder >= HorizantalMapLen) {
            Remainder = 0U;
        }

        if (RepeatCnt > 0U) {
            NewColorIdx = pSrc[i];
            if (NewColorIdx == ColorIdx) {
                /* Use cache color */
            } else {
                //if (NewColorIdx >= NumColor) {
                //    NewColorIdx = 0U; // index0 as default color
                //}

                /* Update color */
                ColorIdx = NewColorIdx;
                Color    = pClut[NewColorIdx];
            }

            Space = DstWidth - Dst_i;
            if (RepeatCnt < Space) {
                for (j = 0; j < RepeatCnt; j++) {
                    if ((pDst[Dst_i] & 0xFF000000U) == 0U) {
                        /* If Dst is transparent or has not been drawn. */
                        pDst[Dst_i] = Color;
                    }
                    Dst_i++;
                }
            } else {
                for (j = 0; j < Space; j++) {
                    if ((pDst[Dst_i] & 0xFF000000U) == 0U) {
                        /* If Dst is transparent or has not been drawn. */
                        pDst[Dst_i] = Color;
                    }
                    Dst_i++;
                }
                break;
            }
        }
    }
#endif
}

/**
 *  Rescale 12 bits compact data to 8 bits data
 *  @param[in] pSrcBuf pointer to source buffer
 *  @param[in] SrcWidth width of data in bytes
 *  @param[in] SrcHeight height of data in bytes
 *  @param[in] SrcPitch pitch of data in bytes
 *  @param[in] pDstBuf pointer to dst buffer
 *  @param[in] DstPitch pitch of data in bytes
 *  @return 0-OK, 1-NG
 */
UINT32 SvcCvImgUtil_12bCompactTo8b(UINT8 *pSrcBuf, UINT32 SrcWidth, UINT32 SrcHeight, UINT32 SrcPitch, UINT8 *pDstBuf, UINT32 DstPitch)
{
    UINT32 i, j, PixelIdx, Temp;
    typedef struct {
        UINT32 Data0:       12;
        UINT32 Data1L:      4;
        UINT32 Data1H:      8;
        UINT32 Data2L:      8;
        UINT32 Data2H:      4;
        UINT32 Data3:       12;
        UINT32 Reserved:    16;
    } SVC_PYRAMID_12B_COMP_DATA_s;
    const UINT8 *pSrc;
    UINT8 *pDst;
    const SVC_PYRAMID_12B_COMP_DATA_s *pData;
    UINT32 Stride = 6; /* Bytes. Data0 ~ Data3 */

    AmbaMisra_TouchUnused(pSrcBuf);

    AmbaPrint_PrintUInt5("SvcCvImgUtil_12bCompactTo8b: Src %d x %d, Pitch %d, Dst Pitch %d, Stride %d",
        SrcWidth, SrcHeight, SrcPitch, DstPitch, Stride);

    if ((NULL != pSrcBuf) && (NULL != pDstBuf)) {
        for (i = 0; i < SrcHeight; i++) {
            pDst = &pDstBuf[i * DstPitch];
            PixelIdx = 0;
            for (j = 0; j < SrcWidth; j+= Stride) {
                pSrc = &pSrcBuf[(i * SrcPitch) + j];
                AmbaMisra_TypeCast(&pData, &pSrc);

                Temp = (UINT32)pData->Data0;
                pDst[PixelIdx] = (UINT8)((UINT32)Temp >> 4U);
                PixelIdx++;

                if (PixelIdx < DstPitch) {
                    Temp = ((UINT32)pData->Data1H << 4U) | pData->Data1L;
                    pDst[PixelIdx] = (UINT8)((UINT32)Temp >> 4U);
                    PixelIdx++;
                }

                if (PixelIdx < DstPitch) {
                    Temp = ((UINT32)pData->Data2H << 8U) | pData->Data2L;
                    pDst[PixelIdx] = (UINT8)((UINT32)Temp >> 4U);
                    PixelIdx++;
                }

                if (PixelIdx < DstPitch) {
                    Temp = (UINT32)pData->Data3;
                    pDst[PixelIdx] = (UINT8)((UINT32)Temp >> 4U);
                    PixelIdx++;
                }
            }
        }
    }

    return SVC_OK;
}
