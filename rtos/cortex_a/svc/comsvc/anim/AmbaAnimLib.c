/**
 *  @file AmbaAnimLib.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Svc Animation Library
 *
 */

#include <AmbaDSP_ImageFilter.h>
#include <AmbaDSP_ImageUtility.h>
#include <AmbaDSP_Liveview.h>
#include <AmbaDSP_VOUT.h>
#include <AmbaCache.h>
#include "AmbaAnimLib.h"


static SVC_ANIM_LIB_SYS_CFG_s g_SysCfg GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

static inline UINT16 U16MAX(UINT16 a, UINT16 b)            {return ((a > b) ? a : b);}

UINT32 SvcAnimLib_Init(const SVC_ANIM_LIB_SYS_CFG_s *Cfg)
{
    UINT32 Rval = OK;
    if (Cfg == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Cfg is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(&g_SysCfg, Cfg, sizeof(SVC_ANIM_LIB_SYS_CFG_s));
    }
    return Rval;
}

UINT32 SvcAnimLib_Load(UINT32 HdlrId, UINT32 GroupId, UINT32 ItemId, UINT8 ItemCount, UINT8 *LoadBuf, UINT32 LoadBufSize)
{
    UINT32 Rval = OK;
    if (LoadBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: LoadBuf is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else {
        Rval = AmbaCalibData_Read(HdlrId, GroupId, (UINT8)ItemId, ItemCount, LoadBuf, LoadBufSize);
        if (Rval != OK){
            AmbaPrint_PrintUInt5("[ERROR] SvcAnimLib_Load: AmbaCalibData_Read() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

UINT32 SvcAnimLib_Decompress(UINT8 Algo, const AMBA_CALIB_DATA_DECMPR_INFO_s *Info, const UINT8 *SrcBuf, UINT32 SrcBufSize, UINT8 *DstBuf, UINT32 DstBufSize)
{
    UINT32 Rval = OK;
    if (Info == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Info is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else if (SrcBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: SrcBuf is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else if (DstBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: DstBuf is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else {
        UINT32 OutputSize = 0U;
        Rval = AmbaCalibData_Decompress(Algo, Info, SrcBuf, SrcBufSize, DstBuf, DstBufSize, &OutputSize);
        if ((Rval != OK) || (OutputSize == 0U)){
            AmbaPrint_PrintUInt5("[ERROR] SvcAnimLib_Decompress: AmbaCalibData_Decompress() failed! Rval = %u, OutputSize = %u", Rval, OutputSize, 0U, 0U, 0U);
        }
    }
    return Rval;
}

UINT32 SvcAnimLib_IkCalWarp(UINT16 ViewZoneId, const AMBA_IK_WARP_INFO_s *Info, const UINT8 *WorkBuf, UINT32 WorkSize, const UINT8 *OutBuf, UINT32 OutSize)
{
    UINT32 Rval = OK;
    if (Info == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Info is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else if (WorkBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: WorkBuf is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else if (OutBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: OutBuf is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else if (ViewZoneId >= SVC_ANIM_LIB_MAX_VIEWZONE) {
        AmbaPrint_PrintStr5("[ERROR] %s: OutBuf is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else {
        AMBA_IK_MODE_CFG_s ImgMode;
        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
        AMBA_IK_DUMMY_MARGIN_RANGE_s DmyMarginRange;
        AMBA_IK_DZOOM_INFO_s DzoomInfo;
        AMBA_IK_VIN_ACTIVE_WINDOW_s ActWin;
        AMBA_IK_CALC_GEO_SETTINGS_IN_s CalcGeoInPar;
        AMBA_IK_CALC_GEO_SETTINGS_s CalcGeoInfo;
        AMBA_IK_CALC_GEO_RESULT_s CalcGeoRst;

        ImgMode.ContextId = ViewZoneId;

        Rval = AmbaIK_GetWindowSizeInfo(&ImgMode, &WindowSizeInfo);
        if (Rval == OK) {
            Rval = AmbaIK_GetDummyMarginRange(&ImgMode, &DmyMarginRange);
            if (Rval == OK) {
                AMBA_IK_ABILITY_s IkAbility;
                (void)AmbaIK_GetContextAbility(&ImgMode, &IkAbility);
                Rval = AmbaIK_GetDzoomInfo(&ImgMode, &DzoomInfo);
                if (Rval == OK) {
                    ActWin.Enable = 1U;
                    ActWin.ActiveGeo.StartX = Info->VinSensorGeo.StartX;
                    ActWin.ActiveGeo.StartY = Info->VinSensorGeo.StartY;
                    ActWin.ActiveGeo.Width  = Info->VinSensorGeo.Width;
                    ActWin.ActiveGeo.Height = Info->VinSensorGeo.Height;

                    CalcGeoInPar.pWindowSizeInfo = &WindowSizeInfo;
                    CalcGeoInPar.pVinActiveWindow = &ActWin;
                    CalcGeoInPar.pDmyRange = &DmyMarginRange;
                    CalcGeoInPar.pDzoomInfo = &DzoomInfo;
                    CalcGeoInPar.FlipMode = 0U;
                    CalcGeoInPar.WarpEnable = 1U;
                    CalcGeoInPar.UseCawarp = 0;
                    CalcGeoInPar.CawarpEnable = 0;
                    CalcGeoInPar.pCalibCaWarpInfo = NULL;

                    CalcGeoInfo.pIn = &CalcGeoInPar;
                    AmbaMisra_TypeCast32(&CalcGeoInPar.pWarpInfo, &Info);
                    AmbaMisra_TypeCast32(&CalcGeoInfo.pWorkBuf, &WorkBuf);
                    CalcGeoInfo.WorkBufSize = WorkSize;
                    AmbaMisra_TypeCast32(&CalcGeoRst.pOut, &OutBuf);
                    CalcGeoRst.OutBufSize = OutSize;
#ifdef CONFIG_SOC_CV2FS
                    Rval = AmbaIK_CalcGeoSettings(&IkAbility, &CalcGeoInfo, &CalcGeoRst);
#else
                    Rval = AmbaIK_CalcGeoSettings(&CalcGeoInfo, &CalcGeoRst);
#endif
                    if (Rval != OK) {
                        AmbaPrint_PrintUInt5("RefAnimPlay_WarpUpdate: AmbaIK_CalcGeoSettings() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("RefAnimPlay_WarpUpdate: AmbaIK_GetDzoomInfo() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("RefAnimPlay_WarpUpdate: AmbaIK_GetDummyMarginRange() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("RefAnimPlay_WarpUpdate: AmbaIK_GetWindowSizeInfo() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 SvcAnimLib_ShowOsd(const SVC_ANIM_LIB_VOUT_CFG_s *VoutCfg, const SVC_ANIM_LIB_OSD_CFG_s *AnimOsdCfg, const UINT8 *OsdBuf)
{
    UINT8 Enable = 1U, SyncWithVin = 1U;
    UINT32 Rval = OK;
    AMBA_DSP_VOUT_OSD_BUF_CONFIG_s DspOsdCfg = {0};
    if (OsdBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: OsdBuf is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    } else {
        DspOsdCfg.InputWidth     = AnimOsdCfg->BufWidth;
        DspOsdCfg.InputHeight    = AnimOsdCfg->BufHeight;
        DspOsdCfg.Pitch          = AnimOsdCfg->BufPitch;
        DspOsdCfg.Window         = VoutCfg->VoutWin;
        DspOsdCfg.FieldRepeat    = 0U;
        DspOsdCfg.DataFormat     = AnimOsdCfg->DataFormat;
        DspOsdCfg.CLUTAddr       = AnimOsdCfg->ClutAddr;
        Rval = AmbaCache_DataClean(DspOsdCfg.CLUTAddr, AnimOsdCfg->ClutSize);
        if (Rval == OK) {
            /* OsdBuf is non-cache, no need to clean cache */
            AmbaMisra_TypeCast32(&DspOsdCfg.BaseAddr, &OsdBuf);
            Rval = AmbaDSP_VoutOsdConfigBuf(VoutCfg->VoutId, &DspOsdCfg);
            if (Rval == OK) {
                UINT64 AttachedRawSeq;
                Rval = AmbaDSP_VoutOsdCtrl(VoutCfg->VoutId, Enable, SyncWithVin, &AttachedRawSeq);
                if (Rval != OK) {
                    AmbaPrint_PrintUInt5("SvcAnimLib_ShowOsd: AmbaDSP_VoutOsdCtrl() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("SvcAnimLib_ShowOsd: AmbaDSP_VoutOsdConfigBuf() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("SvcAnimLib_ShowOsd: AmbaCache_DataClean() failed!, OsdCfg.CLUTAddr = 0x%x, CleanSize = %u", DspOsdCfg.CLUTAddr, AnimOsdCfg->ClutSize, 0U, 0U, 0U);
        }
    }
    return Rval;
}

UINT32 SvcAnimLib_UpdateView(UINT8 ViewZoneCount, UINT8 DispCount, SVC_ANIM_LIB_OFFSET_s DispOffset[SVC_ANIM_LIB_MAX_DISP],
        UINT8 *OsdBuf[SVC_ANIM_LIB_MAX_VOUT], const UINT16 ViewZoneId[SVC_ANIM_LIB_MAX_VIEWZONE],
        const UINT8 IkGeoEnable[SVC_ANIM_LIB_MAX_VIEWZONE], UINT8 *IkGeoResult[SVC_ANIM_LIB_MAX_VIEWZONE])
{
    UINT8 SyncYuvStrmId, i;
    UINT32 Rval = OK, SyncJobId;
    UINT64 AttachedRawSeq;
    AMBA_DSP_LIVEVIEW_SYNC_CTRL_s SyncCtrl;
    AmbaMisra_TouchUnused(IkGeoResult);
    AmbaMisra_TouchUnused(OsdBuf);
    for (i = 0U; i < g_SysCfg.VoutCount; i++) {
        /* Lock YuvStrm */
        SyncCtrl.Opt = AMBA_DSP_YUVSTRM_SYNC_START;
        SyncYuvStrmId = (UINT8)g_SysCfg.YuvStrmCfg[i].YuvStrmId;
        Rval = AmbaDSP_LiveviewYuvStreamSync(SyncYuvStrmId, &SyncCtrl, &SyncJobId, &AttachedRawSeq);
        if (Rval == OK) {
            AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: Sync job %u start!", SyncJobId, 0U, 0U, 0U, 0U);
            /* OSD Update */
            Rval = SvcAnimLib_ShowOsd(&g_SysCfg.VoutCfg[i], &g_SysCfg.OsdCfg[i], OsdBuf[i]);
            if (Rval == OK) {
                /* Layout Update */
                UINT8 j, ChanInfoIdx = 0U;
                UINT16 TotalChanCount = 0U;
                const UINT16 NumYuvStream = 1U;
                AMBA_DSP_LIVEVIEW_STREAM_CFG_s LvStrmCfg = {0};
                AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s ChanInfo[SVC_ANIM_LIB_MAX_CHAN] = {0};
                LvStrmCfg.StreamId  = SyncYuvStrmId;
                LvStrmCfg.Purpose   = g_SysCfg.YuvStrmCfg[i].Purpose;
                LvStrmCfg.DestVout  = (UINT16)1U << g_SysCfg.VoutCfg[i].VoutId;
                LvStrmCfg.DestEnc   = g_SysCfg.YuvStrmCfg[i].EncDest;
                LvStrmCfg.Width     = g_SysCfg.VoutCfg[i].YuvWidth;
                LvStrmCfg.Height    = g_SysCfg.VoutCfg[i].YuvHeight;
                LvStrmCfg.MaxWidth  = U16MAX(LvStrmCfg.Width, LvStrmCfg.Height);
                LvStrmCfg.MaxHeight = U16MAX(LvStrmCfg.Width, LvStrmCfg.Height);
                LvStrmCfg.StreamBuf.AllocType = ALLOC_INTERNAL;
                LvStrmCfg.StreamBuf.BufNum = 0U;
                LvStrmCfg.StreamBuf.pYuvBufTbl = NULL;
                for (j = 0U; j < DispCount; j++) {
//                    TotalChanCount += Index[j]->ChanCount;
                }
                LvStrmCfg.NumChan = TotalChanCount;
                LvStrmCfg.pChanCfg = &ChanInfo[0];
                for (j = 0U; j < DispCount; j++) {
                    (void)DispOffset;
//                    for (k = 0U; k < Index[j]->ChanCount; k++) {
//                        ChanInfo[ChanInfoIdx] = Index[j]->RenderCfg[i][k].ChanInfo;
//                        ChanInfo[ChanInfoIdx].ROI = g_SysCfg.YuvStrmCfg[i].Roi[]; // TODO
                        {
                            const AMBA_DSP_BUF_s *BlendTbl;
                            AmbaMisra_TypeCast32(&BlendTbl, &ChanInfo[ChanInfoIdx].LumaAlphaTable);
                            AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: ChanInfoIdx = %u, ViewZoneId = %u (LvStrmCfg):", ChanInfoIdx, ChanInfo[ChanInfoIdx].ViewZoneId, 0U, 0U, 0U);
                            AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: NumChan = %u, (X, Y, W, H) = (%u, %u, %u, %u)", LvStrmCfg.NumChan, ChanInfo[ChanInfoIdx].Window.OffsetX, ChanInfo[ChanInfoIdx].Window.OffsetY, ChanInfo[ChanInfoIdx].Window.Width, ChanInfo[ChanInfoIdx].Window.Height);
                            AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: ROI = (%u, %u, %u, %u), RotateFlip = %u", ChanInfo[ChanInfoIdx].ROI.OffsetX, ChanInfo[ChanInfoIdx].ROI.OffsetY, ChanInfo[ChanInfoIdx].ROI.Width, ChanInfo[ChanInfoIdx].ROI.Height, ChanInfo[ChanInfoIdx].RotateFlip);
                            if (BlendTbl != NULL) {
                                AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: BlendNum = %u, BlendWindow = (%u, %u, %u, %u)", ChanInfo[ChanInfoIdx].BlendNum, BlendTbl->Window.OffsetX, BlendTbl->Window.OffsetY, BlendTbl->Window.Width, BlendTbl->Window.Height);
                                AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: Blend: Pitch = %u, BaseAddr = 0x%x", BlendTbl->Pitch, BlendTbl->BaseAddr, 0U, 0U, 0U);
                            }
                        }
                        ChanInfoIdx++;
//                    }
                }
                Rval = AmbaDSP_LiveviewUpdateConfig(NumYuvStream, &LvStrmCfg, &AttachedRawSeq);
                if (Rval == OK) {
                    /* Layout Update Geo Cfg */
                    AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s GeoCtrl = {0U};
                    for (j = 0U; j < ViewZoneCount; j++) {
                        if ((IkGeoEnable[j] == 1U) && (Rval == OK)) {
                            AmbaMisra_TypeCast32(&GeoCtrl.GeoCfgAddr, &IkGeoResult[j]);
                            Rval = AmbaDSP_LiveviewUpdateGeoCfg(ViewZoneId[j], &GeoCtrl, &AttachedRawSeq);
                            if (Rval != OK) {
                                AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: AmbaDSP_LiveviewUpdateGeoCfg() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                            }
                            (void)AmbaKAL_TaskSleep(1); // To be deleted after fixed by SSP or ucode
                        }
                    }
                    if (Rval == OK) {
                        /* Execute YuvStrm */
                        SyncCtrl.Opt = AMBA_DSP_YUVSTRM_SYNC_EXECUTE;
                        Rval = AmbaDSP_LiveviewYuvStreamSync(SyncYuvStrmId, &SyncCtrl, &SyncJobId, &AttachedRawSeq);
                        if (Rval != OK) {
                            AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: AmbaDSP_LiveviewYuvStreamSync(EXECUTE) failed! Rval = %u, SyncJobId = %u", Rval, SyncJobId, 0U, 0U, 0U);
                        }
                    }
                } else {
                    AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: AmbaDSP_LiveviewUpdateConfig() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: SvcAnimLib_ShowOsd() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("SvcAnimLib_UpdateView: AmbaDSP_LiveviewYuvStreamSync(START) failed! Rval = %u, SyncJobId = %u", Rval, SyncJobId, 0U, 0U, 0U);
        }
    }
    return Rval;
}

