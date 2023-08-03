/**
 *  @file SvcAnimTask.c
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
 *  @details svc animation task
 *
 */
#include <AmbaSD_Def.h>
#include <AmbaDSP_VOUT_Def.h>
#include "AmbaCalibData.h"
#include "AmbaCalib_EmirrorDef.h"
#include "AmbaUtility.h"


#include "SvcAnim.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcLog.h"
#include "SvcAnimTask.h"
#include "SvcCalibAdas.h"

typedef struct {
    AMBA_DSP_WINDOW_s           OsdDrawWin;
    void                        *pBinBuf;
    UINT32                      HaveExtData;
    AMBA_DSP_WINDOW_s           BaseDrawWin;
    UINT32                      BaseDataDraw;
} SVC_ANIMOSD_DATA_s;

#define SVC_LOG_ANIM_TASK     "SVC_ANIM_TASK"
#define SVC_ANIM_OSD_GUI_LEVEL           (13U)
#define SVC_ANIM_BLACK_GUI_LEVEL         (5U)

#define SVC_ANIM_OSD_GUI_DRAW       (1U)
#define SVC_ANIM_OSD_GUI_UPDATE     (2U)

static SVC_ANIM_DEFAULT_DATA_s g_SvcAnimDefaultData;
static UINT32 g_InitFlag = 0U;
static SVC_ANIMOSD_DATA_s DrawConfig[SVC_ANIM_LIB_MAX_DISP];
static AMBA_KAL_MUTEX_t AnimOsdMutex GNU_SECTION_NOZEROINIT;

static void SvcAnimOsd_MutexCreate(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&AnimOsdMutex, NULL)) {
        SvcLog_NG(SVC_LOG_ANIM_TASK, "SvcAnimOsd_MutexCreate error", 0U, 0U);
    }
}

static void SvcAnimOsd_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&AnimOsdMutex, 5000)) {
        SvcLog_NG(SVC_LOG_ANIM_TASK, "SvcAnimOsd_MutexTake: timeout", 0U, 0U);
    }
}

static void SvcAnimOsd_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&AnimOsdMutex)) {
        SvcLog_NG(SVC_LOG_ANIM_TASK, "SvcAnimOsd_MutexGive: error", 0U, 0U);
    }
}

static void SvcExtOsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((DrawConfig[0].HaveExtData & SVC_ANIM_OSD_GUI_DRAW) > 0U) {
        UINT32 EndX, EndY;
        EndX = (UINT32)DrawConfig[0].OsdDrawWin.Width + DrawConfig[0].OsdDrawWin.OffsetX;
        EndY = (UINT32)DrawConfig[0].OsdDrawWin.Height + DrawConfig[0].OsdDrawWin.OffsetY;
        SvcAnimOsd_MutexTake();
        (void)SvcOsd_DrawBin(VOUT_IDX_B, (UINT32)DrawConfig[0].OsdDrawWin.OffsetX, (UINT32)DrawConfig[0].OsdDrawWin.OffsetY,
                            EndX, EndY, DrawConfig[0].pBinBuf);
        SvcAnimOsd_MutexGive();
    }
}

static void SvcExtOsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    if ((DrawConfig[0].HaveExtData & SVC_ANIM_OSD_GUI_UPDATE) > 0U) {
        DrawConfig[0].HaveExtData &= ~(SVC_ANIM_OSD_GUI_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void SvcBaseOsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((DrawConfig[0].BaseDataDraw & SVC_ANIM_OSD_GUI_DRAW) > 0U) {
        UINT32 EndX, EndY;
        EndX = (UINT32)DrawConfig[0].BaseDrawWin.Width + DrawConfig[0].BaseDrawWin.OffsetX;
        EndY = (UINT32)DrawConfig[0].BaseDrawWin.Height + DrawConfig[0].BaseDrawWin.OffsetY;
        SvcAnimOsd_MutexTake();
        (void)SvcOsd_DrawSolidRect(VOUT_IDX_B, (UINT32)DrawConfig[0].BaseDrawWin.OffsetX, (UINT32)DrawConfig[0].BaseDrawWin.OffsetY,
                                  EndX, EndY, 248U);
        SvcAnimOsd_MutexGive();
    }
}

static void SvcBaseOsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    if ((DrawConfig[0].BaseDataDraw & SVC_ANIM_OSD_GUI_UPDATE) > 0U) {
        DrawConfig[0].BaseDataDraw &= ~(SVC_ANIM_OSD_GUI_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static UINT32 SvcAnimTask_EventCb(UINT8 EventId, UINT32 LayoutId)
{
    UINT32 Rval = SVC_OK;
    (void)EventId;
    (void)LayoutId;
    return Rval;
}

static UINT32 SvcAnimTask_OsdChangeCb(SVC_ANIM_EVENT_INFO_s *EventInfo)
{
    UINT32 Rval = SVC_OK;
    (void)EventInfo;
    if (EventInfo == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: EventInfo is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    }
    return Rval;
}

static UINT32 SvcAnimTask_OsdDoneCb(SVC_ANIM_EVENT_INFO_s *EventInfo)
{
    UINT32 Rval = SVC_OK;
    UINT32 Tmp;
    (void)EventInfo;
    if (EventInfo == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: EventInfo is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT8 i, Found = 0U;
        for (i = 0U; i < EventInfo->VoutCount; i++) {
            if (EventInfo->VoutId[i] == VOUT_IDX_B) {
                Found = 1U;
                break;
            }
        }
        if (Found == 1U) {
            SVC_CALIB_ADAS_NAND_TABLE_s Cfg;
            SVC_CALIB_ADAS_NAND_TABLE_s *pCfg = &Cfg;
            UINT32 Chan,RetVal;
            Chan = SVC_CALIB_ADAS_TYPE_AVM_F;
            AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
            RetVal = SvcCalib_AdasCfgGet(Chan, pCfg);
            if (RetVal == SVC_OK) {
                if ((Cfg.AdasAvm2DCfg.CarVoutOSD.Width == 0U) || (Cfg.AdasAvm2DCfg.CarVoutOSD.Height == 0U)) {
                    DrawConfig[0].BaseDrawWin.Width = (UINT16)Cfg.AdasAvm2DCfg.CarVoutOSD.Width;
                    DrawConfig[0].BaseDrawWin.Height= (UINT16)Cfg.AdasAvm2DCfg.CarVoutOSD.Height;
                } else {
                    DrawConfig[0].BaseDrawWin.Width = 118U;
                    DrawConfig[0].BaseDrawWin.Height= 280U;
                }
            } else {
                DrawConfig[0].BaseDrawWin.Width = 118U;
                DrawConfig[0].BaseDrawWin.Height= 280U;
            }
            Tmp = (960U - (UINT32)DrawConfig[0].BaseDrawWin.Width) / 2U;
            DrawConfig[0].BaseDrawWin.OffsetX = (UINT16)Tmp;
            Tmp = (540U - (UINT32)DrawConfig[0].BaseDrawWin.Height) / 2U;
            DrawConfig[0].BaseDrawWin.OffsetY = (UINT16)Tmp;
            DrawConfig[0].BaseDataDraw |= SVC_ANIM_OSD_GUI_DRAW;
            DrawConfig[0].BaseDataDraw |= SVC_ANIM_OSD_GUI_UPDATE;

            //DrawConfig[0].HaveExtData |= SVC_ANIM_OSD_GUI_DRAW;
            //DrawConfig[0].HaveExtData |= SVC_ANIM_OSD_GUI_UPDATE;
            AmbaSvcWrap_MisraMemcpy(&(DrawConfig[0].OsdDrawWin), &(EventInfo->OsdInfo[i].Window[0][0]), sizeof(AMBA_DSP_WINDOW_s));
            Tmp = (960U - SVC_OSD_WIDTH) / 2U;
            DrawConfig[0].OsdDrawWin.OffsetX = (UINT16)Tmp;
            Tmp = (540U - SVC_OSD_HEIGH) / 2U;
            DrawConfig[0].OsdDrawWin.OffsetY = (UINT16)Tmp;
            AmbaMisra_TypeCast(&(DrawConfig[0].pBinBuf), &EventInfo->OsdInfo[0].Buffer);
        }
   }
    return Rval;
}

UINT32 SvcAnimTask_Config(void)
{
    UINT32 Rval = SVC_OK;
    UINT8 i;
    SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
    if (g_InitFlag == 1U) {
        Rval = SvcAnim_GetDefaultData(&g_SvcAnimDefaultData);
        if (Rval == SVC_OK) {
            g_InitFlag = 2U;
            for (i = 0U; i < pCfg->DispStrm[0].StrmCfg.NumChan; i++) {
                pCfg->DispStrm[0].StrmCfg.ChanCfg[i].DstWin.Width = g_SvcAnimDefaultData.ChanInfo[0][i].Window.Width;
                pCfg->DispStrm[0].StrmCfg.ChanCfg[i].DstWin.Height = g_SvcAnimDefaultData.ChanInfo[0][i].Window.Height;
                pCfg->DispStrm[0].StrmCfg.ChanCfg[i].DstWin.OffsetX= g_SvcAnimDefaultData.ChanInfo[0][i].Window.OffsetX;
                pCfg->DispStrm[0].StrmCfg.ChanCfg[i].DstWin.OffsetY= g_SvcAnimDefaultData.ChanInfo[0][i].Window.OffsetY;
                pCfg->DispStrm[0].StrmCfg.ChanCfg[i].RotateFlip = g_SvcAnimDefaultData.ChanInfo[0][i].RotateFlip;
            }
        }
    } else {
        AmbaPrint_PrintUInt5("SvcAnimTask_Config SvcAnimTask not initial", 0U, 0U, 0U, 0U, 0U);
        Rval = SVC_NG;
    }
    return Rval;
}

UINT32 SvcAnimTask_GetBlendTbl(UINT32 ChanIdx, UINT32 *Tbl)
{
    UINT32 Rval = SVC_OK;
    if (g_InitFlag == 2U) {
        *Tbl = g_SvcAnimDefaultData.ChanInfo[0][ChanIdx].LumaAlphaTable;
    } else {
        Rval = SVC_NG;
        AmbaPrint_PrintUInt5("SvcAnimTask_GetBlendTbl SvcAnimTask not initial/config", 0U, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

UINT32 SvcAnimTask_UpdateWarp(void)
{
    static AMBA_IK_WARP_INFO_s g_SvcAnimWarpInfo;
    UINT32 Rval = SVC_OK;
    UINT8 i;
    if (g_InitFlag == 2U) {
        for (i = 0U; i < 4U; i++) {//TODO
            if(g_SvcAnimDefaultData.WarpEnable[i] == 1U) {
                AMBA_IK_MODE_CFG_s ImgMode = {0};

                ImgMode.ContextId = g_SvcAnimDefaultData.ChanInfo[0][i].ViewZoneId;
                g_SvcAnimWarpInfo.Version                            = g_SvcAnimDefaultData.IkWarpInfo[i].Version;
                g_SvcAnimWarpInfo.HorGridNum                         = g_SvcAnimDefaultData.IkWarpInfo[i].HorGridNum;
                g_SvcAnimWarpInfo.VerGridNum                         = g_SvcAnimDefaultData.IkWarpInfo[i].VerGridNum;
                g_SvcAnimWarpInfo.TileWidthExp                       = g_SvcAnimDefaultData.IkWarpInfo[i].TileWidthExp;
                g_SvcAnimWarpInfo.TileHeightExp                      = g_SvcAnimDefaultData.IkWarpInfo[i].TileHeightExp;
                g_SvcAnimWarpInfo.VinSensorGeo.Width                 = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.Width;
                g_SvcAnimWarpInfo.VinSensorGeo.Height                = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.Height;
                g_SvcAnimWarpInfo.VinSensorGeo.StartX                = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.StartX;
                g_SvcAnimWarpInfo.VinSensorGeo.StartY                = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.StartY;
                g_SvcAnimWarpInfo.VinSensorGeo.HSubSample.FactorDen  = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.HSubSample.FactorDen;
                g_SvcAnimWarpInfo.VinSensorGeo.HSubSample.FactorNum  = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.HSubSample.FactorNum;
                g_SvcAnimWarpInfo.VinSensorGeo.VSubSample.FactorDen  = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.VSubSample.FactorDen;
                g_SvcAnimWarpInfo.VinSensorGeo.VSubSample.FactorNum  = g_SvcAnimDefaultData.IkWarpInfo[i].VinSensorGeo.VSubSample.FactorNum;
                g_SvcAnimWarpInfo.Enb_2StageCompensation             = g_SvcAnimDefaultData.IkWarpInfo[i].Enb_2StageCompensation;
                AmbaMisra_TypeCast(&g_SvcAnimWarpInfo.pWarp, &g_SvcAnimDefaultData.IkWarpInfo[i].pWarp);
                Rval = AmbaIK_SetWarpInfo(&ImgMode, &g_SvcAnimWarpInfo);
                if (Rval != SVC_OK) {
                    AmbaPrint_PrintUInt5("SvcAnimTask_UpdateWarp AmbaIK_SetWarpInfo chan:%d Fail(%d)", i, Rval, 0U, 0U, 0U);
                }

                Rval = AmbaIK_SetWarpEnb(&ImgMode, g_SvcAnimDefaultData.WarpEnable[i]);
                if (Rval != SVC_OK) {
                    AmbaPrint_PrintUInt5("SvcAnimTask_UpdateWarp AmbaIK_SetWarpEnb chan:%d Fail(%d)", i, Rval, 0U, 0U, 0U);
                }
            }
        }
    } else {
        Rval = SVC_NG;
        AmbaPrint_PrintUInt5("SvcAnimTask_UpdateWarp SvcAnimTask not initial/config", 0U, 0U, 0U, 0U, 0U);
    }
    return Rval;
}


UINT32 SvcAnimTask_Init(void)
{
    static UINT32 g_ClutAddr = 0U;
    UINT32 Rval = SVC_OK;
    UINT32 NCBufSize = 0U;
    UINT32 CABufSize = 0U;
    ULONG NCBufBase = 0U, CABufBase = 0U;

    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_SUR_NC, &NCBufBase, &NCBufSize); //TODO
    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_SUR_CA, &CABufBase, &CABufSize); //TODO
    if (Rval == SVC_OK) {
        SVC_ANIM_INIT_CFG_s InitCfg;
        const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
        UINT32 *pClut, NumColor;

        InitCfg.VoutCount = (UINT8)pCfg->DispNum;
        /* Vout config */
        InitCfg.VoutCfg[0].VoutId = (UINT8)pCfg->DispStrm[0].VoutID;
        InitCfg.VoutCfg[0].YuvWidth = pCfg->DispStrm[0].StrmCfg.Win.Width;
        InitCfg.VoutCfg[0].YuvHeight = pCfg->DispStrm[0].StrmCfg.Win.Height;
        InitCfg.VoutCfg[0].VoutWin.OffsetX = 0U;
        InitCfg.VoutCfg[0].VoutWin.OffsetY = 0U;
        InitCfg.VoutCfg[0].VoutWin.Width = pCfg->DispStrm[0].StrmCfg.Win.Width;
        InitCfg.VoutCfg[0].VoutWin.Height = pCfg->DispStrm[0].StrmCfg.Win.Height;
        /* Osd config */
        Rval = SvcOsd_GetOsdCLUT(pCfg->DispStrm[0].VoutID, &pClut, &NumColor);
        AmbaMisra_TypeCast(&g_ClutAddr, &pClut);
        if (Rval != SVC_OK) {
            AmbaPrint_PrintUInt5("[ERROR] SvcOsd_GetOsdCLUT() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
        InitCfg.OsdCfg[0].DataFormat = OSD_8BIT_CLUT_MODE;
        InitCfg.OsdCfg[0].ClutAddr = g_ClutAddr;
        InitCfg.OsdCfg[0].ClutSize = 256U;//TODO
        InitCfg.OsdCfg[0].BufPitch = SVC_OSD_WIDTH;//TODO
        InitCfg.OsdCfg[0].BufWidth = SVC_OSD_WIDTH;//TODO
        InitCfg.OsdCfg[0].BufHeight = SVC_OSD_HEIGH;//TODO
        /* Yuv strm config */
        InitCfg.YuvStrmCfg[0].YuvStrmId = 1U;
        InitCfg.YuvStrmCfg[0].Purpose = ((UINT16)1U << DSP_LV_STRM_PURPOSE_MEMORY_IDX) | ((UINT16)1U << DSP_LV_STRM_PURPOSE_VOUT_IDX);
        InitCfg.YuvStrmCfg[0].EncDest = 0U;
        InitCfg.YuvStrmCfg[0].Roi[0].OffsetX = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetX;
        InitCfg.YuvStrmCfg[0].Roi[0].OffsetY = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetY;
        InitCfg.YuvStrmCfg[0].Roi[0].Width = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Width;
        InitCfg.YuvStrmCfg[0].Roi[0].Height = pCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Height;
        InitCfg.YuvStrmCfg[0].Roi[1].OffsetX = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.OffsetX;
        InitCfg.YuvStrmCfg[0].Roi[1].OffsetY = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.OffsetY;
        InitCfg.YuvStrmCfg[0].Roi[1].Width = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.Width;
        InitCfg.YuvStrmCfg[0].Roi[1].Height = pCfg->DispStrm[0].StrmCfg.ChanCfg[1].SrcWin.Height;
        InitCfg.YuvStrmCfg[0].Roi[2].OffsetX = pCfg->DispStrm[0].StrmCfg.ChanCfg[2].SrcWin.OffsetX;
        InitCfg.YuvStrmCfg[0].Roi[2].OffsetY = pCfg->DispStrm[0].StrmCfg.ChanCfg[2].SrcWin.OffsetY;
        InitCfg.YuvStrmCfg[0].Roi[2].Width = pCfg->DispStrm[0].StrmCfg.ChanCfg[2].SrcWin.Width;
        InitCfg.YuvStrmCfg[0].Roi[2].Height = pCfg->DispStrm[0].StrmCfg.ChanCfg[2].SrcWin.Height;
        InitCfg.YuvStrmCfg[0].Roi[3].OffsetX = pCfg->DispStrm[0].StrmCfg.ChanCfg[3].SrcWin.OffsetX;
        InitCfg.YuvStrmCfg[0].Roi[3].OffsetY = pCfg->DispStrm[0].StrmCfg.ChanCfg[3].SrcWin.OffsetY;
        InitCfg.YuvStrmCfg[0].Roi[3].Width = pCfg->DispStrm[0].StrmCfg.ChanCfg[3].SrcWin.Width;
        InitCfg.YuvStrmCfg[0].Roi[3].Height = pCfg->DispStrm[0].StrmCfg.ChanCfg[3].SrcWin.Height;
        AmbaMisra_TypeCast(&InitCfg.CacheWorkBuf, &CABufBase);
        InitCfg.CacheWorkBufSize = CABufSize;
        AmbaMisra_TypeCast(&InitCfg.NonCacheWorkBuf, &NCBufBase);
        InitCfg.NonCacheWorkBufSize = NCBufSize;
        InitCfg.ChannelId[0U][0] = 0U;
        InitCfg.ChannelId[0U][1] = 1U;
        InitCfg.ChannelId[0U][2] = 2U;
        InitCfg.ChannelId[0U][3] = 3U;
        InitCfg.ViewZoneId[0U][0] = 4U;
        InitCfg.ViewZoneId[0U][1] = 5U;
        InitCfg.ViewZoneId[0U][2] = 6U;
        InitCfg.ViewZoneId[0U][3] = 7U;
        InitCfg.DispOffset[0U].OffsetX = 0U;
        InitCfg.DispOffset[0U].OffsetY = 0U;
        /* Default config for liveview data */
        InitCfg.DefaultDataCfg.DispCount = 1U;
        InitCfg.DefaultDataCfg.DispId[0] = 0U;
        InitCfg.DefaultDataCfg.ViewId[0] = 0U;
        /* Register callback */
        InitCfg.EventCb = SvcAnimTask_EventCb;
        InitCfg.OsdChangeCb = SvcAnimTask_OsdChangeCb;
        InitCfg.OsdDoneCb = SvcAnimTask_OsdDoneCb;
        Rval = SvcAnim_Init(&InitCfg);
        if (Rval != SVC_OK) {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        } else {
            g_InitFlag = 1U;
            AmbaPrint_PrintUInt5("SvcAnim_Init() Success! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[ERROR] SvcBuffer_Request() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
    }


    {//Register OSD

        SvcAnimOsd_MutexCreate();
        SvcGui_Register(VOUT_IDX_B, SVC_ANIM_OSD_GUI_LEVEL, "Anim", SvcExtOsdDraw, SvcExtOsdUpdate);
        SvcGui_Register(VOUT_IDX_B, SVC_ANIM_BLACK_GUI_LEVEL, "Anim_base", SvcBaseOsdDraw, SvcBaseOsdUpdate);
    }

    return Rval;
}

UINT32 SvcAnimTask_GetBufSize(UINT32 *QuaryBufSizeNC, UINT32 *QuaryBufSizeCA)
{
    UINT32 Rval = SVC_OK;
    UINT32 BufSize = 0U;
    ULONG BufBase = 0U;
    AMBA_CALIB_DATA_INIT_CFG_s CalibDataInitCfg;
    /* Svc anim module init */
    SVC_ANIM_GET_BUF_CFG_s GetBufCfg;
    UINT32 BufferSize;
    UINT32 CacheWorkBufSize, NonCacheWorkBufSize;
    UINT32 i;

    CalibDataInitCfg.MaxHdlr = 1U;
    CalibDataInitCfg.StorageId = AMBA_CALIB_DATA_STORAGE_SD;
    CalibDataInitCfg.PartId = (UINT32)'C';
    CalibDataInitCfg.MaxGroup = 3U;
    CalibDataInitCfg.MaxWarp = 5U;
    CalibDataInitCfg.MaxVig = 0U;
    CalibDataInitCfg.MaxBlend = 4U;
    CalibDataInitCfg.MaxOsd = 1U;
    CalibDataInitCfg.MaxRaw = 1U;
    CalibDataInitCfg.MaxUdta = 1U;
    CalibDataInitCfg.UdtaSize[0] = sizeof(SVC_ANIM_HEADER_s);
    Rval = AmbaCalibData_GetInitBufferSize(&CalibDataInitCfg, &BufferSize);
    if (Rval == SVC_OK) {
        CalibDataInitCfg.BufferSize = BufferSize;
        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_LV_CAL, &BufBase, &BufSize); //TODO
        if (Rval == SVC_OK) {
            AmbaMisra_TypeCast(&CalibDataInitCfg.Buffer, &BufBase);
            Rval = AmbaCalibData_Init(&CalibDataInitCfg);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("[ERROR] SvcCalibData_Init() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }

            GetBufCfg.VoutCount = 1U;
            for (i = 0U; i < GetBufCfg.VoutCount; i++) {
                GetBufCfg.OsdBufPitch[i] = SVC_OSD_WIDTH; //TODO
                GetBufCfg.OsdBufHeight[i] = SVC_OSD_HEIGH;//TODO
            }
            Rval = SvcAnim_GetWorkBufSize(&GetBufCfg, &CacheWorkBufSize, &NonCacheWorkBufSize);
            if (Rval == SVC_OK) {
                *QuaryBufSizeNC = NonCacheWorkBufSize;
                *QuaryBufSizeCA = CacheWorkBufSize;
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcBuffer_Request() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

