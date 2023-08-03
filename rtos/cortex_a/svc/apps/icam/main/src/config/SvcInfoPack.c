/**
 *  @file SvcInfoPack.c
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
 *  @details svc information pack
 *
 */

#include "AmbaDef.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"
#include "AmbaFPD.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaCache.h"
#include "AmbaHDMI.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_CVBS.h"
#include "AmbaCVBS.h"

#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcBuffer.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcResCfg.h"
#include "SvcResCfgTask.h"
#include "SvcUserPref.h"
#include "SvcBufCalc.h"
#include "SvcIK.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#include "SvcInfoPack.h"
#include "SvcWinCalc.h"
#include "SvcOsd.h"
#include "SvcDSP.h"
#include "SvcEnc.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillProc.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "SvcPlat.h"

#if defined(CONFIG_ICAM_IMGCAL_USED)
#include "SvcCalibMgr.h"
#include "SvcCalibStitch.h"
#endif

#ifdef CONFIG_ICAM_PROJECT_SURROUND
#include "SvcAnimTask.h"
#endif

#if defined(CONFIG_ICAM_CV_STEREO)
#include "AmbaWrap.h"
#include "SvcCalibMgr.h"
#include "SvcCalibStereo.h"
#define SVC_INFO_PACK_CALIB_ENABLE (1U)
#endif

#define SVC_LOG_INFO        "INFO"

#if defined(CONFIG_ICAM_PYRAMID_BUF_SUPPORTED) || defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
#define LIV_MEM_ALLOC_CONTINUE      (0x00U)
#define LIV_MEM_ALLOC_RESET         (0x01U)
#endif
#define SVC_INFO_PACK_DEF_BOOT_TOUT (5000U) // MS
#define SVC_INFO_PACK_DEF_LIV_TOUT  (2500U) // MS

static inline UINT32 ALIGN64(UINT32 X) {return ((X + 63U) & 0xFFFFFFC0U);}

typedef struct {
    UINT32 BufSize;
    UINT8  CeNeeded;
    UINT16 Compressed;
} SVC_DEF_RAW_s;

static SVC_DEF_RAW_s SvcDefRaw[AMBA_DSP_MAX_VIN_NUM] GNU_SECTION_NOZEROINIT;

#if defined(CONFIG_ICAM_PYRAMID_BUF_SUPPORTED)
static UINT32 PyramidExScaleAlloc(SVC_MEM_POOL_s *pPyramidMem,
                                  const SVC_FOV_CFG_s *pFovCfg,
                                  AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pFovCfgArr,
                                  AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidExScaleBuf,
                                  SVC_LIV_BUFTBL_s *pPyramidExScaleBufTbl)
{
    UINT32                       Rval = SVC_OK;
    UINT16                       BufWidth, BufHeight;
    UINT32                       i, BufSize, ReqSize, RemainSize;
    ULONG                        Val;
    AMBA_DSP_WINDOW_DIMENSION_s  Win;
    ULONG                        DataAddr;

    /* Pyramid ex scale setting */
    if ((pFovCfg->PyramidBits & ((UINT32)1U << pFovCfg->PyramidExScale.SourceScale)) > 0U) {
        Win.Width  = pFovCfg->PyramidExScale.DstWin.Width;
        Win.Height = pFovCfg->PyramidExScale.DstWin.Height;
        SvcBufCalc_YUV420Buffer(&Win, &BufSize, &BufWidth, &BufHeight);

        pFovCfgArr->LaneDetect.HierSource = pFovCfg->PyramidExScale.SourceScale;
        pFovCfgArr->LaneDetect.Width      = pFovCfg->PyramidExScale.DstWin.Width;
        pFovCfgArr->LaneDetect.Height     = pFovCfg->PyramidExScale.DstWin.Height;
        Rval = AmbaWrap_memcpy(&pFovCfgArr->LaneDetect.CropWindow, &pFovCfg->PyramidExScale.SrcWin, sizeof(AMBA_DSP_WINDOW_s));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "PyramidExScaleAlloc err, AmbaWrap_memcpy PyramidExScale.CropWindow failed with 0x%x", Rval, 0U);
        }

        /* Pyramid buffer */
        if (NULL != pPyramidExScaleBuf) {
            pFovCfgArr->pLaneDetectBuf = pPyramidExScaleBuf;
            pPyramidExScaleBuf->AllocType  = ALLOC_EXTERNAL_DISTINCT;
            pPyramidExScaleBuf->BufNum     = CONFIG_ICAM_DSP_LIV_PYRAMID_BUF_NUM;

            if (pPyramidExScaleBuf->AllocType == ALLOC_EXTERNAL_DISTINCT) {
                ReqSize = GetAlignedValU32(BufSize, (UINT32)AMBA_CACHE_LINE_SIZE) * pPyramidExScaleBuf->BufNum;
                RemainSize = pPyramidMem->Size - (UINT32)(pPyramidMem->CurrAddr - pPyramidMem->BaseAddr);

                if (RemainSize >= ReqSize) {
                    if (NULL != pPyramidExScaleBufTbl) {
                        AmbaMisra_TypeCast(&(pPyramidExScaleBuf->pYuvBufTbl), &(pPyramidExScaleBufTbl));

                        for (i = 0U; i < pPyramidExScaleBuf->BufNum; i++) {
                            pPyramidMem->CurrAddr = GetAlignedValU32((UINT32)pPyramidMem->CurrAddr, (UINT32)AMBA_CACHE_LINE_SIZE);
                            pPyramidExScaleBufTbl->BufAddr[i] = pPyramidMem->CurrAddr;
                            pPyramidMem->CurrAddr += BufSize;
                        }

                        AmbaMisra_TypeCast(&DataAddr, &pPyramidExScaleBufTbl);
                        Rval = SvcPlat_CacheClean(DataAddr, sizeof(SVC_LIV_BUFTBL_s));
                        if (Rval != SVC_OK) {
                            SvcWrap_PrintUL("PyramidExScaleAlloc err, SvcPlat_CacheClean DataAddr 0x%x failed with 0x%x", DataAddr, (ULONG)Rval, 0U, 0U, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_INFO, "pPyramidExScaleBufTbl is NULL", 0U, 0U);
                        Rval = SVC_NG;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_INFO, "Buffer not enough. req %u, remain %u", ReqSize, RemainSize);
                    Rval = SVC_NG;
                }
            } else {
                /* Do nothing */
            }

            if (SVC_OK == Rval) {
                pPyramidExScaleBuf->YuvBuf.DataFmt        = AMBA_DSP_YUV420;
                pPyramidExScaleBuf->YuvBuf.Window.Width   = BufWidth;
                pPyramidExScaleBuf->YuvBuf.Window.Height  = BufHeight;
                pPyramidExScaleBuf->YuvBuf.Window.OffsetX = 0U;
                pPyramidExScaleBuf->YuvBuf.Window.OffsetY = 0U;
                Val = GetAlignedValU32((UINT32)BufWidth, (UINT32)AMBA_CACHE_LINE_SIZE);
                pPyramidExScaleBuf->YuvBuf.Pitch = (UINT16)Val;
            } else {
                pFovCfgArr->pLaneDetectBuf = NULL;
                Rval = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_INFO, "pPyramidExScaleBuf is NULL", 0U, 0U);
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_INFO, "PyramidExScaleAlloc: Pyramid scale(%u) is not enabled", pFovCfg->PyramidExScale.SourceScale, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 PyramidAlloc(UINT32 AllocBits,
                           const SVC_FOV_CFG_s *pFovCfg,
                           AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pFovCfgArr,
                           AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                           SVC_LIV_BUFTBL_s *pPyramidBufTbl,
                           AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidExScaleBuf,
                           SVC_LIV_BUFTBL_s *pPyramidExScaleBufTbl)
{
    static SVC_MEM_POOL_s        PyramidMem;

    UINT32                       Rval = SVC_OK;
    UINT16                       BufWidth, BufHeight;
    UINT32                       i, PyramidBufSize, ReqSize, RemainSize;
    ULONG                        Val;
    AMBA_DSP_WINDOW_DIMENSION_s  MainWin, HierWin;
    ULONG                        DataAddr;

    if (AllocBits == LIV_MEM_ALLOC_RESET) {
        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_PYRAMID, &PyramidMem.BaseAddr, &PyramidMem.Size);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "PyramidAlloc err, SvcBuffer_Request FMEM_ID_PYRAMID failed with 0x%x", Rval, 0U);
        }
        PyramidMem.CurrAddr = PyramidMem.BaseAddr;
    }

    MainWin.Width  = pFovCfg->MainWin.Width;
    MainWin.Height = pFovCfg->MainWin.Height;
    SvcWinCalc_CalcPyramidHierWin((pFovCfg->PyramidFlag & SVC_RES_PYRAMID_DOWNSCALE_2X),&MainWin, &HierWin);
    SvcBufCalc_Pyramid420(&MainWin, &HierWin, (UINT16)pFovCfg->PyramidBits, &PyramidBufSize, &BufWidth, &BufHeight);

    /* Pyramid setting */
    pFovCfgArr->Pyramid.HierBit        = (UINT16) pFovCfg->PyramidBits;
    if ((pFovCfg->PyramidFlag & SVC_RES_PYRAMID_DOWNSCALE_2X) > 0U) {
        pFovCfgArr->Pyramid.IsPolySqrt2 = 0U;
    } else {
        pFovCfgArr->Pyramid.IsPolySqrt2 = 1U;
    }
    if ((pFovCfg->PyramidFlag & SVC_RES_PYRAMID_12BITS_Y) > 0U) {
        pFovCfgArr->Pyramid.IsPolySqrt2 |= ((UINT16)1U << 6U);
    }
    if (pFovCfg->PyramidDeciRate != 0U) {
        pFovCfgArr->Pyramid.IsPolySqrt2 |= ((UINT16)pFovCfg->PyramidDeciRate << 8U);
    }
    pFovCfgArr->Pyramid.HierPolyWidth  = HierWin.Width;
    pFovCfgArr->Pyramid.HierPolyHeight = HierWin.Height;
    if ((pFovCfg->PyramidFlag & SVC_RES_PYRAMID_ENABLE_CROP) > 0U) {
        Rval = AmbaWrap_memcpy(&pFovCfgArr->Pyramid.HierCropWindow[0], &pFovCfg->HierCropWin[0],
                               AMBA_DSP_MAX_HIER_NUM * sizeof(AMBA_DSP_WINDOW_s));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "PyramidAlloc err, AmbaWrap_memcpy pFovCfg->HierCropWin failed with 0x%x", Rval, 0U);
        }
    }

    /* Pyramid buffer */
    if (NULL != pPyramidBuf) {
        pFovCfgArr->pPyramidBuf = pPyramidBuf;
        pPyramidBuf->AllocType  = ALLOC_EXTERNAL_DISTINCT;
        pPyramidBuf->BufNum     = CONFIG_ICAM_DSP_LIV_PYRAMID_BUF_NUM;

        if (pPyramidBuf->AllocType == ALLOC_EXTERNAL_DISTINCT) {
            ReqSize = GetAlignedValU32(PyramidBufSize, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)pPyramidBuf->BufNum;
            RemainSize = PyramidMem.Size - (UINT32)(PyramidMem.CurrAddr - PyramidMem.BaseAddr);

            if (RemainSize >= ReqSize) {
                if (NULL != pPyramidBufTbl) {
                    AmbaMisra_TypeCast(&(pPyramidBuf->pYuvBufTbl), &(pPyramidBufTbl));

                    for (i = 0U; i < pPyramidBuf->BufNum; i++) {
                        PyramidMem.CurrAddr = AmbaSvcWrap_GetAlignedAddr(PyramidMem.CurrAddr, AMBA_CACHE_LINE_SIZE);
                        pPyramidBufTbl->BufAddr[i] = PyramidMem.CurrAddr;
                        PyramidMem.CurrAddr += PyramidBufSize;
                    }

                    AmbaMisra_TypeCast(&DataAddr, &pPyramidBufTbl);
                    Rval = SvcPlat_CacheClean(DataAddr, sizeof(SVC_LIV_BUFTBL_s));
                    if (Rval != SVC_OK) {
                        SvcWrap_PrintUL("PyramidAlloc err, SvcPlat_CacheClean DataAddr 0x%x failed with 0x%x", DataAddr, (ULONG)Rval, 0U, 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_INFO, "PyramidBufTbl is NULL", 0U, 0U);
                    Rval = SVC_NG;
                }
            } else {
                SvcLog_NG(SVC_LOG_INFO, "Buffer not enough. req %u, remain %u", ReqSize, RemainSize);
                Rval = SVC_NG;
            }
        } else {
            /* Do nothing */
        }

        if (SVC_OK == Rval) {
            pPyramidBuf->YuvBuf.DataFmt        = AMBA_DSP_YUV420;
            pPyramidBuf->YuvBuf.Window.Width   = BufWidth;
            pPyramidBuf->YuvBuf.Window.Height  = BufHeight;
            pPyramidBuf->YuvBuf.Window.OffsetX = 0U;
            pPyramidBuf->YuvBuf.Window.OffsetY = 0U;
            Val = GetAlignedValU32((UINT32)BufWidth, (UINT32)AMBA_CACHE_LINE_SIZE);
            pPyramidBuf->YuvBuf.Pitch = (UINT16)Val;
        } else {
            pFovCfgArr->pPyramidBuf = NULL;
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_INFO, "pPyramidBuf is NULL", 0U, 0U);
        Rval = SVC_NG;
    }

    if ((SVC_OK == Rval) && ((pFovCfg->PyramidFlag & SVC_RES_PYRAMID_ENABLE_EX_SCALE) > 0U)) {
        Rval = PyramidExScaleAlloc(&PyramidMem, pFovCfg, pFovCfgArr, pPyramidExScaleBuf, pPyramidExScaleBufTbl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_INFO, "PyramidExScaleAlloc error", 0U, 0U);
        }
    }

    return Rval;
}
#endif

#if defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
static UINT32 MainY12BufAlloc(UINT32 AllocBits,
                              const SVC_FOV_CFG_s *pFovCfg,
                              AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pFovCfgArr,
                              AMBA_DSP_LIVEVIEW_YUV_BUF_s *pMainY12Buf,
                              SVC_LIV_BUFTBL_s *pMainY12BufTbl)
{
    static SVC_MEM_POOL_s        MainY12Mem;

    UINT32                       Rval = SVC_OK;
    UINT16                       BufWidth, BufHeight;
    UINT32                       i, Val, BufSize, ReqSize, RemainSize;
    AMBA_DSP_WINDOW_DIMENSION_s  MainWin;
    ULONG                        DataAddr;

    if (AllocBits == LIV_MEM_ALLOC_RESET) {
        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_MAIN_Y12_BUF, &MainY12Mem.BaseAddr, &MainY12Mem.Size);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "MainY12BufAlloc err, SvcBuffer_Request FMEM_ID_MAIN_Y12_BUF failed with 0x%x", Rval, 0U);
        }
        MainY12Mem.CurrAddr = MainY12Mem.BaseAddr;
    }

    MainWin.Width  = pFovCfg->MainWin.Width;
    MainWin.Height = pFovCfg->MainWin.Height;
    SvcBufCalc_MainY12b(&MainWin, &BufSize, &BufWidth, &BufHeight);

    /* Main 12 bits Y buffer */
    if (NULL != pMainY12Buf) {
        pFovCfgArr->pMainY12Buf = pMainY12Buf;
        pMainY12Buf->AllocType  = ALLOC_EXTERNAL_DISTINCT;
        pMainY12Buf->BufNum     = CONFIG_ICAM_DSP_MAIN_Y12_BUF_NUM;

        if (pMainY12Buf->AllocType == ALLOC_EXTERNAL_DISTINCT) {
            ReqSize = GetAlignedValU32(BufSize, AMBA_CACHE_LINE_SIZE) * pMainY12Buf->BufNum;
            RemainSize = MainY12Mem.Size - (MainY12Mem.CurrAddr - MainY12Mem.BaseAddr);

            if (RemainSize >= ReqSize) {
                if (NULL != pMainY12BufTbl) {
                    AmbaMisra_TypeCast(&(pMainY12Buf->pYuvBufTbl), &(pMainY12BufTbl));

                    for (i = 0U; i < pMainY12Buf->BufNum; i++) {
                        MainY12Mem.CurrAddr = GetAlignedValU32(MainY12Mem.CurrAddr, AMBA_CACHE_LINE_SIZE);
                        pMainY12BufTbl->BufAddr[i] = MainY12Mem.CurrAddr;
                        MainY12Mem.CurrAddr += BufSize;
                        SvcLog_DBG(SVC_LOG_INFO, "pMainY12BufTbl[%d]: 0x%x", i, pMainY12BufTbl->BufAddr[i]);
                    }

                    AmbaMisra_TypeCast(&DataAddr, &pMainY12BufTbl);
                    Rval = SvcPlat_CacheClean(DataAddr, sizeof(SVC_LIV_BUFTBL_s));
                } else {
                    SvcLog_NG(SVC_LOG_INFO, "pMainY12BufTbl is NULL", 0U, 0U);
                    Rval = SVC_NG;
                }
            } else {
                SvcLog_NG(SVC_LOG_INFO, "Buffer not enough. req %u, remain %u", ReqSize, RemainSize);
                Rval = SVC_NG;
            }
        } else {
            /* Do nothing */
        }

        if (SVC_OK == Rval) {
            pMainY12Buf->YuvBuf.DataFmt        = AMBA_DSP_YUV420;
            pMainY12Buf->YuvBuf.Window.Width   = BufWidth;
            pMainY12Buf->YuvBuf.Window.Height  = BufHeight;
            pMainY12Buf->YuvBuf.Window.OffsetX = 0U;
            pMainY12Buf->YuvBuf.Window.OffsetY = 0U;
            Val = GetAlignedValU32((UINT32)BufWidth, AMBA_CACHE_LINE_SIZE);
            pMainY12Buf->YuvBuf.Pitch = (UINT16)Val;
        } else {
            pFovCfgArr->pMainY12Buf = NULL;
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_INFO, "pMainY12Buf is NULL", 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}
#endif

#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
static UINT32 LvFeedExtDataBufAlloc(UINT32 AllocBits,
                              const SVC_FOV_CFG_s *pFovCfg,
                              AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pFovCfgArr,
                              AMBA_DSP_LIVEVIEW_YUV_BUF_s *pFeedExtDataBuf,
                              SVC_LIV_BUFTBL_s *pFeedExtDataBufTbl)
{
    UINT32 BufSize, TotalUsedSize = 0, StartAddr;
    UINT16 BufWidth = 0, BufHeight = 0;
    UINT32 ReqBufSize = 0U;
    ULONG  ReqBufBase = 0U;
    UINT32 ExitJLoop;

    if (AllocBits == LIV_MEM_ALLOC_RESET) {
        if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_LV_FEED_EXT_DATA, &ReqBufBase, &ReqBufSize)) {
            SvcLog_NG(SVC_LOG_INFO, "## fail to request mipi-raw pipe memory", 0U, 0U);
        } else {
            SvcLog_DBG(SVC_LOG_INFO, "## request for mipi-raw pipe memory: 0x%x / 0x%x", ReqBufBase, ReqBufSize);
        }
    }

    if (SVC_OK != AmbaWrap_memset(pFeedExtDataBuf, 0, sizeof(AMBA_DSP_LIVEVIEW_YUV_BUF_s))) {
        SvcLog_NG(SVC_LOG_INFO, "LvFeedExtDataBufAlloc() err, AmbaWrap_memset AMBA_DSP_LIVEVIEW_YUV_BUF_s failed", 0U, 0U);
    }

    pFovCfgArr->pIntMainBuf = pFeedExtDataBuf;
    pFeedExtDataBuf->AllocType = ALLOC_EXTERNAL_DISTINCT;
    pFeedExtDataBuf->BufNum = CONFIG_ICAM_LV_FEED_EXT_DATA_BUF_NUM;

    BufWidth = pFovCfg->MainWin.Width << 1U; // mono 16bit
    BufHeight = pFovCfg->MainWin.Height;
    BufSize = (UINT32)BufWidth * BufHeight;

    pFeedExtDataBuf->YuvBuf.DataFmt = AMBA_DSP_YUV400;
    pFeedExtDataBuf->YuvBuf.Pitch = (UINT16)ALIGN64((UINT32)BufWidth);
    pFeedExtDataBuf->YuvBuf.Window.Width = BufWidth;
    pFeedExtDataBuf->YuvBuf.Window.Height = BufHeight;
    pFeedExtDataBuf->YuvBuf.Window.OffsetX = 0U;
    pFeedExtDataBuf->YuvBuf.Window.OffsetY = 0U;

    AmbaMisra_TypeCast(&(pFeedExtDataBuf->pYuvBufTbl), &(pFeedExtDataBufTbl));

    StartAddr = ALIGN64(ReqBufBase);
    TotalUsedSize = StartAddr - ReqBufBase;
    ExitJLoop = 0U;
    for (UINT32 i = 0U; (i < 4U) && (ExitJLoop == 0U); i++) {
        if ((TotalUsedSize + ALIGN64(BufSize)) > ReqBufSize) {
            ExitJLoop = 1U;
            SvcLog_NG(SVC_LOG_INFO, "[%d] ExtIntMain exceed reserved size[%d]", i, ReqBufSize);
        } else {
            pFeedExtDataBufTbl->BufAddr[i] = StartAddr + ALIGN64(i * BufSize);
            TotalUsedSize += ALIGN64(BufSize);
            SvcLog_DBG(SVC_LOG_INFO, "ADDR 0x%x, TotalUsedSize %d", pFeedExtDataBufTbl->BufAddr[i], TotalUsedSize);
            SvcLog_DBG(SVC_LOG_INFO, "pYuvBufTbl[%d] = 0x%x", i, pFeedExtDataBuf->pYuvBufTbl[i]);
        }
    }

    SvcLog_DBG(SVC_LOG_INFO, "## AllocType/BufNum = %d/%d", pFovCfgArr->pIntMainBuf->AllocType, pFovCfgArr->pIntMainBuf->BufNum);

    return SVC_OK;
}
#endif

static UINT8 GetMaxSearchRange(UINT16 Width)
{
    UINT8 MaxSearchRange;

    AmbaMisra_TouchUnused(&Width);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    MaxSearchRange = 48U;
#else
    if (3840U <= Width) {
        MaxSearchRange = 8U;
    } else if ((2880U <= Width)) {
        MaxSearchRange = 16U;
    } else if ((1920U < Width)) {
        MaxSearchRange = 24U;
    } else {
        MaxSearchRange = 48U;
    }
#endif

    SvcLog_OK(SVC_LOG_INFO, "MaxSearchRange = %d", MaxSearchRange, 0U);
    return MaxSearchRange;
}

static void FillEncResource(AMBA_DSP_ENCODE_RESOURCE_s *pEncRes, const SVC_RES_CFG_MAX_s *pMaxCfg)
{
    UINT32 i;

    pEncRes->MaxEncodeStream = (UINT16)pMaxCfg->RecNum;
    for (i = 0; i < pMaxCfg->RecNum; i++) {
        pEncRes->MaxGopM[i]    = (UINT8) pMaxCfg->RecGopM[i];
        pEncRes->MaxStrmFmt[i] = (UINT8) pMaxCfg->RecFormat[i];
        pEncRes->MaxSearchRange[i] = GetMaxSearchRange(pMaxCfg->RecWin[i].Width);
    #if defined(CONFIG_ICAM_DSP_HEVC_SCHRANGE_SUPPORTED)
        pEncRes->MaxSearchRangeHevc[i] = 24U;
    #endif

    #if CONFIG_ICAM_DSP_ENC_ENG_NUM > 1
        {
            const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
            UINT32                 Bit = 1U;
            pEncRes->MaxAffinity[i] = Bit << pCfg->RecStrm[i].RecSetting.VEngineId;
        }
    #endif
    }
}

/**
 * VIN source initailization
 * @param [in] pSrcInit pointer to VIN source configuration
 */
void SvcInfoPack_VinSrcInit(SVC_VIN_SRC_INIT_s *pSrcInit)
{
    UINT32 RetVal;
    UINT32 i, j, IsUsed;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM] = {0}, VinNum = 0;
    UINT32 VinSrc = 0U;
    UINT32 SensorID = 0U, SensorIdx = 0U, SensorIdxs[16] = {0}, SensorNum = 0U, SensorMode = 0U;
    UINT32 IsVirtualChan = 0U, FromVinID = 0U;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    AmbaSvcWrap_MisraMemset(pSrcInit, 0, sizeof(SVC_VIN_SRC_INIT_s));

    RetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);

    if (SVC_OK == RetVal) {
        UINT32 CfgCnt = 0U;
        for (i = 0U; i < VinNum; i++) {
            UINT32 VinID = VinIDs[i];
            IsUsed = 0U;

            if (SVC_OK == RetVal) {
                RetVal = SvcResCfg_GetVinSrc(VinID, &VinSrc);
                SvcLog_OK(SVC_LOG_INFO, "VinID (%d) VinSrc (%d)", VinID, VinSrc);
            }
            if ((SVC_OK == RetVal) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_YUV))) {
                RetVal = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
                SvcLog_OK(SVC_LOG_INFO, "VinID (%d) SensorID (0x%8x)", VinID, SensorID);
            }
            if ((SVC_OK == RetVal) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_YUV))) {
                RetVal = SvcResCfg_GetSensorIdxsInVinID(VinID, SensorIdxs, &SensorNum);
                if ((SVC_OK == RetVal) && (SensorNum > 0U)) {
                    SensorIdx = SensorIdxs[0];
                    SensorMode = pCfg->SensorCfg[VinID][SensorIdx].SensorMode;
                } else {
                    RetVal = SVC_NG;
                }
                SvcLog_OK(SVC_LOG_INFO, "VinID (%d) SensorNum (%d)", VinID, SensorNum);
                for (j = 0; j < SensorNum; j++) {
                    SvcLog_OK(SVC_LOG_INFO, "VinID (%d) SensorIdxs[0] (%d)", VinID, SensorIdxs[0]);
                }
            }

            if (SVC_OK == RetVal) {
                RetVal = SvcResCfg_GetVinVirtualChan(VinID, &IsVirtualChan, &FromVinID);
            }

            if (SVC_OK == RetVal) {
                if (VinSrc == SVC_VIN_SRC_SENSOR){
                    if (IsVirtualChan != 0U) {
                        SvcLog_OK(SVC_LOG_INFO, "VinID (%d) is Virtual-Channel VIN, from (%d)", VinID, FromVinID);
                        for (j = 0U; j < pSrcInit->CfgNum; j++) {
                            if (pSrcInit->InitCfgArr[j].Config.VinID == FromVinID) {
                                pSrcInit->InitCfgArr[j].Config.SrcBits |= SensorID;

                                if (IsVirtualChan == SVC_RES_VIN_VIRTUAL_1) {
                                    pSrcInit->InitCfgArr[j].Config.SrcMode_1 = SensorMode;
                                } else if (IsVirtualChan == SVC_RES_VIN_VIRTUAL_2) {
                                    pSrcInit->InitCfgArr[j].Config.SrcMode_2 = SensorMode;
                                } else {
                                    pSrcInit->InitCfgArr[j].Config.SrcMode_3 = SensorMode;
                                }
                                break;
                            }
                        }
                        if (j == pSrcInit->CfgNum) {
                            SvcLog_OK(SVC_LOG_INFO, "InitCfgArr (%d) is active (%d)", j, VinID);
                            /* One virtual channel case */
                            pSrcInit->InitCfgArr[CfgCnt].IsActive = 1U;
                            pSrcInit->InitCfgArr[CfgCnt].pSrcObj  = pCfg->VinCfg[VinID].pDriver;
                            pSrcInit->InitCfgArr[CfgCnt].pEEPROBObj = pCfg->VinCfg[VinID].pEEPROMDriver;
                            pSrcInit->InitCfgArr[CfgCnt].Config.VinID = FromVinID;
                            pSrcInit->InitCfgArr[CfgCnt].Config.SrcType = VinSrc;
                            pSrcInit->InitCfgArr[CfgCnt].Config.SrcBits = SensorID;
                            pSrcInit->InitCfgArr[CfgCnt].Config.SrcMode = SensorMode;
                            pSrcInit->InitCfgArr[CfgCnt].Config.SensorBits = 0;

                            IsUsed = 1U;
                        }
                    } else {
                        pSrcInit->InitCfgArr[CfgCnt].IsActive = 1U;
                        pSrcInit->InitCfgArr[CfgCnt].pSrcObj  = pCfg->VinCfg[VinID].pDriver;
                        pSrcInit->InitCfgArr[CfgCnt].pEEPROBObj = pCfg->VinCfg[VinID].pEEPROMDriver;
                        pSrcInit->InitCfgArr[CfgCnt].Config.VinID = VinID;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcType = VinSrc;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcBits = SensorID;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcMode = SensorMode;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SensorBits = 0;
                        pSrcInit->InitCfgArr[CfgCnt].Config.DisableMasterSync = pCfg->VinCfg[VinID].DisableMasterSync;
#if defined(CONFIG_ICAM_CV_STEREO)
                        {
                            SVC_CALIB_TBL_INFO_s TblInfo = {0};
                            const SVC_CALIB_STEREO_INFO_s    *pOutput = NULL;
                            UINT32 TblID = 0U;
                            UINT32 k = 0U;
                            UINT32 CalibBitEnable = 0U;
                            SVC_APP_STAT_CALIB_s CalibStatus;
                            UINT32 BufferSize;
                            ULONG BufferBase;

                            TblInfo.BufSize = sizeof(SVC_CALIB_STEREO_INFO_s);
                            TblInfo.DbgMsgOn = 1U;
                            AmbaSvcWrap_MisraMemset(&CalibStatus, 0, sizeof(CalibStatus));

                            for (k = 0U; k < pCfg->FovNum; k++){
                                if ((pCfg->FovCfg[k].CalUpdBits & (0x1UL << SVC_CALIB_STEREO_ID)) > 0U){
                                    CalibBitEnable |= SVC_INFO_PACK_CALIB_ENABLE;
                                }
                            }

                            if (CalibBitEnable == SVC_INFO_PACK_CALIB_ENABLE){
                                do {
                                    (void)SvcSysStat_Get(SVC_APP_STAT_CALIB, &CalibStatus);
                                } while (CalibStatus.Status != SVC_APP_STAT_CALIB_LOAD_DONE);

                                RetVal = SvcCalib_ItemDataInit(SVC_CALIB_STEREO_ID);
                                if (SVC_OK == RetVal) {
                                    RetVal = SvcCalib_StereoGetTblIDInVinIDSensorID(VinID, SensorID, &TblID);
                                    if (SVC_OK == RetVal) {
                                        RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_STEREO_INFOPACK, &BufferBase, &BufferSize);
                                        if (SVC_OK != RetVal) {
                                            SvcLog_NG(SVC_LOG_INFO, "SvcBuffer_Request(SMEM_PF0_ID_STEREO_INFOPACK) failed", 0U, 0U);

                                        }
                                        AmbaMisra_TypeCast(&(TblInfo.pBuf), &(BufferBase));

                                        RetVal = SvcCalib_ItemTableGet(SVC_CALIB_STEREO_ID, TblID, &TblInfo);
                                        if (SVC_OK == RetVal) {
                                            AmbaMisra_TypeCast(&(pOutput), &(TblInfo.pBuf));
                                            pSrcInit->InitCfgArr[CfgCnt].Config.VsyncDelay = pOutput->CamWarpInfo.Method.V2100.Header.VsyncDelay;
                                            SvcLog_OK(SVC_LOG_INFO, "VinID (%d)", VinID, 0U);
                                            SvcLog_OK(SVC_LOG_INFO, "TblID (%d) VsyncDelay (%d)", TblID, pOutput->CamWarpInfo.Method.V2100.Header.VsyncDelay);
                                        } else {
                                            pSrcInit->InitCfgArr[CfgCnt].Config.VsyncDelay = 0U;
                                            SvcLog_NG(SVC_LOG_INFO, "SvcCalib_ItemTableGet Fail", 0U, 0U);
                                        }
                                    } else {
                                        SvcLog_NG(SVC_LOG_INFO, "SvcCalib_StereoGetTblIDInVinIDSensorID Fail", 0U, 0U);
                                    }
                                } else {
                                    SvcLog_NG(SVC_LOG_INFO, "SvcCalib_ItemDataInit Fail", 0U, 0U);
                                }
                            } else {
                                pSrcInit->InitCfgArr[CfgCnt].Config.VsyncDelay = 0x0U;
                                SvcLog_OK(SVC_LOG_INFO, "CalibBitEnable is not enable", 0U, 0U);
                            }
                        }
#else
                        pSrcInit->InitCfgArr[CfgCnt].Config.VsyncDelay = 0x0U;
#endif
                        IsUsed = 1U;
                    }
                }

                if (IsUsed == 0U) {
                    if (VinSrc == SVC_VIN_SRC_YUV) {
                        pSrcInit->InitCfgArr[CfgCnt].IsActive = 1U;
                        pSrcInit->InitCfgArr[CfgCnt].pSrcObj  = pCfg->VinCfg[VinID].pDriver;
                        pSrcInit->InitCfgArr[CfgCnt].pEEPROBObj = pCfg->VinCfg[VinID].pEEPROMDriver;
                        pSrcInit->InitCfgArr[CfgCnt].Config.VinID = VinID;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcType = VinSrc;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcBits = SensorID;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcMode = SensorMode;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SensorBits = 0;
                        pSrcInit->InitCfgArr[CfgCnt].Config.DisableMasterSync = pCfg->VinCfg[VinID].DisableMasterSync;
                        IsUsed = 1U;
                    }
                }

                if (IsUsed == 0U) {
                    if (VinSrc == SVC_VIN_SRC_MEM) {
                        pSrcInit->InitCfgArr[CfgCnt].IsActive = 1U;
                        pSrcInit->InitCfgArr[CfgCnt].pSrcObj  = pCfg->VinCfg[VinID].pDriver;
                        pSrcInit->InitCfgArr[CfgCnt].pEEPROBObj = NULL;
                        pSrcInit->InitCfgArr[CfgCnt].Config.VinID = VinID;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcType = VinSrc;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcBits = SensorID;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SrcMode = SensorMode;
                        pSrcInit->InitCfgArr[CfgCnt].Config.SensorBits = 0;
                        IsUsed = 1U;
                    }
                }

                if (IsUsed == 0U) {
                    if (IsVirtualChan == 0U) {
                        SvcLog_NG(SVC_LOG_INFO, "## vin(%u), no driver is hooked", VinID, 0U);
                    } else {
                        SvcLog_OK(SVC_LOG_INFO, "## vin(%u) is virtual channel, from VinID (%d)", VinID, FromVinID);
                    }
                } else {
                    pSrcInit->CfgNum++;
                    CfgCnt++;
                }
            }
        }
    }
}

/**
 * VOUT source initailization
 * @param [in] pSrcInit pointer to VOUT source configuration
 */
void SvcInfoPack_VoutSrcInit(SVC_VOUT_SRC_INIT_s *pSrcInit)
{
    UINT32  i, Rval, Num = 0U;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    Rval = AmbaWrap_memset(pSrcInit, 0, sizeof(SVC_VOUT_SRC_INIT_s));
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_VoutSrcInit err, AmbaWrap_memset pSrcInit failed with 0x%x", Rval, 0U);
    }

    /* Configure based on SvcResCfg */
    for (i = 0; i < pCfg->DispNum; i++) {
        if ((pCfg->DispBits & ((UINT32) 1U << i)) > 0U) {
            AMBA_FPD_OBJECT_s *pFpdObj = NULL;

            AmbaMisra_TypeCast(&pFpdObj, &pCfg->DispStrm[i].pDriver);

            pSrcInit->Dev[Num].VoutID = (UINT8) pCfg->DispStrm[i].VoutID;
            pSrcInit->Dev[Num].DevMode = pCfg->DispStrm[i].DevMode;
            pSrcInit->Dev[Num].pFpdObj = pFpdObj;

            Num++;
        }
    }

    pSrcInit->NumInit = Num;
}

/**
 * IK FOV window getting function
 * @param [in] FovId FOV index
 * @param [out] pIKFovWin pointer to IK FOV window
 */
void SvcInfoPack_IKFovWin(UINT32 FovId, SVC_IK_FOV_WIN_s *pIKFovWin)
{
    UINT32 RetVal;
    UINT32 VinID = 0U, FovSrc = 0xFFU;
    UINT32 SensorID = 0U, SensorIdx = 0U;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
    const SVC_FOV_CFG_s  *pFovCfg;
    const SVC_VIN_CFG_s  *pVinCfg;

    RetVal = SvcResCfg_GetFovSrc(FovId, &FovSrc);

    if ((SVC_OK == RetVal) && ((FovSrc == SVC_VIN_SRC_SENSOR) || (FovSrc == SVC_VIN_SRC_YUV) || (FovSrc == SVC_VIN_SRC_MEM))) {
        RetVal = SvcResCfg_GetSensorIdxOfFovIdx(FovId, &VinID, &SensorIdx);
        RetVal |= SvcResCfg_GetSensorIDOfSensorIdx(VinID, SensorIdx, &SensorID);
    }

    if (SVC_OK == RetVal) {
        if (FovId < pCfg->FovNum) {
            pFovCfg = &(pCfg->FovCfg[FovId]);
            pVinCfg = &(pCfg->VinCfg[VinID]);

            pIKFovWin->Chan.VinID    = VinID;
            pIKFovWin->Chan.SensorID = SensorID;

            pIKFovWin->Cap.OffsetX   = pVinCfg->CapWin.OffsetX;
            pIKFovWin->Cap.OffsetY   = pVinCfg->CapWin.OffsetY;
            pIKFovWin->Cap.Width     = pVinCfg->CapWin.Width;
            pIKFovWin->Cap.Height    = pVinCfg->CapWin.Height;

            pIKFovWin->Raw.OffsetX   = pFovCfg->RawWin.OffsetX;
            pIKFovWin->Raw.OffsetY   = pFovCfg->RawWin.OffsetY;
            pIKFovWin->Raw.Width     = pFovCfg->RawWin.Width;
            pIKFovWin->Raw.Height    = pFovCfg->RawWin.Height;

            pIKFovWin->Main.Width    = pFovCfg->MainWin.Width;
            pIKFovWin->Main.Height   = pFovCfg->MainWin.Height;

            pIKFovWin->Option = pVinCfg->SubChanCfg[0].Option;
            pIKFovWin->Option &= 0xFFFFU;
            if (pVinCfg->SubChanCfg[0].Option == AMBA_DSP_VIN_CAP_OPT_PROG) {
                pIKFovWin->ConCatNum = pVinCfg->SubChanCfg[0].ConCatNum;
                pIKFovWin->ConCatNum &= 0xFFFFU;
            } else if (pVinCfg->SubChanCfg[0].Option == AMBA_DSP_VIN_CAP_OPT_INTC) {
                pIKFovWin->ConCatNum = pVinCfg->SubChanCfg[0].IntcNum;
                pIKFovWin->ConCatNum &= 0xFFFFU;
            } else {
                /* Do nothing */
            }
        }
    }
}

/**
 * DSP resource getting function
 * @param [out] pDspRes pointer to DSP resource
 */
void SvcInfoPack_DspResource(AMBA_DSP_RESOURCE_s *pDspRes)
{
    UINT32                       VinID, i, Width, Height, PbkFormatId, Err;
    const SVC_RES_CFG_MAX_s      *pMaxCfg = SvcResCfg_GetMax();
    const SVC_RES_CFG_s          *pCfg = SvcResCfg_Get();
    AMBA_DSP_WINDOW_DIMENSION_s  HierWin;
    SVC_USER_PREF_s              *pPref;

    Err = SvcUserPref_Get(&pPref);
    if (SVC_OK != Err) {
        SvcLog_NG(SVC_LOG_INFO, "SvcUserPref_Get failed %u", Err, 0U);
    }

    Err = SvcResCfgTask_GetPbkModeId(&PbkFormatId);
    if (SVC_OK != Err) {
        SvcLog_DBG(SVC_LOG_INFO, "no pbk format id", Err, 0U);
        PbkFormatId = 0xFFFFFFFFU;
    }
    AmbaMisra_TouchUnused(&PbkFormatId);

#if defined(DSP_DRAM_PIPE_NUM)
    pDspRes->LiveviewResource.VideoPipe      = 0U;
#else
    pDspRes->LiveviewResource.VideoPipe      = pCfg->FovCfg[0].PipeCfg.PipeMode;
#endif

    pDspRes->LiveviewResource.LowDelayMode   = 0U;
    pDspRes->LiveviewResource.MaxViewZoneNum = (UINT16) pMaxCfg->FovNum;

    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID++) {
        pDspRes->LiveviewResource.MaxVinVirtChanWidth[VinID][0U] = pMaxCfg->VinWin[VinID].Width;
        pDspRes->LiveviewResource.MaxVinVirtChanHeight[VinID][0U] = pMaxCfg->VinWin[VinID].Height;
        pDspRes->LiveviewResource.RawBuf[VinID][0].AllocType = ALLOC_INTERNAL;
        pDspRes->LiveviewResource.RawBuf[VinID][0].BufNum    = CONFIG_ICAM_DSP_VIN_RAW_BUF_NUM;

        pDspRes->LiveviewResource.MaxVinVirtChanBit[VinID] = 1U;
        pDspRes->LiveviewResource.MaxVinVirtChanOutputNum[VinID][0U] = (UINT16)pMaxCfg->VinOutputNum[VinID];

        if (pMaxCfg->VinTimeOut[VinID].Boot == 0U) {
            pDspRes->LiveviewResource.MaxVinBootTimeout[VinID] = SVC_INFO_PACK_DEF_BOOT_TOUT;
        } else {
            pDspRes->LiveviewResource.MaxVinBootTimeout[VinID] = pMaxCfg->VinTimeOut[VinID].Boot;
        }

        if (pMaxCfg->VinTimeOut[VinID].Liveview == 0U) {
            pDspRes->LiveviewResource.MaxVinTimeout[VinID]  = SVC_INFO_PACK_DEF_LIV_TOUT;
        } else {
            pDspRes->LiveviewResource.MaxVinTimeout[VinID] = pMaxCfg->VinTimeOut[VinID].Liveview;
        }
        if (pCfg->StillCfg.EnableStillRaw == 1U) {
            if (pMaxCfg->StillRawUncomp == 0U) {
                UINT32 Rval;
                UINT16 RawWidth, Pitch;
                Rval = AmbaDSP_GetCmprRawBufInfo(pCfg->StillCfg.RawCfg[VinID].MaxRaw.Width, pCfg->StillCfg.RawCfg[VinID].MaxRaw.Compressed, &RawWidth, &Pitch);
                //SvcLog_DBG(SVC_LOG_INFO, "VinID %u, SvcDefRaw[VinID].Compressed %u", VinID, SvcDefRaw[VinID].Compressed);
                //SvcLog_DBG(SVC_LOG_INFO, "MaxRaw.Width %u, Pitch %u", pCfg->StillCfg.RawCfg[VinID].MaxRaw.Width, Pitch);
                if (Rval == OK) {
                    pDspRes->StillResource.MaxVinWidth[VinID] = Pitch;
                    pDspRes->StillResource.MaxVinHeight[VinID] = pCfg->StillCfg.RawCfg[VinID].MaxRaw.Height;
                } else {
                    //SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_DspResource line %u, AmbaDSP_GetCmprRawBufInfo return 0x%x", __LINE__, Rval);
                }
            } else {
                pDspRes->StillResource.MaxVinWidth[VinID] = pCfg->StillCfg.RawCfg[VinID].MaxRaw.Width;
                pDspRes->StillResource.MaxVinHeight[VinID] = pCfg->StillCfg.RawCfg[VinID].MaxRaw.Height;
            }
        }
    }

    pDspRes->LiveviewResource.MaxVinBit = (UINT16)pMaxCfg->VinBits;

    {
        UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;
        AmbaSvcWrap_MisraMemset(FovIdxs, 0, sizeof(FovIdxs));
        if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxs, &FovNum)) {
            for (UINT32 Idx = 0U; Idx < FovNum; Idx++) {
                UINT32 FovID = FovIdxs[Idx], VirVinId;
                if (pCfg->FovCfg[FovID].PipeCfg.FromVirtVin != 0U) {
                    /* if user specify FovIdx as input, i.e., bit[31] is 0 */
                    if ((pCfg->FovCfg[FovID].PipeCfg.VirtVinSrc & 0x80000000U) == 0U) {
                        VirVinId = (pCfg->FovCfg[FovID].PipeCfg.FromVirtVin & ((UINT32)SVC_RES_FOV_FROM_VIRT_VIN - (UINT32)1U));
                        pDspRes->LiveviewResource.MaxVirtVinBit                |= (UINT16) (1UL << VirVinId);
                        pDspRes->LiveviewResource.MaxVirtVinOutputNum[VirVinId] = (UINT8) 1U;
                        pDspRes->LiveviewResource.MaxVirtVinWidth[VirVinId]     = pMaxCfg->FovWin[FovID].Width;
                        pDspRes->LiveviewResource.MaxVirtVinHeight[VirVinId]    = pMaxCfg->FovWin[FovID].Height;
                    }
                }
            }
        }
    }

    /* Enable image pyramid */
    for (i = 0U; i < pMaxCfg->FovNum; i++) {
        if (pMaxCfg->FovPyramidEnable[i] == 0U) {
            pDspRes->LiveviewResource.MaxHierWidth[i] = 0U;
            pDspRes->LiveviewResource.MaxHierHeight[i] = 0U;
            pDspRes->LiveviewResource.MaxLndtWidth[i] = 0U;
            pDspRes->LiveviewResource.MaxLndtHeight[i] = 0U;
        } else {
            SvcWinCalc_CalcPyramidHierWin((pCfg->FovCfg[i].PyramidFlag & SVC_RES_PYRAMID_DOWNSCALE_2X),&pMaxCfg->FovWin[i], &HierWin);
            pDspRes->LiveviewResource.MaxHierWidth[i] = HierWin.Width;
            pDspRes->LiveviewResource.MaxHierHeight[i] = HierWin.Height;

            if (pMaxCfg->FovPyramidExScaleEnable[i] > 0U) {
                pDspRes->LiveviewResource.MaxLndtWidth[i] = HierWin.Width;
                pDspRes->LiveviewResource.MaxLndtHeight[i] = HierWin.Height;
            }
        }
    }

    /* Luma/Chroma max waitline */
    for (i = 0U; i < pCfg->FovNum; i++) {
        if (pCfg->FovCfg[i].PipeCfg.PipeMode == SVC_VDOPIPE_DRAMEFCY) {
            if (pCfg->FovCfg[i].PipeCfg.WarpLumaWaitLine > 0U) {
                pDspRes->LiveviewResource.MaxWarpWaitLineLuma[i] = (UINT16)pCfg->FovCfg[i].PipeCfg.WarpLumaWaitLine;
            }
            if (pCfg->FovCfg[i].PipeCfg.WarpChromaWaitLine > 0U) {
                pDspRes->LiveviewResource.MaxWarpWaitLineChroma[i] = (UINT16)pCfg->FovCfg[i].PipeCfg.WarpChromaWaitLine;
            }
        }
    }

    /* max horizontal warp compensation */
    for (i = 0U; i < pCfg->FovNum; i++) {
        pDspRes->LiveviewResource.MaxWarpDma[i]               = (UINT16) pCfg->FovCfg[i].PipeCfg.MaxWarpDma;
        pDspRes->LiveviewResourceEx.MaxHorWarpCompensation[i] = (UINT16) pCfg->FovCfg[i].PipeCfg.MaxHorWarpCompensation;
    }

    /* fill encoder resource */
    FillEncResource(&(pDspRes->EncodeResource), pMaxCfg);

#ifdef CONFIG_ICAM_STLCAP_USED
    if (((pCfg->StillCfg.EnableStill == 1U) && (pCfg->VinNum > 0U)) ||
        (PbkFormatId == pPref->FormatId)) {
        UINT16 StillW = pMaxCfg->StillWin.Width;
        UINT16 StillH = pMaxCfg->StillWin.Height;
        UINT8  ProcWithDec = 0U;
        if (pCfg->StillCfg.EnableStillRaw == 1U) {
            if ((UINT16)pMaxCfg->StillRawW > StillW) {
                StillW = (UINT16)pMaxCfg->StillRawW;
            }
            if ((UINT16)pMaxCfg->StillRawH > StillH) {
                StillH = (UINT16)pMaxCfg->StillRawH;
            }
        }

        {
            UINT32 RetVal, Src, FovNum, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

            RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
            if (SVC_OK == RetVal) {
                for (i = 0U; i < FovNum; i++) {
                    Src = 255U;
                    RetVal = SvcResCfg_GetFovSrc(FovIdxs[i], &Src);
                    if (SVC_OK == RetVal) {
                        if (Src == SVC_VIN_SRC_MEM_DEC) {
                            if ((UINT16)pMaxCfg->FovWin[i].Width > StillW) {
                                StillW = (UINT16)pMaxCfg->FovWin[i].Width;
                            }
                            if ((UINT16)pMaxCfg->FovWin[i].Height > StillH) {
                                StillH = (UINT16)pMaxCfg->FovWin[i].Height;
                            }
                            ProcWithDec |= 1U;
                        }
                    }
                }
            }
        }
#ifdef CONFIG_SOC_CV2FS
        if (ProcWithDec > 0U) {
            StillW = (UINT16)ALIGN64((UINT32)StillW + 16U);
            StillH = (UINT16)ALIGN64((UINT32)StillH + 16U);
        }
#endif
        AmbaMisra_TouchUnused(&ProcWithDec);

#ifndef CONFIG_SOC_CV2FS
        if (pCfg->StillCfg.EnableStill == 1U) {
            pDspRes->EncodeResource.MaxEncodeStream ++;
            pDspRes->EncodeResource.MaxStrmFmt[pMaxCfg->RecNum] = 1U << 2U; // JPEG
        }
#endif
        pDspRes->StillResource.MaxProcessFormat = 1U | (pCfg->StillCfg.EnableHISO << 1U) | ((UINT8)pMaxCfg->StillCeEn << 2U) | (pCfg->StillCfg.EnableHDR << 3U) | ((UINT8)pMaxCfg->StillRawUncomp << 4U);
                                                       //B[0]LISO, B[1]HISO, B[2]CE, B[3]HDR, B[4]UncompressRaw
        SvcLog_DBG(SVC_LOG_INFO, "StillResource.MaxProcessFormat 0x%x", pDspRes->StillResource.MaxProcessFormat, 0U);
#ifndef CONFIG_SOC_CV2FS
        if (pCfg->StillCfg.EnableStill == 1U) {
            pDspRes->StillResource.MaxYuvEncWidth = StillW;
            pDspRes->StillResource.MaxYuvEncHeight = StillH;
        }
#endif
        pDspRes->StillResource.MaxYuvInputWidth = StillW;
        pDspRes->StillResource.MaxYuvInputHeight = StillH;
        pDspRes->StillResource.MaxMainWidth = StillW;
        pDspRes->StillResource.MaxMainHeight = StillH;
        SvcStillProc_SetMaxYuvInputWidth(StillW);
        if (pCfg->StillCfg.EnableStillRaw == 1U) {
            if (StillW > pDspRes->StillResource.MaxRawInputWidth) {
                pDspRes->StillResource.MaxRawInputWidth = StillW;
            }
            if (StillH > pDspRes->StillResource.MaxRawInputHeight) {
                pDspRes->StillResource.MaxRawInputHeight = StillH;
            }
        }
    }
#endif

    for (i = 0U; i < pCfg->DispNum; i++) {
        UINT32 VoutID = pCfg->DispStrm[i].VoutID;

        pDspRes->DisplayResource.MaxVoutWidth[VoutID] = pMaxCfg->DispMaxWin[VoutID].Width;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        pDspRes->DisplayResource.MaxVirtChanNum[VoutID] = 1U;
#endif

        if (SVC_OK != SvcOsd_GetOsdBufSize(VoutID, &Width, &Height)) {
            SvcLog_NG(SVC_LOG_INFO, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
        }
        pDspRes->DisplayResource.MaxOsdBufWidth[VoutID] = (UINT16)(Width << (UINT32)SVC_OSD_PIXEL_SIZE_SFT);
    }
#ifdef CONFIG_SOC_CV2FS
    {
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));

        if (0U == SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
            UINT32 VinSrc, SensorIdx[SVC_MAX_NUM_SENSOR_PER_VIN], SensorNum, SensorID;
            AMBA_SENSOR_CHANNEL_s        SsChan;
            AMBA_SENSOR_STATUS_INFO_s    SsStatus;

            for (i = 0U; i < VinNum; i ++) {
                VinID = VinIDs[i];
                VinSrc = 255U;
                (void) SvcResCfg_GetVinSrc(VinID, &VinSrc);
                SensorNum = 0U;
                (void) SvcResCfg_GetSensorIdxsInVinID(VinID, SensorIdx, &SensorNum);
                (void) SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);

                if (VinSrc == SVC_VIN_SRC_SENSOR) {

                    AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(SsChan));
                    SsChan.VinID = VinID;
                    SsChan.SensorID = SensorID;
                    AmbaSvcWrap_MisraMemset(&SsStatus, 0, sizeof(SsStatus));
                    if (0U == AmbaSensor_GetStatus(&SsChan, &SsStatus)) {

                        SvcLog_DBG(SVC_LOG_INFO, "VinID %u SsStatus.ModeInfo.EmbDataInfo.IsSupport %u", VinID, SsStatus.ModeInfo.EmbDataInfo.IsSupport);

                        if (SsStatus.ModeInfo.EmbDataInfo.IsSupport > 0U) {
                            pDspRes->LiveviewResourceEx.MaxVinEmbdDataWidth[VinID] = (UINT16)(SsStatus.ModeInfo.EmbDataInfo.Width);
                            pDspRes->LiveviewResourceEx.MaxVinEmbdDataHeight[VinID] = (UINT16)(SsStatus.ModeInfo.EmbDataInfo.Height) + 1U;
                            SvcLog_DBG(SVC_LOG_INFO, "Embedded Data %04dx%04d", pDspRes->LiveviewResourceEx.MaxVinEmbdDataWidth[VinID], pDspRes->LiveviewResourceEx.MaxVinEmbdDataHeight[VinID]);
                        }
                    }
                }
            }
        }
    }
#endif
}

/**
 * VIN capture window getting function
 * @param [out] pNumVin pointer to number of VIN
 * @param [out] pVinPostBits pointer to VIN mask for post configuration
 * @param [out] pVinCapArr pointer to VIN capture window
 */
void SvcInfoPack_LivVinCap(UINT32 *pNumVin, UINT32 *pVinPostBits, SVC_LIV_VINCAP_s *pVinCapArr)
{
    UINT32 RetVal;
    UINT32 VinID = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM] = {0} ,VinNum = 0U, VinSrc = 0U;
    UINT32 SensorID = 0U;
    UINT32                 i, j, Rval;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
    const SVC_VIN_CFG_s    *pVinCfg;

    RetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);

    *pVinPostBits = 0;
    if (SVC_OK == RetVal) {
        *pNumVin = VinNum;
        for (i = 0; i < VinNum; i++) {
            VinID = VinIDs[i];

            if (SVC_OK == RetVal) {
                RetVal = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
            }

            if (SVC_OK == RetVal) {
                RetVal = SvcResCfg_GetVinSrc(VinID, &VinSrc);
            }

            if (SVC_OK == RetVal) {
                if (VinSrc != SVC_VIN_SRC_YUV) {
                    *pVinPostBits |= (UINT32)1 << VinID;
                }
                Rval = AmbaWrap_memset(&(pVinCapArr[i]), 0, sizeof(SVC_LIV_VINCAP_s));
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivVinCap err, AmbaWrap_memset SVC_LIV_VINCAP_s failed with 0x%x", Rval, 0U);
                }

                pVinCfg = &(pCfg->VinCfg[VinID]);
                pVinCapArr[i].VinID    = VinID;
                if (pVinCfg->SubChanNum != 0U) {
                    pVinCapArr[i].SubChNum = pVinCfg->SubChanNum;
                    Rval = AmbaWrap_memcpy(pVinCapArr[i].SubChCfg, pVinCfg->SubChanCfg, sizeof(AMBA_DSP_VIN_SUB_CHAN_CFG_s) * AMBA_DSP_MAX_VIRT_CHAN_NUM);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivVinCap err, AmbaWrap_memcpy pVinCfg->SubChanCfg failed with 0x%x", Rval, 0U);
                    }
                    Rval = AmbaWrap_memcpy(pVinCapArr[i].SubChTDFrmNum, pVinCfg->SubChanTDFrmNum, sizeof(UINT16)*AMBA_DSP_MAX_VIRT_CHAN_NUM*AMBA_DSP_MAX_VIN_TD_NUM);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivVinCap err, AmbaWrap_memcpy pVinCfg->SubChanTDFrmNum failed with 0x%x", Rval, 0U);
                    }
                    for (j = 0; j < pVinCapArr[i].SubChNum; j++) {
                        pVinCapArr[i].SubChCfg[j].pTDFrmNum = pVinCapArr[i].SubChTDFrmNum[j];
                    }
                } else {
                    pVinCapArr[i].SubChNum = 1U;
                    pVinCapArr[i].SubChCfg[0U].SubChan.IsVirtual = 0U;
                    pVinCapArr[i].SubChCfg[0U].SubChan.Index     = (UINT16) VinID;
                    pVinCapArr[i].SubChCfg[0U].Option            = AMBA_DSP_VIN_CAP_OPT_PROG;
                    pVinCapArr[i].SubChCfg[0U].ConCatNum         = 1U;
                    pVinCapArr[i].SubChCfg[0U].IntcNum           = 1U;
                    pVinCapArr[i].SubChCfg[0U].CaptureWindow     = pVinCfg->CapWin;
                }
#if defined(AMBA_DSP_VIN_CAP_OPT_EMBD)
                if (pVinCfg->EmbChanCfg.Option == AMBA_DSP_VIN_CAP_OPT_EMBD) {
                    Rval = AmbaWrap_memcpy(&(pVinCapArr[i].EmbChCfg), &(pVinCfg->EmbChanCfg), sizeof(AMBA_DSP_VIN_SUB_CHAN_CFG_s));
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivVinCap err, AmbaWrap_memcpy pVinCfg->EmbChanCfg failed with 0x%x", Rval, 0U);
                    }
                }
#endif
            }
        }
    }
}

/**
 * Liveview FOV getting function
 * @param [out] pNumFov pointer to number of FOV
 * @param [out] pFovCfgArr pointer to FOV configuration
 * @param [out] pPyramidBufArr pointer to pyramid buffer info array
 * @param [out] pPyramidBufTbl pointer to pyramid buffer address table
 */
void SvcInfoPack_LivFovCfg(UINT32 *pNumFov,
                           AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pFovCfgArr,
                           SVC_INFO_PACK_LIV_FOV_BUF_ARR_s *pFovBufArr)
{
    UINT32               i, RetVal;
    UINT32               VinID = 0U, Src = 0U, VinSource = DSP_LV_SOURCE_VIN;
    UINT32               FovIdx, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0}, FovNum = 0U;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
    const SVC_FOV_CFG_s  *pFovCfg;
    SVC_USER_PREF_s      *pPref;

    #if defined(CONFIG_ICAM_PYRAMID_BUF_SUPPORTED)
    UINT32  AllocBits    = LIV_MEM_ALLOC_RESET;
    #endif

    #if defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
    UINT32  Y12_AllocBits    = LIV_MEM_ALLOC_RESET;
    #endif

    AmbaMisra_TouchUnused(pFovBufArr);

    if (SVC_OK != SvcUserPref_Get(&pPref)) {
        SvcLog_NG(SVC_LOG_INFO, "Failure to get preference", 0U, 0U);
    }

    RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);

    if (SVC_OK == RetVal) {
        *pNumFov = FovNum;

        for (i = 0U; i < FovNum; i++) {
            FovIdx = FovIdxs[i];
            if (SVC_OK != AmbaWrap_memset(&(pFovCfgArr[i]), 0, sizeof(AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s))) {
                SvcLog_NG(SVC_LOG_INFO, "AmbaWrap_memset pFovCfgArr[%d] preference", i, 0U);
            }

            if (SVC_OK == RetVal) {
                RetVal = SvcResCfg_GetFovSrc(FovIdx, &Src);
                SvcLog_DBG(SVC_LOG_INFO, "GetFovSrc of Fov[%d] is %d", FovIdx, Src);
                if (SVC_OK == RetVal) {
                    if (Src == SVC_VIN_SRC_MEM) {
                        VinSource = DSP_LV_SOURCE_MEM;
                        RetVal = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID);
                    } else if (Src == SVC_VIN_SRC_MEM_YUV422) {
                        VinSource = DSP_LV_SOURCE_MEM_YUV422;
                    } else if (Src == SVC_VIN_SRC_MEM_YUV420) {
                        VinSource = DSP_LV_SOURCE_MEM_YUV420;
                    } else if (Src == SVC_VIN_SRC_MEM_DEC) {
                        VinSource = DSP_LV_SOURCE_MEM_DEC;
                        RetVal = SvcResCfg_GetDecIDOfFovIdx(FovIdx, &VinID);
                    } else {
                        VinSource = DSP_LV_SOURCE_VIN;
                        RetVal = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID);
                    }
                }
            }

            pFovCfg = &(pCfg->FovCfg[FovIdx]);

            if (SVC_OK == RetVal) {
                pFovCfgArr[i].ViewZoneId        = (UINT16)FovIdx;
                pFovCfgArr[i].VinId             = (UINT16)VinID;
                if (pFovCfg->PipeCfg.FromVirtVin != 0U) {
                    pFovCfgArr[i].VinId         = (UINT16)pFovCfg->PipeCfg.FromVirtVin;
                }
                pFovCfgArr[i].VinSource         = (UINT16)VinSource;
                pFovCfgArr[i].VinRoi.OffsetX    = pFovCfg->RawWin.OffsetX;
                pFovCfgArr[i].VinRoi.OffsetY    = pFovCfg->RawWin.OffsetY;
                pFovCfgArr[i].VinRoi.Width      = pFovCfg->RawWin.Width;
                pFovCfgArr[i].VinRoi.Height     = pFovCfg->RawWin.Height;
                pFovCfgArr[i].MainWidth         = pFovCfg->MainWin.Width;
                pFovCfgArr[i].MainHeight        = pFovCfg->MainWin.Height;
                pFovCfgArr[i].RotateFlip        = (UINT8)pFovCfg->PipeCfg.RotateFlip;
                pFovCfgArr[i].HdrExposureNum    = (UINT8)pFovCfg->PipeCfg.HdrExposureNum;
                pFovCfgArr[i].HdrBlendHieght    = (UINT16)pFovCfg->PipeCfg.HdrBlendHieght;

                /* MctfDisable: B[0] Mctf Off, B[7:6] Mctf compression needed */
                pFovCfgArr[i].MctfDisable = 0U;
                if (CheckBits(pFovCfg->PipeCfg.MctfDisable, MCTF_BITS_OFF) == 0U) {
                    if ((CheckBits(pFovCfg->PipeCfg.MctsDisable, MCTS_BITS_OFF) == 0U)) {
                        /* enable mctf compression default if mctf/mcts on and no off cmpr */
                        if (CheckBits(pFovCfg->PipeCfg.MctfDisable, MCTF_BITS_ON_CMPR) != 0U) {
                            if (0U < pPref->MctfCmprCtrl) {
                                pFovCfgArr[i].MctfDisable = (UINT8)SetBits(pFovCfgArr[i].MctfDisable, 0x40U);
                            }
                        }
                    }
                } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                    pFovCfgArr[i].MctfDisable = (UINT8)SetBits(pFovCfgArr[i].MctfDisable, 0x20U);
#else
                    pFovCfgArr[i].MctfDisable = (UINT8)SetBits(pFovCfgArr[i].MctfDisable, 0x01U);
#endif
                }

                if (0U < CheckBits(pFovCfgArr[i].MctfDisable, 0x40U)) {
                    SvcLog_DBG(SVC_LOG_INFO, "## mctf compression is on", 0U, 0U);
                } else {
                    SvcLog_DBG(SVC_LOG_INFO, "## mctf compression is off", 0U, 0U);
                }

                /* MctsDisable: B[0] Mcts Off, B[7] Mcts dram out disable */
                pFovCfgArr[i].MctsDisable = (UINT8)pFovCfg->PipeCfg.MctsDisable;
                if (CheckBits(pFovCfg->PipeCfg.MctsDisable, MCTS_BITS_FORCE_RES) == 0U) {
                    if (pPref->MctsDOutCtrl == 0U) {
                        /* force to disable mcts dram out */
                        pFovCfgArr[i].MctsDisable = (UINT8)SetBits(pFovCfgArr[i].MctsDisable, MCTS_BITS_OFF_DRAMOUT);
                    }
                }

                if (0U < CheckBits(pFovCfgArr[i].MctsDisable, 0x80U)) {
                    SvcLog_DBG(SVC_LOG_INFO, "## mcts dram out is off", 0U, 0U);
                }

                if ((pFovCfg->PipeCfg.LinearCE & LINEAR_CE_FORCE_DISABLE) > 0U) {
                    pFovCfgArr[i].LinearCE          = 0U;
                } else {
                    pFovCfgArr[i].LinearCE          = (UINT8)pFovCfg->PipeCfg.LinearCE;
                }
#if defined(DSP_DRAM_PIPE_NUM)
                pFovCfgArr[i].DramUtilityPipe   = (UINT8)pFovCfg->PipeCfg.PipeMode;
#endif

                pFovCfgArr[i].IsVirtChan        = (UINT8)pFovCfg->VirtChan.Enable;
                if (0U < CheckBits(pFovCfg->VirtChan.Enable, SVC_RES_PIPE_VIRTUAL_CHAN_VIN_DECI)) {
                    pFovCfgArr[i].VinDecimation = (UINT8)pFovCfg->VirtChan.Id;
                } else {
                    pFovCfgArr[i].VinVirtChId   = (UINT16)pFovCfg->VirtChan.Id;
                }

                if (0U < CheckBits(pFovCfg->VirtChan.Enable, SVC_RES_PIPE_VIRTUAL_CHAN_INPUT_MUX_SEL)) {
                    pFovCfgArr[i].InputMuxSelCfg = pFovCfg->VirtChan.InputMuxSelCfg;
                }
            }

            if (SVC_OK == RetVal) {
#if defined(CONFIG_ICAM_PYRAMID_BUF_SUPPORTED)
                if (0U != pFovCfg->PyramidBits) {
                    if (SVC_OK != PyramidAlloc(AllocBits, pFovCfg, &(pFovCfgArr[i]),
                                               &(pFovBufArr->pPyramidBufArr[i]), &pFovBufArr->pPyramidBufTbl[i],
                                               &(pFovBufArr->pPyramidExScaleBufArr[i]), &pFovBufArr->pPyramidExScaleBufTbl[i])) {
                        SvcLog_NG(SVC_LOG_INFO, "PyramidAlloc[%u] error", FovIdx, 0U);
                    }

                    AllocBits = LIV_MEM_ALLOC_CONTINUE;
                }
#endif

#if defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
                if (0U != pFovCfg->PipeCfg.MainY12Out) {
                    if (SVC_OK != MainY12BufAlloc(Y12_AllocBits, pFovCfg, &(pFovCfgArr[i]), &(pFovBufArr->pMainY12BufArr[i]), &pFovBufArr->pMainY12BufTbl[i])) {
                        SvcLog_NG(SVC_LOG_INFO, "MainY12Alloc[%u] error", FovIdx, 0U);
                    }

                    Y12_AllocBits = LIV_MEM_ALLOC_CONTINUE;
                }
#endif

#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
                if (SVC_VDOPIPE_MIPIRAW == pFovCfg->PipeCfg.PipeMode) {
                    if (SVC_OK != LvFeedExtDataBufAlloc(LIV_MEM_ALLOC_RESET, pFovCfg, &(pFovCfgArr[i]), &(pFovBufArr->pFeedExtDataBufArr[i]), &(pFovBufArr->pFeedExtDataBufTbl[i]))) {
                        SvcLog_NG(SVC_LOG_INFO, "LvFeedExtDataBufAlloc[%u] error", FovIdx, 0U);
                    }
                }
#endif
            }
        }
    }
}

#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
/**
 * Liveview latency configuration getting function
 * @param [out] pLowDlyArr pointer to latency configuration
 */
void SvcInfoPack_LivLowDlyCfg(AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLowDlyArr)
{
    UINT32                i, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0}, FovNum = 0U;
    const SVC_RES_CFG_s   *pCfg = SvcResCfg_Get();
    const SVC_PIPE_CFG_s  *pPipeCfg;

    if (SvcResCfg_GetFovIdxs(FovIdxs, &FovNum) == SVC_OK) {
        for (i = 0U; i < FovNum; i++) {
            pPipeCfg = &(pCfg->FovCfg[FovIdxs[i]].PipeCfg);

            if (SVC_OK != AmbaWrap_memset(&(pLowDlyArr[i]), 0, sizeof(AMBA_DSP_LIVEVIEW_SLICE_CFG_s))) {
                SvcLog_NG(SVC_LOG_INFO, "AmbaWrap_memset pLowDlyArr[%d] failed", i, 0U);
            }
            if (1U < pPipeCfg->LowDelayParams[SVC_LDY_PIDX_SLICE_NUM]) {
                pLowDlyArr[i].EncSyncOpt  = 1;
                pLowDlyArr[i].SliceNumRow = (UINT8)pPipeCfg->LowDelayParams[SVC_LDY_PIDX_SLICE_NUM];
                pLowDlyArr[i].WarpOverLap = (UINT16)pPipeCfg->LowDelayParams[SVC_LDY_PIDX_LINE_WARP_OVERLAP];
                pLowDlyArr[i].VinDragLine = (UINT16)pPipeCfg->LowDelayParams[SVC_LDY_PIDX_LINE_VIN_DRAG];
            }
            pLowDlyArr[i].WarpLumaWaitLine      = (UINT16)pPipeCfg->WarpLumaWaitLine;
            pLowDlyArr[i].WarpChromaWaitLine    = (UINT16)pPipeCfg->WarpChromaWaitLine;
        }
    }
}
#endif

/**
 * Liveview YUV stream configuration getting function
 * @param [out] pNumStrm pointer to number of streams
 * @param [out] pStrmCfgArr pointer to streams configuration array
 * @param [out] pStrmChanArr pointer to streams channel configuration array
 */
void SvcInfoPack_LivStrmCfg(UINT32 *pNumStrm,
                            AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pStrmCfgArr,
                            SVC_LIV_STRM_CHAN_t *pStrmChanArr)
{
    UINT32                 i, c, Rval, Bit = 0x01U, StrmNum = 0U, EncId = 0U, BldBufSize = 0U;
    ULONG                  BldBufBase = 0U;
    UINT32                 CfgBits, DpxMode = 0U, Src, DpxNum = 0U, DpxFovId[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
    const SVC_STRM_CFG_s   *pStrmCfg;
    const SVC_CHAN_CFG_s   *pChanCfg;
    SVC_RES_CFG_MAX_s      *pCfgMax = SvcResCfg_GetMax();

    AmbaMisra_TouchUnused(&DpxFovId);

    for (i = 0U; i < pCfg->FovNum; i++) {
        Src = 255U;
        Rval = SvcResCfg_GetFovSrc(i, &Src);
        if ((Rval == SVC_OK) && (Src == SVC_VIN_SRC_MEM_DEC)) {
            DpxMode  = 1U;
            DpxFovId[DpxNum] = i;
            DpxNum++;
        }
    }

    CfgBits = pCfg->DispBits;
    for (i = 0U; i < pCfg->DispNum; i++) {
        if ((CfgBits & (UINT32)(Bit << i)) == 0U) {
            continue;
        }
        pStrmCfg = &(pCfg->DispStrm[i].StrmCfg);

        for (c = 0; c < pStrmCfg->NumChan; c++) {
            pChanCfg = &(pStrmCfg->ChanCfg[c]);

            if (pChanCfg->BlendEnable != 0U) {
                if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_BLENDING, &BldBufBase, &BldBufSize)) {
                    SvcLog_NG(SVC_LOG_INFO, "Request buffer for blending table failed", 0U, 0U);
                }
                break;
            }
        }
        if ((BldBufBase > 0U) && (BldBufSize > 0U)) {
            break;
        }
    }

#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
    AmbaMisra_TouchUnused(&DpxMode);
    AmbaMisra_TouchUnused(&DpxNum);
#else
    for (i = 0U; i < pCfg->DispNum; i++) {
        if ((CfgBits & (UINT32)(Bit << i)) == 0U) {
            continue;
        }
        pStrmCfg = &(pCfg->DispStrm[i].StrmCfg);

        Rval = AmbaWrap_memset(&(pStrmCfgArr[StrmNum]), 0, sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivStrmCfg err, AmbaWrap_memset pStrmCfgArr failed with 0x%x", Rval, 0U);
        }
        Rval = AmbaWrap_memset(pStrmChanArr[StrmNum], 0, sizeof(SVC_LIV_STRM_CHAN_t));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivStrmCfg err, AmbaWrap_memset pStrmChanArr failed with 0x%x", Rval, 0U);
        }

        pStrmCfgArr[StrmNum].StreamId = (UINT16)StrmNum;
        pStrmCfgArr[StrmNum].Purpose  = SVC_LIV_PURPOSE_MEM;
        pStrmCfgArr[StrmNum].Purpose |= SVC_LIV_PURPOSE_VOUT;
        pStrmCfgArr[StrmNum].DestVout = (UINT16)(Bit << pCfg->DispStrm[i].VoutID);
        pStrmCfgArr[StrmNum].DestEnc  = 0U;

        pStrmCfgArr[StrmNum].Width                = pStrmCfg->Win.Width;
        pStrmCfgArr[StrmNum].Height               = pStrmCfg->Win.Height;
        pStrmCfgArr[StrmNum].MaxWidth             = pStrmCfg->MaxWin.Width;
        pStrmCfgArr[StrmNum].MaxHeight            = pStrmCfg->MaxWin.Height;
        pStrmCfgArr[StrmNum].StreamBuf.AllocType  = ALLOC_INTERNAL;
        pStrmCfgArr[StrmNum].StreamBuf.BufNum     = CONFIG_ICAM_DSP_VOUT_YUV_BUF_NUM;
#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
        pStrmCfgArr[StrmNum].StreamBuf.BufNum     = CONFIG_ICAM_VOUT_FRAME_CTRL_VOUT_YUV_BUF_NUM;
#endif
        pStrmCfgArr[StrmNum].StreamBuf.pYuvBufTbl = NULL;

        pStrmCfgArr[StrmNum].NumChan  = (UINT16)pStrmCfg->NumChan;
        pStrmCfgArr[StrmNum].pChanCfg = pStrmChanArr[StrmNum];

        for (c = 0; c < pStrmCfg->NumChan; c++) {
            pChanCfg = &(pStrmCfg->ChanCfg[c]);

            pStrmChanArr[StrmNum][c].ViewZoneId     = (UINT16)pChanCfg->FovId;
            pStrmChanArr[StrmNum][c].ROI.OffsetX    = pChanCfg->SrcWin.OffsetX;
            pStrmChanArr[StrmNum][c].ROI.OffsetY    = pChanCfg->SrcWin.OffsetY;
            pStrmChanArr[StrmNum][c].ROI.Width      = pChanCfg->SrcWin.Width;
            pStrmChanArr[StrmNum][c].ROI.Height     = pChanCfg->SrcWin.Height;
            pStrmChanArr[StrmNum][c].Window.OffsetX = pChanCfg->DstWin.OffsetX;
            pStrmChanArr[StrmNum][c].Window.OffsetY = pChanCfg->DstWin.OffsetY;
            pStrmChanArr[StrmNum][c].Window.Width   = pChanCfg->DstWin.Width;
            pStrmChanArr[StrmNum][c].Window.Height  = pChanCfg->DstWin.Height;
            pStrmChanArr[StrmNum][c].RotateFlip     = pChanCfg->RotateFlip;
            pStrmChanArr[StrmNum][c].BlendNum       = 0U;
            pStrmChanArr[StrmNum][c].LumaAlphaTable = 0x0U;

            if (pChanCfg->BlendEnable == SVC_RES_BLD_TBL_FROM_CALIB) {
#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
                UINT32                  RetVal;
                ULONG                   BufSize = 0U;
                UINT32                  BlendVinId, BlendSensorIdx;
                UINT8                  *pBuf;
                AMBA_DSP_BUF_s         *pBldBuf;
                SVC_CALIB_TBL_INFO_s    CalTbl;
                SVC_CALIB_ST_BLEND_TBL_DATA_s CalBlendTbl;
                const SVC_CALIB_ST_BLEND_TBL_DATA_s *pCalBlendTbl = NULL;

                AmbaMisra_TypeCast(&pBldBuf, &BldBufBase);
                BldBufBase += sizeof(AMBA_DSP_BUF_s);

                if (pBldBuf != NULL) {
                    RetVal = SvcCalib_ItemDataInit(SVC_CALIB_STITCH_BLEND_ID);
                    if (RetVal == SVC_OK) {
                        RetVal = SvcResCfg_GetSensorIdxOfFovIdx(pChanCfg->FovId, &BlendVinId, &BlendSensorIdx);
                        if (RetVal == SVC_OK) {
                            RetVal |= AmbaWrap_memset(&CalTbl, 0, sizeof(CalTbl));
                            RetVal |= AmbaWrap_memset(&CalBlendTbl, 0, sizeof(CalBlendTbl));
                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_INFO, "AmbaWrap_memset CalTbl/CalBlendTbl failed", 0U, 0U);
                            }

                            AmbaMisra_TypeCast(&pBuf, &BldBufBase);
                            CalBlendTbl.pTbl = pBuf;
                            CalBlendTbl.TblLength = SVC_CALIB_STITCH_BLEND_TBL_SZ;
                            CalTbl.CalChan.VinID = BlendVinId;
                            CalTbl.CalChan.SensorID = 1U << BlendSensorIdx;
                            CalTbl.pBuf = &CalBlendTbl;
                            CalTbl.BufSize = sizeof(SVC_CALIB_ST_BLEND_TBL_DATA_s);
                            CalTbl.DbgMsgOn = 0U;
                            RetVal = SvcCalib_ItemTableGet(SVC_CALIB_STITCH_BLEND_ID, 0xFFU, &CalTbl);
                            if (RetVal == SVC_OK) {
                                AmbaMisra_TypeCast(&(pCalBlendTbl), &(CalTbl.pBuf));

                                pBldBuf->BaseAddr         = BldBufBase;
                                pBldBuf->Pitch            = (UINT16) ALIGN64(pCalBlendTbl->Width);
                                pBldBuf->Window.OffsetX   = 0U;
                                pBldBuf->Window.OffsetY   = 0U;
                                pBldBuf->Window.Width     = (UINT16) ALIGN64(pCalBlendTbl->Width);
                                pBldBuf->Window.Height    = (UINT16) pCalBlendTbl->Height;

                                AmbaMisra_TypeCast(&(pStrmChanArr[StrmNum][c].LumaAlphaTable), &pBldBuf);
                                pStrmChanArr[StrmNum][c].BlendNum = 1U;

                                BufSize = (ULONG) ALIGN64(pCalBlendTbl->Width) * pCalBlendTbl->Height;

                                if (SVC_OK != SvcPlat_CacheClean(BldBufBase, BufSize)) {
                                    // SvcLog_NG(SVC_LOG_INFO, "SvcPlat_CacheClean BufBase failed", 0U, 0U);
                                }

                                BldBufBase += BufSize;
                            }
                        }
                    }
                }
#else
                /* Do nothing */
#endif
            } else if (pChanCfg->BlendEnable == SVC_RES_BLD_TBL_FROM_ROM) {
                ULONG                 BufSize = 0U;
                AMBA_DSP_BUF_s        *pBldBuf;
                void                  *pBuf;
                const char            *pBldFileName;
                UINT16                OffsetX, OffsetY, Width, Height;

                AmbaMisra_TypeCast(&pBldBuf, &BldBufBase);
                BldBufBase += sizeof(AMBA_DSP_BUF_s);

                AmbaMisra_TypeCast(&pBuf, &BldBufBase);
                pBldFileName = pChanCfg->BlendTable.RomFileName;
                OffsetX = pChanCfg->BlendTable.Win.OffsetX;
                OffsetY = pChanCfg->BlendTable.Win.OffsetY;
                Width   = pChanCfg->BlendTable.Win.Width;
                Height  = pChanCfg->BlendTable.Win.Height;

                if (SVC_OK == SvcOsd_LoadBmpReverse(pBuf, ALIGN64(Width), Width, Height, pBldFileName)) {
                    pBldBuf->BaseAddr       = BldBufBase;
                    pBldBuf->Pitch          = (UINT16) ALIGN64(Width);
                    pBldBuf->Window.OffsetX = OffsetX;
                    pBldBuf->Window.OffsetY = OffsetY;
                    pBldBuf->Window.Width   = Width;
                    pBldBuf->Window.Height  = Height;

                    AmbaMisra_TypeCast(&(pStrmChanArr[StrmNum][c].LumaAlphaTable), &pBldBuf);
                    pStrmChanArr[StrmNum][c].BlendNum = 1U;

                    BufSize = (ULONG)ALIGN64(Width) * (ULONG)Height;

                    if (SVC_OK != SvcPlat_CacheClean(BldBufBase, BufSize)) {
                        // SvcLog_NG(SVC_LOG_INFO, "SvcPlat_CacheClean BufBase failed", 0U, 0U);
                    }

                    BldBufBase += BufSize;
                } else {
                    SvcLog_NG(SVC_LOG_INFO, "Load Blend Table Bmp failed", 0U, 0U);
                }
            } else {
                /* Do nothing */
            }

#ifdef CONFIG_ICAM_PIP_BLEND_USED
            if ((DpxMode == 1U) && (pStrmCfg->NumChan > 1U)) {
                UINT32                  BufSize, *pBuf, j;
                ULONG                   BufBase;
                static AMBA_DSP_BUF_s   BldBuf = {0};
                const AMBA_DSP_BUF_s    *pBldBuf;

                for (j = 0U; j < DpxNum; j++) {
                    if (pChanCfg->FovId == DpxFovId[j]) {
                        Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_PIP_ALPHA, &BufBase, &BufSize);
                        if (Rval == SVC_OK) {
                            BldBuf.BaseAddr         = BufBase;
                            BldBuf.Pitch            = (UINT16)GetAlignedValU32((UINT32)pChanCfg->DstWin.Width, 64U);
                            BldBuf.Window.OffsetX   = 0U;
                            BldBuf.Window.OffsetY   = 0U;
                            BldBuf.Window.Width     = pChanCfg->DstWin.Width;
                            BldBuf.Window.Height    = pChanCfg->DstWin.Height;

                            AmbaMisra_TypeCast(&pBuf, &BufBase);
                            if (SVC_OK != AmbaWrap_memset(pBuf, 128, BufSize)) {
                                SvcLog_NG(SVC_LOG_INFO, "AmbaWrap_memset pBuf failed", 0U, 0U);
                            }
                            if (SVC_OK != SvcPlat_CacheClean(BufBase, BufSize)) {
                                // SvcLog_NG(SVC_LOG_INFO, "SvcPlat_CacheClean BufBase failed", 0U, 0U);
                            }

                            pBldBuf = &BldBuf;
                            AmbaMisra_TypeCast(&(pStrmChanArr[StrmNum][c].LumaAlphaTable), &pBldBuf);
                            pStrmChanArr[StrmNum][c].BlendNum = 1U;
                            break;
                        }
                    }
                }
            }
#else
            AmbaMisra_TouchUnused(&DpxMode);
            AmbaMisra_TouchUnused(&DpxFovId);
            AmbaMisra_TouchUnused(&DpxNum);
#endif

#ifdef CONFIG_ICAM_PROJECT_SURROUND
            {
                UINT32 BlendTbl, RvalBlend;
                RvalBlend = SvcAnimTask_GetBlendTbl(c, &BlendTbl);
                if (RvalBlend == SVC_OK) {
                    pStrmChanArr[StrmNum][c].BlendNum = 1U;
                    pStrmChanArr[StrmNum][c].LumaAlphaTable = BlendTbl;
                    AmbaPrint_PrintUInt5("[CAL] SvcAnimTask_GetBlendTbl(%u): BlendTbl = 0x%x", c, BlendTbl, 0U, 0U, 0U);
                }
            }
#endif
        }
        pStrmCfgArr[StrmNum].MaxChanBitMask = pCfgMax->DispChanBits[pCfg->DispStrm[i].VoutID];
        pStrmCfgArr[StrmNum].MaxChanNum = (UINT16)pCfgMax->DispChanNum[pCfg->DispStrm[i].VoutID];
        pStrmCfgArr[StrmNum].pMaxChanWin = pCfgMax->DispChan[pCfg->DispStrm[i].VoutID];
        StrmNum = StrmNum + 1U;
    }
#endif

    CfgBits = pCfg->RecBits;
    for (i = 0U; i < pCfg->RecNum; i++) {
        if ((CfgBits & (UINT32)(Bit << i)) == 0U) {
            continue;
        }
        pStrmCfg = &(pCfg->RecStrm[i].StrmCfg);

        Rval = AmbaWrap_memset(&(pStrmCfgArr[StrmNum]), 0, sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivStrmCfg err, AmbaWrap_memset pStrmCfgArr failed with 0x%x", Rval, 0U);
        }
        Rval = AmbaWrap_memset(pStrmChanArr[StrmNum], 0, sizeof(SVC_LIV_STRM_CHAN_t));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_LivStrmCfg err, AmbaWrap_memset pStrmChanArr failed with 0x%x", Rval, 0U);
        }

        pStrmCfgArr[StrmNum].StreamId = (UINT16)StrmNum;
        pStrmCfgArr[StrmNum].Purpose  = SVC_LIV_PURPOSE_MEM;
        pStrmCfgArr[StrmNum].Purpose |= SVC_LIV_PURPOSE_ENC;
        pStrmCfgArr[StrmNum].DestVout = 0U;
        pStrmCfgArr[StrmNum].DestEnc  = (Bit << EncId);

        pStrmCfgArr[StrmNum].Width                = pStrmCfg->Win.Width;
        pStrmCfgArr[StrmNum].Height               = pStrmCfg->Win.Height;
        pStrmCfgArr[StrmNum].MaxWidth             = pStrmCfg->MaxWin.Width;
        pStrmCfgArr[StrmNum].MaxHeight            = pStrmCfg->MaxWin.Height;
        pStrmCfgArr[StrmNum].StreamBuf.AllocType  = ALLOC_INTERNAL;
        pStrmCfgArr[StrmNum].StreamBuf.BufNum     = CONFIG_ICAM_DSP_ENC_YUV_BUF_NUM;
        pStrmCfgArr[StrmNum].StreamBuf.pYuvBufTbl = NULL;

        pStrmCfgArr[StrmNum].NumChan  = (UINT16)pStrmCfg->NumChan;
        pStrmCfgArr[StrmNum].pChanCfg = pStrmChanArr[StrmNum];
        for (c = 0; c < pStrmCfg->NumChan; c++) {
            pChanCfg = &(pStrmCfg->ChanCfg[c]);

            pStrmChanArr[StrmNum][c].ViewZoneId     = (UINT16)pChanCfg->FovId;
            pStrmChanArr[StrmNum][c].ROI.OffsetX    = pChanCfg->SrcWin.OffsetX;
            pStrmChanArr[StrmNum][c].ROI.OffsetY    = pChanCfg->SrcWin.OffsetY;
            pStrmChanArr[StrmNum][c].ROI.Width      = pChanCfg->SrcWin.Width;
            pStrmChanArr[StrmNum][c].ROI.Height     = pChanCfg->SrcWin.Height;
            pStrmChanArr[StrmNum][c].Window.OffsetX = pChanCfg->DstWin.OffsetX;
            pStrmChanArr[StrmNum][c].Window.OffsetY = pChanCfg->DstWin.OffsetY;
            pStrmChanArr[StrmNum][c].Window.Width   = pChanCfg->DstWin.Width;
            pStrmChanArr[StrmNum][c].Window.Height  = pChanCfg->DstWin.Height;
            pStrmChanArr[StrmNum][c].RotateFlip     = pChanCfg->RotateFlip;
            pStrmChanArr[StrmNum][c].LumaAlphaTable = 0x0U;
        }
        pStrmCfgArr[StrmNum].MaxChanBitMask = pCfgMax->RecChanBits[i];
        pStrmCfgArr[StrmNum].MaxChanNum = (UINT16)pCfgMax->RecChanNum[i];
        pStrmCfgArr[StrmNum].pMaxChanWin = pCfgMax->RecChan[i];

        StrmNum = StrmNum + 1U;
        EncId = EncId + 1U;
    }

    *pNumStrm = StrmNum;
}

/**
 * Display configuration getting function
 * @param [out] pInfo pointer to display configuration
 */
void SvcInfoPack_DispConfig(const SVC_DISP_INFO_s *pInfo)
{
    UINT32                        i, Rval, Bit = 0x01U, DispNum = 0U;
    AMBA_DSP_VOUT_MIXER_CONFIG_s  *pMixer;
    AMBA_DSP_VOUT_VIDEO_CFG_s     *pVideo;
    AMBA_DSP_DISPLAY_CONFIG_s     *pSyncCfg;
    const SVC_RES_CFG_s           *pCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s         *pDisp;

    for (i = 0; i < pCfg->DispNum; i++) {
        if ((pCfg->DispBits & (UINT32)(Bit << i)) == 0U) {
            continue;
        }
        pDisp = &(pCfg->DispStrm[i]);

        pInfo->pDispCfg[DispNum].VoutID = pDisp->VoutID;
        pMixer = &(pInfo->pDispCfg[DispNum].MixerCfg);
        Rval = AmbaWrap_memset(pMixer, 0, sizeof(AMBA_DSP_VOUT_MIXER_CONFIG_s));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_DispConfig err, AmbaWrap_memset pDispCfg failed with 0x%x", Rval, 0U);
        }
#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) // Currently only CV5 supports 8K, and in the case, it is YUV420 output
        if (pDisp->DevMode == (UINT8) HDMI_VIC_7680_4320_30P) {
            pMixer->MixerColorFormat         = 0x10;
        } else {
            pMixer->MixerColorFormat         = MIXER_IN_YUV_444_RGB;
        }
#else
        pMixer->MixerColorFormat         = MIXER_IN_YUV_444_RGB;
#endif
        pMixer->VideoHorReverseEnable    = 0U;
        pMixer->FrameRate.TimeScale      = pDisp->FrameRate.TimeScale;
        pMixer->FrameRate.NumUnitsInTick = pDisp->FrameRate.NumUnitsInTick;
        pMixer->FrameRate.Interlace      = pDisp->FrameRate.Interlace;
        pMixer->ActiveWidth              = pDisp->StrmCfg.MaxWin.Width;
        pMixer->ActiveHeight             = pDisp->StrmCfg.MaxWin.Height;

        pVideo = &(pInfo->pDispCfg[DispNum].VideoCfg);
        Rval = AmbaWrap_memset(pVideo, 0, sizeof(AMBA_DSP_VOUT_VIDEO_CFG_s));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_DispConfig err, AmbaWrap_memset pVideo failed with 0x%x", Rval, 0U);
        }

        pVideo->Window.Width   = pDisp->StrmCfg.Win.Width;
        pVideo->Window.Height  = pDisp->StrmCfg.Win.Height;
        pVideo->Window.OffsetX = pMixer->ActiveWidth - pVideo->Window.Width;
        pVideo->Window.OffsetX = (UINT16)((UINT32)pVideo->Window.OffsetX >> (UINT32)1U);
        pVideo->Window.OffsetY = pMixer->ActiveHeight - pVideo->Window.Height;
        pVideo->Window.OffsetY = (UINT16)((UINT32)pVideo->Window.OffsetY >> (UINT32)1U);

        pMixer->ActiveHeight   = (pMixer->ActiveHeight >> pDisp->FrameRate.Interlace);
        pVideo->Window.Height  = (pVideo->Window.Height >> pDisp->FrameRate.Interlace);
        pVideo->Window.OffsetY = (pVideo->Window.OffsetY >> pDisp->FrameRate.Interlace);
#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
        pVideo->VideoSource    = VOUT_SOURCE_BACKGROUND_COLOR;
#else
        pVideo->VideoSource    = VOUT_SOURCE_INTERNAL;
#endif
        pVideo->RotateFlip     = pDisp->VideoRotateFlip;

        /* check if CVBS */
#if defined(CONFIG_ICAM_CVBS_EXIST)
        if (pDisp->pDriver == &AmbaFPD_CVBS_Obj) {
            if (pDisp->DevMode == AMBA_CVBS_SYSTEM_NTSC) {
                pInfo->pDispCfg[DispNum].CvbsMode = SVC_DISP_CVBS_NTSC;
            } else if (pDisp->DevMode == AMBA_CVBS_SYSTEM_PAL) {
                pInfo->pDispCfg[DispNum].CvbsMode = SVC_DISP_CVBS_PAL;
            } else {
                pInfo->pDispCfg[DispNum].CvbsMode = SVC_DISP_CVBS_NONE;
            }
        }
#endif

        pSyncCfg = &(pInfo->pDispCfg[DispNum].SyncCfg);
        Rval = AmbaWrap_memset(pSyncCfg, 0, sizeof(AMBA_DSP_DISPLAY_CONFIG_s));
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_DispConfig err, AmbaWrap_memset pSyncCfg failed with 0x%x", Rval, 0U);
        }
#if defined(CONFIG_ICAM_VIN_VOUT_LOWDLY_SUPPORTED)
        if ((pDisp->VinVoutSyncCfg.DelayTime > 0U) ||
            (pDisp->DualVoutSyncDelay> 0U)) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            pSyncCfg->VinTargetIndex = (UINT16)(pDisp->VinVoutSyncCfg.SyncVinID);
#endif
            pSyncCfg->VinVoutSyncDelay = (UINT16)(pDisp->VinVoutSyncCfg.DelayTime);
            pSyncCfg->VoutSyncDelay    = (UINT16)(pDisp->DualVoutSyncDelay);
        }
#endif

        DispNum = DispNum + 1U;
    }
    *(pInfo->pNumDisp) = DispNum;
}

static UINT32 SvcInfoPack_DefRawVinMemQry(UINT32 *pRawMemSize, UINT32 *pVinIDs, UINT32 *pVinNum)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s* pResCfg = SvcResCfg_Get();

    if (pResCfg == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_INFO, "Failure to query default vin memory - invalid resolution configuration", 0U, 0U);
    } else {
        UINT32 VinIdx, VinSrc, MemSize;
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0;
        UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0;

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
        AmbaSvcWrap_MisraMemset(FovIdxs, 0, sizeof(FovIdxs));

        PRetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_INFO, "Failure to query default vin memory - get VinIDs failure", 0U, 0U);
        }

        if (RetVal == SVC_OK) {
            if (pVinNum != NULL) {
                *pVinNum = VinNum;
            }

            for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {
                if (pVinIDs != NULL) {
                    pVinIDs[VinIdx] = VinIDs[VinIdx];
                }
                if (pRawMemSize != NULL) {
                    pRawMemSize[VinIdx] = 0U;
                }
            }

            for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {
                VinSrc = 255U;
                PRetVal = SvcResCfg_GetVinSrc(VinIDs[VinIdx], &VinSrc);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_INFO, "Failure to query default vin memory - get VinID(%d) source failure", VinIDs[VinIdx], 0U);
                }

                if (RetVal == SVC_OK) {
                    if (VinSrc != SVC_VIN_SRC_YUV) {
                        SVC_DSP_FRAME_s Setup;

                        PRetVal = SvcResCfg_GetFovIdxsInVinID(VinIDs[VinIdx], FovIdxs, &FovNum);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            SvcLog_NG(SVC_LOG_INFO, "Failure to query default vin memory - get VinID(%d) fov id failure", VinIDs[VinIdx], 0U);
                        } else {

                            Setup.FrameType = SVC_FRAME_TYPE_RAW;
                            Setup.FrameW = (UINT32)pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Width;
                            Setup.FrameH = (UINT32)pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Height;
                            Setup.RawCmpr = (UINT16)SvcIK_RawCompressConfig(VinIDs[VinIdx], pResCfg->FovCfg[FovIdxs[0]].PipeCfg.RawCompression);
                            if (pResCfg->FovCfg[FovIdxs[0]].PipeCfg.HdrExposureNum > 1U) {
                                Setup.CeNeeded = 1;
                            } else {
                                if ((pResCfg->FovCfg[FovIdxs[0]].PipeCfg.LinearCE & LINEAR_CE_FORCE_DISABLE) > 0U) {
                                    Setup.CeNeeded = 0;
                                } else if ((pResCfg->FovCfg[FovIdxs[0]].PipeCfg.LinearCE & LINEAR_CE_ENABLE) > 0U) {
                                    Setup.CeNeeded = 1;
                                } else {
                                    Setup.CeNeeded = 0;
                                }
                            }
                            Setup.FrameNum = 1;
                            SvcDefRaw[VinIDs[VinIdx]].CeNeeded = Setup.CeNeeded;
                            SvcDefRaw[VinIDs[VinIdx]].Compressed = Setup.RawCmpr;
                            PRetVal = SvcDSP_QueryFrameBufSize(&Setup, &MemSize);
                            if (PRetVal == OK) {
                                SvcLog_DBG(SVC_LOG_INFO, "vin %u default raw buffer for liveview 0x%08x", VinIDs[VinIdx], MemSize);
                                if (pRawMemSize != NULL) {
                                    pRawMemSize[VinIdx] += MemSize;
                                    SvcDefRaw[VinIDs[VinIdx]].BufSize = MemSize;
                                }
                            }
                        }
                    } else {
                        // YUV 422
                        MemSize  = GetAlignedValU32((UINT32)(pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Width), 64U);
                        MemSize *= GetAlignedValU32((UINT32)(pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Height), 64U);
                        MemSize  = MemSize << 1;

                        SvcDefRaw[VinIDs[VinIdx]].CeNeeded = 0U;
                        SvcDefRaw[VinIDs[VinIdx]].Compressed = 0U;

                        SvcLog_DBG(SVC_LOG_INFO, "vin %u default raw buffer for yuv liveview 0x%08x", VinIdx, MemSize);
                        if (pRawMemSize != NULL) {
                            pRawMemSize[VinIdx] += MemSize;
                            SvcDefRaw[VinIDs[VinIdx]].BufSize = MemSize;
                         }
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 * Default raw buffer size getting function
 * @param [out] pMemSize pointer to default raw buffer size
 */
void SvcInfoPack_DefRawMemQry(UINT32 *pMemSize)
{
    UINT32 RetVal = SVC_OK;

    if (pMemSize == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_INFO, "Failure to query default raw memory - output memory size should not null!", 0U, 0U);
    } else {
        UINT32 VinNum = 0U;
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM];
        UINT32 RawMemSize[AMBA_DSP_MAX_VIN_NUM];

        AmbaSvcWrap_MisraMemset(VinIDs,     0, sizeof(VinIDs));
        AmbaSvcWrap_MisraMemset(RawMemSize, 0, sizeof(RawMemSize));

        RetVal = SvcInfoPack_DefRawVinMemQry(RawMemSize, VinIDs, &VinNum);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_INFO, "Failure to query default raw memory", 0U, 0U);
        } else {
            UINT32 VinIdx;

            for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {
                *pMemSize += RawMemSize[VinIdx];
            }
            SvcLog_DBG(SVC_LOG_INFO, "Total default raw buffer size 0x%08x", *pMemSize, 0U);
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

/**
 * Default raw buffer configuration function
 * @param [in] pLivRes pointer to liveview resource setting
 */
void SvcInfoPack_DefRawCfg(AMBA_DSP_LIVEVIEW_RESOURCE_s *pLivRes)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s* pResCfg = SvcResCfg_Get();

    if (pLivRes == NULL) {
        SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw - output liveview resource should not null!", 0U, 0U);
    } else if (pResCfg == NULL) {
        SvcLog_NG(SVC_LOG_INFO, "Failure to query default vin memory - invalid resolution configuration", 0U, 0U);
    } else {
        ULONG  BufBase = 0U;
        UINT32 BufSize = 0U;
        UINT32 VinIdx, VinSrc;
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0;

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));

        PRetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
        if (PRetVal != 0U) {
            RetVal = SVC_NG;
            SvcLog_NG(SVC_LOG_INFO, "Failure to query default vin memory - get VinIDs failure", 0U, 0U);
        } else {

            // If all vin source from dec, the system does not need to config def_raw
            RetVal = 99U;

            for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {
                VinSrc = 255U;
                PRetVal = SvcResCfg_GetVinSrc(VinIDs[VinIdx], &VinSrc);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw - get VinID(%d) source failure", VinIDs[VinIdx], 0U);
                } else {
                    // exist vin source from CFA/YUV
                    if (VinSrc != SVC_VIN_SRC_MEM_DEC) {
                        RetVal = SVC_OK;
                        break;
                    }
                }
            }
        }

        if (RetVal == SVC_OK) {
            PRetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_DEFAULT_RAW, &BufBase, &BufSize);
            if (PRetVal != SVC_OK) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw - get the default raw memory fail", 0U, 0U);
            } else {
                if (BufBase == 0U) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw - invalid buffer address!", 0U, 0U);
                }

                if (BufSize == 0U) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw - buffer size should not zero!", 0U, 0U);
                }

                if (RetVal == SVC_OK) {
                    void *pBuf = NULL;
                    AmbaMisra_TypeCast(&(pBuf), &(BufBase));
                    if (pBuf != NULL) {
                        AmbaSvcWrap_MisraMemset(pBuf, 255, BufSize);
                    }
                }
            }
        }

        if (RetVal == SVC_OK) {
            AMBA_DSP_RAW_BUF_s *pDefRaw;
            AMBA_DSP_RAW_BUF_s *pDefHds;
            UINT16 RawWidth, CmprType;

            for (VinIdx = 0U; VinIdx < VinNum; VinIdx ++) {

                PRetVal = SvcResCfg_GetVinSrc(VinIDs[VinIdx], &VinSrc);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw - get VinID(%d) source failure", VinIDs[VinIdx], 0U);
                } else {
                    pDefRaw = &(pLivRes->DefaultRawBuf[VinIDs[VinIdx]][0]);
                    pDefHds = &(pLivRes->DefaultAuxRawBuf[VinIDs[VinIdx]][0]);

                    if (VinSrc != SVC_VIN_SRC_YUV) {
                        UINT16 Pitch;

                        pDefRaw->Window.OffsetX = 0;
                        pDefRaw->Window.OffsetY = 0;
                        pDefRaw->Window.Width   = pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Width;
                        pDefRaw->Window.Height  = pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Height;

                        CmprType = SvcDefRaw[VinIDs[VinIdx]].Compressed;
                        pDefRaw->Compressed = (CmprType == 1U)?1U:0U;

                        if (VinSrc == SVC_VIN_SRC_MEM) {
                            // Video raw encode only support un-compression raw type
                            pDefRaw->Compressed = 0U;
                        }

                        RetVal = AmbaDSP_GetCmprRawBufInfo(pDefRaw->Window.Width, CmprType, &RawWidth, &Pitch);
                        pDefRaw->Pitch = Pitch;
                        if (RetVal == OK) {
                            SvcLog_DBG(SVC_LOG_INFO, "default raw liv %u x %u", pDefRaw->Pitch, pDefRaw->Window.Height );
                        }
                        if (SvcDefRaw[VinIDs[VinIdx]].CeNeeded == 1U) {
                            pDefHds->Window.OffsetX = 0U;
                            pDefHds->Window.OffsetY = 0U;
                            pDefHds->Window.Width   = pDefRaw->Window.Width >> 2U;
                            pDefHds->Window.Height  = pDefRaw->Window.Height;
                            pDefHds->Compressed     = pDefRaw->Compressed;
                            RetVal = AmbaDSP_GetCmprRawBufInfo(pDefHds->Window.Width, CmprType, &RawWidth, &Pitch);
                            pDefHds->Pitch = Pitch;
                            if (RetVal != OK) {
                                SvcLog_DBG(SVC_LOG_INFO, "default ce liv %u x %u", pDefHds->Pitch, pDefHds->Window.Height );
                            }
                        }
                        if (BufSize >= SvcDefRaw[VinIDs[VinIdx]].BufSize) {
                            pDefRaw->BaseAddr = BufBase;
                            if (SvcDefRaw[VinIDs[VinIdx]].CeNeeded == 1U) {
                                pDefHds->BaseAddr = BufBase + ((ULONG)pDefRaw->Pitch * (ULONG)pDefRaw->Window.Height);
                            }
                            BufBase += SvcDefRaw[VinIDs[VinIdx]].BufSize;
                            BufSize -= SvcDefRaw[VinIDs[VinIdx]].BufSize;
                        } else {
                            RetVal = SVC_NG;
                            SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw - remain memory cannot service VinID(%d)", VinIDs[VinIdx], 0 );
                            SvcLog_NG(SVC_LOG_INFO, "requested size 0x%x > reserved buffer 0x%x", SvcDefRaw[VinIDs[VinIdx]].BufSize, (UINT32)BufBase );
                        }
                        SvcLog_DBG(SVC_LOG_INFO, "enable still raw %u", pResCfg->StillCfg.EnableStillRaw, 0 );
                        if (pResCfg->StillCfg.EnableStillRaw == 1U) {
                            RetVal = AmbaDSP_GetCmprRawBufInfo(pResCfg->StillCfg.RawCfg[VinIDs[VinIdx]].MaxRaw.Width, pResCfg->StillCfg.RawCfg[VinIDs[VinIdx]].MaxRaw.Compressed, &RawWidth, &Pitch);
                            if (RetVal == OK) {
                                SvcLog_DBG(SVC_LOG_INFO, "still pitch %u", Pitch, 0 );
                                if (Pitch > pDefRaw->Pitch) {
                                    pDefRaw->Pitch = Pitch;
                                }
                                if (SvcDefRaw[VinIDs[VinIdx]].CeNeeded == 1U) {
                                    RetVal = AmbaDSP_GetCmprRawBufInfo(pResCfg->StillCfg.RawCfg[VinIDs[VinIdx]].MaxRaw.Width >> 2U, pResCfg->StillCfg.RawCfg[VinIDs[VinIdx]].MaxRaw.Compressed, &RawWidth, &Pitch);
                                    if (RetVal == OK) {
                                        if (Pitch > pDefHds->Pitch) {
                                            pDefHds->Pitch = Pitch;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        // YUV 422

                        // Y-channel
                        pDefRaw->Window.OffsetX = 0;
                        pDefRaw->Window.OffsetY = 0;
                        pDefRaw->Window.Width   = pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Width;
                        pDefRaw->Window.Height  = pResCfg->VinCfg[VinIDs[VinIdx]].CapWin.Height;
                        pDefRaw->Compressed     = 0U;
                        pDefRaw->Pitch          = pDefRaw->Window.Width;

                        // UV-channel
                        pDefHds->Window.OffsetX = 0U;
                        pDefHds->Window.OffsetY = 0U;
                        pDefHds->Window.Width   = pDefRaw->Window.Width;
                        pDefHds->Window.Height  = pDefRaw->Window.Height;
                        pDefHds->Compressed     = 0U;
                        pDefHds->Pitch          = pDefHds->Window.Width;

                        if (BufSize >= SvcDefRaw[VinIDs[VinIdx]].BufSize) {
                            void *pBuf = NULL;
                            ULONG TmpSize = (ULONG)pDefRaw->Pitch * (ULONG)pDefRaw->Window.Height;

                            pDefRaw->BaseAddr = BufBase;
                            pDefHds->BaseAddr = BufBase + TmpSize;

                            // reset default raw buffer as white screen
                            AmbaMisra_TypeCast(&(pBuf), &(pDefRaw->BaseAddr));
                            if (pBuf != NULL) {
                                TmpSize = (ULONG)pDefRaw->Pitch * (ULONG)pDefRaw->Window.Height;
                                AmbaSvcWrap_MisraMemset(pBuf, 255, TmpSize);
                            }
                            AmbaMisra_TypeCast(&(pBuf), &(pDefHds->BaseAddr));
                            if (pBuf != NULL) {
                                TmpSize = (ULONG)pDefHds->Pitch * (ULONG)pDefHds->Window.Height;
                                AmbaSvcWrap_MisraMemset(pBuf, 255, TmpSize);
                            }

                            BufBase += SvcDefRaw[VinIDs[VinIdx]].BufSize;
                            BufSize -= SvcDefRaw[VinIDs[VinIdx]].BufSize;
                        } else {
                            RetVal = SVC_NG;
                            SvcLog_NG(SVC_LOG_INFO, "Failure to cfg default raw yuv fmt - remain memory cannot service VinID(%d)", VinIDs[VinIdx], 0 );
                            SvcLog_NG(SVC_LOG_INFO, "requested size 0x%x > reserved buffer 0x%x", SvcDefRaw[VinIDs[VinIdx]].BufSize, BufSize );
                        }

                        SvcLog_DBG(SVC_LOG_INFO, "Configure default raw yuv fmt done!", 0U, 0U);
                    }
                }

                if (RetVal != SVC_OK) {
                    break;
                }
            }
        }

        if (RetVal == SVC_OK) {

            if (BufBase > 0U) {
#if defined(CONFIG_THREADX)
                PRetVal = SvcPlat_CacheClean(BufBase, BufSize);
                if (PRetVal != 0U) {
                    SvcLog_NG(SVC_LOG_INFO, "clean default raw buffer fail. ErrCode(0x%08X)", PRetVal, 0U);
                }
#endif
            }

            SvcLog_OK(SVC_LOG_INFO, "Successful to cfg default raw", 0U, 0U);
        }
    }
}

/**
 * Query liveview external data feeding feeding task mem buffer size, based on the fov from external memory
 * @param [out] Worksize of the osd module
 * return 0-OK, 1-NG
 */
UINT32 SvcInfoPack_ExtDataMemSizeQuery(UINT32 *pWorkSize)
{
    #define ICAM_EXT_DATA_BUF_NUM    (4U)
    UINT32 FovIdx, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum = 0U;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    UINT32 RetVal = SVC_OK;

    if (pWorkSize == NULL) {
        SvcLog_NG(SVC_LOG_INFO, "SvcInfoPack_ExtDataMemSizeQuery failed. Input should not be NULL", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        *pWorkSize = 0U;
        AmbaSvcWrap_MisraMemset(FovIdxs, 0, sizeof(FovIdxs));
        if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxs, &FovNum)) {
            for (UINT32 i = 0U; i < FovNum; i++) {
                UINT32 ReqSize = 0U;
                FovIdx = FovIdxs[i];

                ReqSize = (UINT32) pResCfg->FovCfg[FovIdx].RawWin.Width * (UINT32) pResCfg->FovCfg[FovIdx].RawWin.Height;
                ReqSize = (UINT32) (ReqSize << 1U);
                ReqSize = ReqSize * ICAM_EXT_DATA_BUF_NUM; /* Ring */
                *pWorkSize = *pWorkSize + ReqSize;
            }
        }
    }

    return RetVal;
}

