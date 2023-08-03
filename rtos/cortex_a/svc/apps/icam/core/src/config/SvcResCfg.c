/**
 *  @file SvcResCfg.c
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
 *  @details svc resolution config
 *
 */

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaShell.h"
#include "AmbaDef.h"
#include "AmbaUtility.h"
#include "AmbaVIN_Priv.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcVinSrc.h"
#include "SvcLiveview.h"
#include "SvcResCfg.h"
#include "AmbaKAL.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaFPD.h"
#include "AmbaUtility.h"
#include "SvcWrap.h"

#include "SvcVinTree.h"
#include "SvcDSP.h"
#include "SvcStill.h"
#include "SvcStillCap.h"

#define SVC_LOG_RESOL_CFG "RESOL_CFG"

static UINT32 SvcResolCfgDebugEnable = 0U;

static void ResolCfg_Dbg(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcResolCfgDebugEnable > 0U) {
        SvcLog_OK(pModule, pFormat, Arg1, Arg2);
    }
}

static void ResolCfg_Err(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcResolCfgDebugEnable > 0U) {
        SvcLog_NG(pModule, pFormat, Arg1, Arg2);
    }
}

static SVC_RES_CFG_s SvcResCfg GNU_SECTION_NOZEROINIT;
static SVC_RES_CFG_MAX_s SvcResCfgMax GNU_SECTION_NOZEROINIT;

/**
 * Init resolution config module
 * @param [in] pResCfg The user defined resolution confiugration to init SvcResCfg module
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_Init(const SVC_RES_CFG_s* pResCfg)
{
    UINT32 Rval;

    if (pResCfg == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_Init() failed. Input ResCfg should not be NULL", 0U, 0U);
        Rval = SVC_NG;
    } else {
        Rval = AmbaWrap_memcpy(&SvcResCfg, pResCfg, sizeof(SVC_RES_CFG_s));
        if (SVC_OK == Rval) {
            /* Check vin tree is empty or not */
            if (0U == SvcWrap_strlen(SvcResCfg.VinTree)) {
                Rval = SVC_NG;
            } else {
                Rval = SvcVinTree_Init();

                if (SVC_OK == Rval) {
                    Rval = SvcVinTree_Create(SvcResCfg.VinTree);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcVinTree_Create() failed !!", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcVinTree_Init() failed !!", 0U, 0U);
                }
            }

            /* Replace original DispStrm with DispAlt */
            if (SvcResCfg.DispAltNum > 0U) {
                UINT32 AltIdx = SvcResCfg.DispAltIdx;

                SvcResCfg.DispNum  = SvcResCfg.DispAlt[AltIdx].DispNum;
                SvcResCfg.DispBits = SvcResCfg.DispAlt[AltIdx].DispBits;
                Rval = AmbaWrap_memcpy(SvcResCfg.DispStrm, SvcResCfg.DispAlt[AltIdx].DispStrm, sizeof(SVC_DISP_STRM_s) * AMBA_DSP_MAX_VOUT_NUM);
                if (SVC_OK == Rval) {
                    SvcLog_OK(SVC_LOG_RESOL_CFG, "SvcResCfg.Disp setting is switched to AltIdx (%d)", AltIdx, 0U);
                } else {
                    SvcLog_NG(SVC_LOG_RESOL_CFG, "Copy AltDispStrm failed with %d", Rval, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_RESOL_CFG, "Copy SvcResCfg failed with %d", Rval, 0U);
        }
    }

    return Rval;
}

/**
 * Re-init resolution config
 * @param [in] pResCfg The user defined resolution confiugration to init SvcResCfg module
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_Config(const SVC_RES_CFG_s* pResCfg)
{
    UINT32 Rval;

    if (pResCfg == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_Config() failed. Input ResCfg should not be NULL", 0U, 0U);
        Rval = SVC_NG;
    } else {
        UINT32 CmpResult = 0U;
        Rval = AmbaWrap_memcmp(&SvcResCfg, pResCfg, sizeof(SVC_RES_CFG_s), &CmpResult);
        if (SVC_OK == Rval) {
            if (CmpResult == 0U) {
                SvcLog_OK(SVC_LOG_RESOL_CFG, "SvcResCfg_Config() skip. Input ResCfg is the same with the one when init. No need to configure again", 0U, 0U);
                Rval = SVC_OK;
            } else {
                Rval = SvcResCfg_Init(pResCfg);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_Config() failed. Re-init failed", 0U, 0U);
                }
            }
        }
    }

    return Rval;
}

/**
 * Calculate the maximum resolution config
 * @param [in] pResCfgArr The user defined resolution confiugration array for calculation
 * @param [in] Num The input array number
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_ConfigMax(const SVC_RES_CFG_s *pResCfgArr, UINT32 Num)
{
    UINT32 Rval = SVC_OK;
    UINT32 VinID = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM] = {0U}, VinNum = 0U, VinSrc = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0U}, FovNum = 0U;
    UINT32 i, j, k, h;

    if (SVC_OK != AmbaWrap_memset(&SvcResCfgMax, 0, sizeof(SVC_RES_CFG_MAX_s))) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_ConfigMax() failed. AmbaWrap_memset failed", 0U, 0U);
    }

    if (pResCfgArr == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_ConfigMax() failed. Input ResCfg should not be NULL", 0U, 0U);
        Rval = SVC_NG;
    } else {
        SvcResCfgMax.StillEn = 0;
        SvcResCfgMax.StillCeEn = 0;
        SvcResCfgMax.StillRawUncomp = 0;
        for (i = 0U; i < Num; i++) {
            /* To use vin tree */
            Rval = SvcResCfg_Init(&pResCfgArr[i]);
            if (Rval != SVC_OK) {
                break;
            }

            Rval = SvcResCfg_GetVinIDs(VinIDs, &VinNum);

            SvcResCfgMax.StillRawCapBufSize = 0;
            /* Compare Vin Resource */
            for (j = 0U; j < VinNum; j++) {
                VinID = VinIDs[j];

                Rval = SvcResCfg_GetVinSrc(VinID, &VinSrc);

                SvcResCfgMax.VinBits |= (UINT32) 1U << VinID;

                SvcResCfgMax.VinSrc[VinID] = VinSrc;

                if (SvcResCfgMax.VinWin[VinID].Width < pResCfgArr[i].VinCfg[VinID].CapWin.Width) {
                    SvcResCfgMax.VinWin[VinID].Width = pResCfgArr[i].VinCfg[VinID].CapWin.Width;
                }
                if (SvcResCfgMax.VinWin[VinID].Height < pResCfgArr[i].VinCfg[VinID].CapWin.Height) {
                    SvcResCfgMax.VinWin[VinID].Height = pResCfgArr[i].VinCfg[VinID].CapWin.Height;
                }

                if (SvcResCfgMax.VinTimeOut[VinID].Boot < pResCfgArr[i].VinCfg[VinID].TimeoutCfg.Boot) {
                    SvcResCfgMax.VinTimeOut[VinID].Boot = pResCfgArr[i].VinCfg[VinID].TimeoutCfg.Boot;
                }

                if (SvcResCfgMax.VinTimeOut[VinID].Liveview < pResCfgArr[i].VinCfg[VinID].TimeoutCfg.Liveview) {
                    SvcResCfgMax.VinTimeOut[VinID].Liveview = pResCfgArr[i].VinCfg[VinID].TimeoutCfg.Liveview;
                }

                if (SvcResCfgMax.MaxVinTimeout < pResCfgArr[i].VinCfg[VinID].TimeoutCfg.Liveview) {
                    SvcResCfgMax.MaxVinTimeout = pResCfgArr[i].VinCfg[VinID].TimeoutCfg.Liveview;
                }

                Rval = SvcResCfg_GetFovIdxsInVinID(VinID, FovIdxs, &FovNum);

                if (SVC_OK == Rval) {
                    SvcResCfgMax.VinOutputNum[VinID] = FovNum;
                }

                SvcResCfgMax.StillCeEn |= pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.CeNeeded;
                if (pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Compressed == 0U) {
                    SvcResCfgMax.StillRawUncomp = 1;
                }

                if (pResCfgArr[i].StillCfg.EnableStillRaw == 1U) {
                    SVC_DSP_FRAME_s Setup;
                    UINT32 Size;
                    Setup.FrameNum = (UINT32)pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.FrameNum;
                    Setup.FrameType = SVC_FRAME_TYPE_RAW;
                    Setup.FrameW = (UINT32)pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Width;
                    Setup.FrameH = (UINT32)pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Height;
                    Setup.RawCmpr = (UINT16)pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Compressed;
                    Setup.CeNeeded = pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.CeNeeded;
                    Rval = SvcDSP_QueryFrameBufSize(&Setup, &Size);
                    if (Rval == OK) {
                        SvcResCfgMax.StillRawCapBufSize += Size;
                        if (SvcResCfgMax.StillRawW < pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Width) {
                            SvcResCfgMax.StillRawW = pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Width;
                        }
                        if (SvcResCfgMax.StillRawH < pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Height) {
                            SvcResCfgMax.StillRawH = pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.Height;
                        }
                        if (SvcResCfgMax.StillWin.Width < pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.EffectW) {
                            SvcResCfgMax.StillWin.Width = pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.EffectW;
                        }
                        if (SvcResCfgMax.StillWin.Height < pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.EffectH) {
                            SvcResCfgMax.StillWin.Height = pResCfgArr[i].StillCfg.RawCfg[VinID].MaxRaw.EffectH;
                        }
                    }

                    if (pResCfgArr[i].StillCfg.Qview.Enable == 1U) {
                        for (k = 0u; k < pResCfgArr[i].StillCfg.Qview.NumQview; k++) {
                            if (SvcResCfgMax.StillQviewWin.Width < pResCfgArr[i].StillCfg.Qview.Win[k].Width) {
                                SvcResCfgMax.StillQviewWin.Width = (UINT16)pResCfgArr[i].StillCfg.Qview.Win[k].Width;
                            }
                            if (SvcResCfgMax.StillQviewWin.Height < pResCfgArr[i].StillCfg.Qview.Win[k].Height) {
                                SvcResCfgMax.StillQviewWin.Height = (UINT16)pResCfgArr[i].StillCfg.Qview.Win[k].Height;
                            }
                        }
                    }
                }
            }


            if (SvcResCfgMax.MaxVinTimeout == 0U) {
                SvcResCfgMax.MaxVinTimeout = DEFAULT_MAX_VIN_TIMEOUT;
            }

            /* Compare Fov Resource */
            if (SvcResCfgMax.FovNum < pResCfgArr[i].FovNum) {
                SvcResCfgMax.FovNum = pResCfgArr[i].FovNum;
            }
            for (j = 0U; j < pResCfgArr[i].FovNum; j++) {
                if (SvcResCfgMax.FovWin[j].Width < pResCfgArr[i].FovCfg[j].MainWin.Width) {
                    SvcResCfgMax.FovWin[j].Width = pResCfgArr[i].FovCfg[j].MainWin.Width;
                }
                if (SvcResCfgMax.FovWin[j].Height < pResCfgArr[i].FovCfg[j].MainWin.Height) {
                    SvcResCfgMax.FovWin[j].Height = pResCfgArr[i].FovCfg[j].MainWin.Height;
                }
#if defined(CONFIG_ICAM_PYRAMID_BUF_SUPPORTED)
                if (SvcResCfgMax.FovPyramidEnable[j] == 0U) {
                    if (pResCfgArr[i].FovCfg[j].PyramidBits != 0U) {
                        SvcResCfgMax.FovPyramidEnable[j] = 1U;
                    }
                }
                if (SvcResCfgMax.FovPyramidExScaleEnable[j] == 0U) {
                    if ((pResCfgArr[i].FovCfg[j].PyramidFlag & SVC_RES_PYRAMID_ENABLE_EX_SCALE) > 0U) {
                        SvcResCfgMax.FovPyramidExScaleEnable[j] = 1U;
                    }
                }
#endif
            }

            SvcResCfgMax.StillYuvCapBufSize = 0;

            /* Compare Disp Resource */
            if (pResCfgArr[i].DispAltNum == 0U) {
                if (SvcResCfgMax.DispNum < pResCfgArr[i].DispNum) {
                    SvcResCfgMax.DispNum = pResCfgArr[i].DispNum;
                }
                for (j = 0U; j < pResCfgArr[i].DispNum; j++) {
                    const SVC_DISP_STRM_s *pDispStrm;
                    UINT32 VoutID, MaxDispFovNum = 0U;
                    pDispStrm = &pResCfgArr[i].DispStrm[j];
                    VoutID = pDispStrm->VoutID;

                    if (SvcResCfgMax.DispMaxWin[VoutID].Width < pDispStrm->StrmCfg.MaxWin.Width) {
                        SvcResCfgMax.DispMaxWin[VoutID].Width = pDispStrm->StrmCfg.MaxWin.Width;
                    }
                    if (SvcResCfgMax.DispMaxWin[VoutID].Height < pDispStrm->StrmCfg.MaxWin.Height) {
                        SvcResCfgMax.DispMaxWin[VoutID].Height = pDispStrm->StrmCfg.MaxWin.Height;
                    }

                    if (SvcResCfgMax.DispWin[VoutID].Width < pDispStrm->StrmCfg.Win.Width) {
                        SvcResCfgMax.DispWin[VoutID].Width = pDispStrm->StrmCfg.Win.Width;
                    }
                    if (SvcResCfgMax.DispWin[VoutID].Height < pDispStrm->StrmCfg.Win.Height) {
                        SvcResCfgMax.DispWin[VoutID].Height = pDispStrm->StrmCfg.Win.Height;
                    }

                    for (k = 0U; k < pDispStrm->StrmCfg.NumChan; k++) {
                        UINT32 FovIdx = pDispStrm->StrmCfg.ChanCfg[k].FovId;
                        UINT16 Width, Height;

                        Width  = pDispStrm->StrmCfg.ChanCfg[k].DstWin.Width;
                        Height = pDispStrm->StrmCfg.ChanCfg[k].DstWin.Height;

                        SvcResCfgMax.DispChanBits[VoutID] |= ((UINT32) 1U) << FovIdx;
                        /* Compare and update if there's larger dimension */
                        if (SvcResCfgMax.DispChan[VoutID][FovIdx].Width < Width) {
                            SvcResCfgMax.DispChan[VoutID][FovIdx].Width = Width;
                        }
                        if (SvcResCfgMax.DispChan[VoutID][FovIdx].Height < Height) {
                            SvcResCfgMax.DispChan[VoutID][FovIdx].Height = Height;
                        }
                        /* To support rotation */
                        if ((pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_90) ||
                            (pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_90_VERT_FLIP) ||
                            (pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_270) ||
                            (pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_270_VERT_FLIP)) {

                            /* Update to rectangle dimension to support rotation */
                            SvcResCfgMax.DispChan[VoutID][FovIdx].Width  = GetMaxValU16(SvcResCfgMax.DispChan[VoutID][FovIdx].Width, SvcResCfgMax.DispChan[VoutID][FovIdx].Height);
                            SvcResCfgMax.DispChan[VoutID][FovIdx].Height = GetMaxValU16(SvcResCfgMax.DispChan[VoutID][FovIdx].Width, SvcResCfgMax.DispChan[VoutID][FovIdx].Height);
                        }

                        if (SvcResCfgMax.StillWin.Width < Width) {
                            SvcResCfgMax.StillWin.Width = Width;
                        }
                        if (SvcResCfgMax.StillWin.Height < Height) {
                            SvcResCfgMax.StillWin.Height = Height;
                        }
                        {
                            SVC_DSP_FRAME_s Setup;
                            UINT32 Size;
                            Setup.FrameNum = 1;
                            Setup.FrameType = SVC_FRAME_TYPE_YUV;
                            Setup.FrameW = Width;
                            Setup.FrameH = Height;
                            if (SVC_OK == SvcDSP_QueryFrameBufSize(&Setup, &Size)) {
                                SvcResCfgMax.StillYuvCapBufSize += Size;
                            }
                        }
                    }

                    for (k = 0U; k < AMBA_DSP_MAX_VIEWZONE_NUM; k++) {
                        if ((SvcResCfgMax.DispChanBits[VoutID] & ((UINT32)1U << k)) != 0U) {
                            MaxDispFovNum++;
                        }
                    }
                    SvcResCfgMax.DispChanNum[VoutID] = MaxDispFovNum;
                    SvcResCfgMax.DispFormat[VoutID] = 0U;    /* Don't care */
                }
            } else {
                for (h = 0U; h < pResCfgArr[i].DispAltNum; h++) {
                    if (SvcResCfgMax.DispNum < pResCfgArr[i].DispAlt[h].DispNum) {
                        SvcResCfgMax.DispNum = pResCfgArr[i].DispAlt[h].DispNum;
                    }
                    for (j = 0U; j < pResCfgArr[i].DispAlt[h].DispNum; j++) {
                        const SVC_DISP_STRM_s *pDispStrm;
                        UINT32 VoutID,  MaxDispFovNum = 0U;
                        pDispStrm = &pResCfgArr[i].DispAlt[h].DispStrm[j];
                        VoutID = pDispStrm->VoutID;

                        SvcResCfgMax.DispMaxWin[VoutID].Width  = pDispStrm->StrmCfg.MaxWin.Width;
                        SvcResCfgMax.DispMaxWin[VoutID].Height = pDispStrm->StrmCfg.MaxWin.Height;

                        SvcResCfgMax.DispWin[VoutID].Width  = pDispStrm->StrmCfg.Win.Width;
                        SvcResCfgMax.DispWin[VoutID].Height = pDispStrm->StrmCfg.Win.Height;

                        for (k = 0U; k < pDispStrm->StrmCfg.NumChan; k++) {
                            UINT32 FovIdx = pDispStrm->StrmCfg.ChanCfg[k].FovId;
                            UINT16 Width, Height;

                            if ((pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_90) ||
                                (pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_90_VERT_FLIP) ||
                                (pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_270) ||
                                (pDispStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_270_VERT_FLIP)) {

                                Width  = pDispStrm->StrmCfg.ChanCfg[k].DstWin.Height;
                                Height = pDispStrm->StrmCfg.ChanCfg[k].DstWin.Width;
                            } else {
                                Width  = pDispStrm->StrmCfg.ChanCfg[k].DstWin.Width;
                                Height = pDispStrm->StrmCfg.ChanCfg[k].DstWin.Height;
                            }

                            SvcResCfgMax.DispChanBits[VoutID] |= ((UINT32) 1U) << FovIdx;
                            if (SvcResCfgMax.DispChan[VoutID][FovIdx].Width < Width) {
                                SvcResCfgMax.DispChan[VoutID][FovIdx].Width = Width;
                            }
                            if (SvcResCfgMax.DispChan[VoutID][FovIdx].Height < Height) {
                                SvcResCfgMax.DispChan[VoutID][FovIdx].Height = Height;
                            }
                            if (SvcResCfgMax.StillWin.Width < Width) {
                                SvcResCfgMax.StillWin.Width = Width;
                            }
                            if (SvcResCfgMax.StillWin.Height < Height) {
                                SvcResCfgMax.StillWin.Height = Height;
                            }
                            {
                                SVC_DSP_FRAME_s Setup;
                                UINT32 Size;
                                Setup.FrameNum = 1;
                                Setup.FrameType = SVC_FRAME_TYPE_YUV;
                                Setup.FrameW = Width;
                                Setup.FrameH = Height;
                                if (SVC_OK == SvcDSP_QueryFrameBufSize(&Setup, &Size)) {
                                    SvcResCfgMax.StillYuvCapBufSize += Size;
                                }
                            }
                        }

                        for (k = 0U; k < AMBA_DSP_MAX_VIEWZONE_NUM; k++) {
                            if ((SvcResCfgMax.DispChanBits[VoutID] & ((UINT32)1U << k)) != 0U) {
                                MaxDispFovNum++;
                            }
                        }
                        SvcResCfgMax.DispChanNum[VoutID] = MaxDispFovNum;
                        SvcResCfgMax.DispFormat[VoutID] = 0U;    /* Don't care */
                    }
                }
            }

            /* Compare Rec Resource */
            if (SvcResCfgMax.RecNum < pResCfgArr[i].RecNum) {
                SvcResCfgMax.RecNum = pResCfgArr[i].RecNum;
            }
            for (j = 0U; j < pResCfgArr[i].RecNum; j++) {
                UINT32 MaxRecFovNum = 0U;
                const SVC_REC_STRM_s *pRecStrm;
                pRecStrm = &pResCfgArr[i].RecStrm[j];

                if (SvcResCfgMax.RecMaxWin[j].Width < pRecStrm->StrmCfg.MaxWin.Width) {
                    SvcResCfgMax.RecMaxWin[j].Width = pRecStrm->StrmCfg.MaxWin.Width;
                }
                if (SvcResCfgMax.RecMaxWin[j].Height < pRecStrm->StrmCfg.MaxWin.Height) {
                    SvcResCfgMax.RecMaxWin[j].Height = pRecStrm->StrmCfg.MaxWin.Height;
                }

                if (SvcResCfgMax.RecWin[j].Width < pRecStrm->StrmCfg.Win.Width) {
                    SvcResCfgMax.RecWin[j].Width = pRecStrm->StrmCfg.Win.Width;
                }

                if (SvcResCfgMax.RecWin[j].Height < pRecStrm->StrmCfg.Win.Height) {
                    SvcResCfgMax.RecWin[j].Height = pRecStrm->StrmCfg.Win.Height;
                }

                if (SvcResCfgMax.StillWin.Width < pRecStrm->StrmCfg.Win.Width) {
                    SvcResCfgMax.StillWin.Width = pRecStrm->StrmCfg.Win.Width;
                }
                if (SvcResCfgMax.StillWin.Height < pRecStrm->StrmCfg.Win.Height) {
                    SvcResCfgMax.StillWin.Height = pRecStrm->StrmCfg.Win.Height;
                }
                {
                    SVC_DSP_FRAME_s Setup;
                    UINT32 Size;
                    Setup.FrameNum = 1;
                    Setup.FrameType = SVC_FRAME_TYPE_YUV;
                    Setup.FrameW = pRecStrm->StrmCfg.Win.Width;
                    Setup.FrameH = pRecStrm->StrmCfg.Win.Height;
                    if (SVC_OK == SvcDSP_QueryFrameBufSize(&Setup, &Size)) {
                        SvcResCfgMax.StillYuvCapBufSize += Size;
                    }
                }

                if (SvcResCfgMax.RecGopM[j] < pRecStrm->RecSetting.M) {
                    SvcResCfgMax.RecGopM[j] = pRecStrm->RecSetting.M;
                }

                SvcResCfgMax.RecFormat[j] = 0x7U;           /* H264 + H265 + JPEG */
                if (0U < pRecStrm->RecSetting.TimeLapse) {
                    SvcResCfgMax.RecFormat[j] |= 0x80U;     /* Timelapse */
                }

                for (k = 0U; k < pRecStrm->StrmCfg.NumChan; k++) {
                    UINT32 FovIdx = pRecStrm->StrmCfg.ChanCfg[k].FovId;
                    UINT16 Width, Height;

                    if ((pRecStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_90) ||
                        (pRecStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_90_VERT_FLIP) ||
                        (pRecStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_270) ||
                        (pRecStrm->StrmCfg.ChanCfg[k].RotateFlip == AMBA_DSP_ROTATE_270_VERT_FLIP)) {

                        Width  = pRecStrm->StrmCfg.ChanCfg[k].DstWin.Height;
                        Height = pRecStrm->StrmCfg.ChanCfg[k].DstWin.Width;
                    } else {
                        Width  = pRecStrm->StrmCfg.ChanCfg[k].DstWin.Width;
                        Height = pRecStrm->StrmCfg.ChanCfg[k].DstWin.Height;
                    }

                    SvcResCfgMax.RecChanBits[j] |= ((UINT32) 1U) << FovIdx;
                    if (SvcResCfgMax.RecChan[j][FovIdx].Width < Width) {
                        SvcResCfgMax.RecChan[j][FovIdx].Width = Width;
                    }
                    if (SvcResCfgMax.RecChan[j][FovIdx].Height < Height) {
                        SvcResCfgMax.RecChan[j][FovIdx].Height = Height;
                    }
                }

                for (k = 0U; k < AMBA_DSP_MAX_VIEWZONE_NUM; k++) {
                    if ((SvcResCfgMax.RecChanBits[j] & ((UINT32)1U << k)) != 0U) {
                        MaxRecFovNum++;
                    }
                }
                SvcResCfgMax.RecChanNum[j] = MaxRecFovNum;
            }

            if (SvcResCfgMax.RecNum > 0U) {
                SvcResCfgMax.StillNum = SvcResCfgMax.RecNum;
                SvcResCfgMax.StillFormat = 0x2U;    /* JPEG */
            }
            if (pResCfgArr[i].StillCfg.EnableStill == 1U) {
                SvcResCfgMax.StillEn = 1;
            }
        }
    }
    return Rval;
}

/**
 * get resolution config
 * return Configuration Address
 */
SVC_RES_CFG_s* SvcResCfg_Get(void)
{
    return &SvcResCfg;
}

/**
 * get max resolution config
 * return Configuration Address
 */
SVC_RES_CFG_MAX_s* SvcResCfg_GetMax(void)
{
    return &SvcResCfgMax;
}

UINT32 SvcResCfg_GetVinVirtualChan(UINT32 VinID, UINT32 *pIsVirtual, UINT32 *pFromVinID)
{
#if defined(CONFIG_SOC_CV2FS)
    switch (VinID) {
        case 2U:
            *pIsVirtual = SVC_RES_VIN_VIRTUAL_1;
            *pFromVinID = 0U;
            break;
        case 3U:
            *pIsVirtual = SVC_RES_VIN_VIRTUAL_2;
            *pFromVinID = 0U;
            break;
        case 4U:
            *pIsVirtual = SVC_RES_VIN_VIRTUAL_3;
            *pFromVinID = 0U;
            break;
        default:
            *pIsVirtual = SVC_RES_VIN_REAL;
            *pFromVinID = VinID;
            break;
    }
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                                            /* VIN0,          VIN1,                  VIN2,                  VIN3 */
    const UINT32 VinVirt[AMBA_DSP_MAX_VIN_NUM] = {SVC_RES_VIN_REAL, SVC_RES_VIN_VIRTUAL_1, SVC_RES_VIN_VIRTUAL_2, SVC_RES_VIN_VIRTUAL_3,
                                            /* VIN4,          VIN5,                  VIN6,                  VIN7 */
                                            SVC_RES_VIN_REAL, SVC_RES_VIN_VIRTUAL_1, SVC_RES_VIN_VIRTUAL_2, SVC_RES_VIN_VIRTUAL_3,
                                            /* VIN8,          VIN9,                  VIN10 */
                                            SVC_RES_VIN_REAL, SVC_RES_VIN_VIRTUAL_1, SVC_RES_VIN_VIRTUAL_2,
                                            /* VIN11,         VIN12,                 VIN13 */
                                            SVC_RES_VIN_REAL, SVC_RES_VIN_VIRTUAL_1, SVC_RES_VIN_VIRTUAL_2};

                                            /* VIN0,          VIN1,                  VIN2,                  VIN3 */
    const UINT32 VinFrom[AMBA_DSP_MAX_VIN_NUM] = {AMBA_VIN_CHANNEL0, AMBA_VIN_CHANNEL0, AMBA_VIN_CHANNEL0, AMBA_VIN_CHANNEL0,
                                            /* VIN4,          VIN5,                  VIN6,                  VIN7 */
                                            AMBA_VIN_CHANNEL4, AMBA_VIN_CHANNEL4, AMBA_VIN_CHANNEL4, AMBA_VIN_CHANNEL4,
                                            /* VIN8,          VIN9,                  VIN10 */
                                            AMBA_VIN_CHANNEL8, AMBA_VIN_CHANNEL8, AMBA_VIN_CHANNEL8,
                                            /* VIN11,         VIN12,                 VIN13 */
                                            AMBA_VIN_CHANNEL11, AMBA_VIN_CHANNEL11, AMBA_VIN_CHANNEL11};
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
        *pIsVirtual = VinVirt[VinID];
        *pFromVinID = VinFrom[VinID];
    }
#else
    *pIsVirtual = 0U;
    *pFromVinID = VinID;
#endif

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "Virtual (%d), FromVinID", *pIsVirtual, *pFromVinID);

    return SVC_OK;
}

/**
 * get vin source
 * @param [in] VinID The desired Vin ID
 * @param [out] pVinSrc The Vin source, maybe 0 SRC_SENSOR, 1 SRC_YUV, 2: SRC_MEM
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetVinSrc(UINT32 VinID, UINT32 *pVinSrc)
{
    UINT32 RetVal = SVC_OK;
    UINT32 SensorIdx, MemIdx;
    UINT32 Find = 0U;
    const SVC_VIN_TREE_NODE_s *pNode, *pSearchNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinSrc() start", 0U, 0U);

    if (pVinSrc == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinSrc() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (SensorIdx = 0U; SensorIdx < SVC_MAX_NUM_SENSOR_PER_VIN; SensorIdx++) {
            pNode = SvcVinTree_SearchSensor(VinID, SensorIdx);
            pSearchNode = pNode;
            /* Find previous node until it is VIN */
            while ((pSearchNode != NULL) && (pSearchNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_VIN)) {
                pSearchNode = pSearchNode->pPrevNode;
            }

            if (pSearchNode != NULL) {
                if (pSearchNode->Content.Value.Vin.VinID == VinID) {
                    if (pNode->Content.Value.Sensor.SensorOrYuv == SVC_VIN_TREE_NODE_VALUE_SENSOR) {
                        *pVinSrc = SVC_VIN_SRC_SENSOR;
                        Find = 1U;
                    } else if (pNode->Content.Value.Sensor.SensorOrYuv == SVC_VIN_TREE_NODE_VALUE_YUV) {
                        *pVinSrc = SVC_VIN_SRC_YUV;
                        Find = 1U;
                    } else {
                        /* Should not be here */
                        Find = 0U;
                    }
                    break;
                }
            } else {
                ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcVinTree_SearchSensor() failed. VinTree cannot find Sensor Idx", 0U, 0U);
            }
        }

        /* Cannot find VinSrc as sensor or YUV input */
        if (Find == 0U) {
            for (MemIdx = 0U; MemIdx < SVC_MAX_NUM_MEM_INPUT; MemIdx++) {
                pNode = SvcVinTree_SearchMem(MemIdx);
                pSearchNode = pNode;
                /* Find previous node until it is VIN */
                while ((pSearchNode != NULL) && (pSearchNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_VIN)) {
                    pSearchNode = pSearchNode->pPrevNode;
                }

                if (pSearchNode != NULL) {
                    if (pSearchNode->Content.Value.Vin.VinID == VinID) {
                        if (pNode->Content.Value.Mem.Type == SVC_VIN_TREE_NODE_VALUE_MEM_RAW) {
                            *pVinSrc = SVC_VIN_SRC_MEM;
                        } else if (pNode->Content.Value.Mem.Type == SVC_VIN_TREE_NODE_VALUE_MEM_422) {
                            *pVinSrc = SVC_VIN_SRC_MEM_YUV422;
                        } else if (pNode->Content.Value.Mem.Type == SVC_VIN_TREE_NODE_VALUE_MEM_420) {
                            *pVinSrc = SVC_VIN_SRC_MEM_YUV420;
                        } else {
                            /* Should not be here */
                            SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinSrc() err. Should not be here. Type = %d, Value = %d", pSearchNode->Content.Type, pSearchNode->Content.Value.Mem.Type);
                        }
                        break;
                    }
                } else {
                    ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcVinTree_SearchMem() failed. VinTree cannot find mem idx", 0U, 0U);
                }
            }
        }
    }

    return RetVal;
}

/**
 * get fov source
 * @param [in] FovIdx The desired Fov ID
 * @param [out] pFovSrc The Fov source, maybe 0 SRC_SENSOR, 1 SRC_YUV, 2: SRC_MEM
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetFovSrc(UINT32 FovIdx, UINT32 *pFovSrc)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pSearchNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovSrc() start", 0U, 0U);

    AmbaMisra_TouchUnused(&RetVal);

    if (RetVal == SVC_OK) {
        UINT32 Find = 0U;

        pSearchNode = SvcVinTree_SearchFov(FovIdx);
        if (pSearchNode == NULL) {
            ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcVinTree_SearchFov() failed. VinTree cannot find Fov Idx %d", FovIdx, 0U);
        }

        while ((pSearchNode != NULL) && (Find == 0U)) {
            if (pSearchNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR) {
                if (pSearchNode->Content.Value.Sensor.SensorOrYuv == SVC_VIN_TREE_NODE_VALUE_SENSOR) {
                    *pFovSrc = SVC_VIN_SRC_SENSOR;
                } else if (pSearchNode->Content.Value.Sensor.SensorOrYuv == SVC_VIN_TREE_NODE_VALUE_YUV) {
                    *pFovSrc = SVC_VIN_SRC_YUV;
                } else {
                    /* Should not be here */
                    SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovSrc() err. Should not be here. Type = %d, Value = %d", pSearchNode->Content.Type, pSearchNode->Content.Value.Sensor.SensorOrYuv);
                }
                Find = 1U;
            } else if (pSearchNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_MEM) {
                if (pSearchNode->Content.Value.Mem.Type == SVC_VIN_TREE_NODE_VALUE_MEM_RAW) {
                    *pFovSrc = SVC_VIN_SRC_MEM;
                } else if (pSearchNode->Content.Value.Mem.Type == SVC_VIN_TREE_NODE_VALUE_MEM_422) {
                    *pFovSrc = SVC_VIN_SRC_MEM_YUV422;
                } else if (pSearchNode->Content.Value.Mem.Type == SVC_VIN_TREE_NODE_VALUE_MEM_420) {
                    *pFovSrc = SVC_VIN_SRC_MEM_YUV420;
                } else {
                    /* Should not be here */
                    SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovSrc() err. Should not be here. Type = %d, Value = %d", pSearchNode->Content.Type, pSearchNode->Content.Value.Mem.Type);
                }
                Find = 1U;
            } else if (pSearchNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_DEC) {
                *pFovSrc = SVC_VIN_SRC_MEM_DEC;
                Find = 1U;
            } else {
                pSearchNode = pSearchNode->pPrevNode;
            }
        }

        if (Find == 0U) {
            ResolCfg_Err(SVC_LOG_RESOL_CFG, "VinTree cannot find Fov Idx %d Src", FovIdx, 0U);
        }
    }

    return RetVal;
}

/**
 * Get the VinIDs now can be used
 * @param [out] pVinID An array containing VinIDs
 * @param [out] pVinNum The array length of pVinID
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetVinIDs(UINT32 *pVinID, UINT32 *pVinNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 VinIdx = 0, i = 0;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinID() start", 0U, 0U);

    if ((pVinID == NULL) || (pVinNum == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinID() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (VinIdx = 0; VinIdx < AMBA_DSP_MAX_VIN_NUM; VinIdx++) {
            pNode = SvcVinTree_SearchVin(VinIdx);
            if (pNode != NULL) {
                pVinID[i] = pNode->Content.Value.Vin.VinID;
                i = i + 1U;
            }
        }

        *pVinNum = i;

        ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinID() done VinNum (%d)", *pVinNum, 0U);
    }

    return RetVal;
}

/**
 * Get the VinID of certain fov idx
 * @param [in] FovIdx The desired fov idx
 * @param [out] pVinID The VinID with this fov index
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetVinIDOfFovIdx(UINT32 FovIdx, UINT32 *pVinID)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinIDOfFovIdx(%d) start", FovIdx, 0U);

    if (pVinID == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinIDOfFovIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        pNode = SvcVinTree_SearchFov(FovIdx);
        /* Find previous node until it is VIN */
        while ((pNode != NULL) && (pNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_VIN)) {
            pNode = pNode->pPrevNode;
        }

        if (pNode != NULL) {
            *pVinID = pNode->Content.Value.Vin.VinID;
        } else {
            ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinIDOfFovIdx(%d) failed. VinTree cannot find VinID", FovIdx, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
 * Get the VinID of certain fov idx
 * @param [in] FovIdx The desired fov idx
 * @param [out] pDecID The VinID with this fov index
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetDecIDOfFovIdx(UINT32 FovIdx, UINT32 *pDecID)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetDecIDOfFovIdx(%d) start", FovIdx, 0U);

    if (pDecID == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetDecIDOfFovIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        pNode = SvcVinTree_SearchFov(FovIdx);
        /* Find previous node until it is VIN */
        while ((pNode != NULL) && (pNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_DEC)) {
            pNode = pNode->pPrevNode;
        }

        if (pNode != NULL) {
            *pDecID = pNode->Content.Value.Dec.DecID;
        } else {
            ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcResCfg_GetDecIDOfFovIdx(%d) failed. VinTree cannot find VinID", FovIdx, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
 * Get the SensorID of certain sensor idx
 * @param [in] VinID The vin ID
 * @param [in] SensorIdx The sensor idx
 * @param [out] pSensorID The sensor ID based on input sensorIdx
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSensorIDOfSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pSensorID)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIDOfSensorIdx() start", 0U, 0U);

    if (pSensorID == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIDOfSensorIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        pNode = SvcVinTree_SearchSensor(VinID, SensorIdx);

        if (pNode != NULL) {
            *pSensorID = pNode->Content.Value.Sensor.SensorID;
        } else {
            SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIDOfSensorIdx() failed. VinTree cannot find SensorID", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
 * Get the SensorID of certain fov idx
 * @param [in] FovIdx The fov idx
 * @param [out] pVinID The VinID based on input fov idx
 * @param [out] pSensorID The SensorID based on input fov idx
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSensorIDOfFovIdx(UINT32 FovIdx, UINT32 *pVinID, UINT32 *pSensorID)
{
    UINT32 RetVal;

    AmbaMisra_TouchUnused(&FovIdx);
    AmbaMisra_TouchUnused(pVinID);
    AmbaMisra_TouchUnused(pSensorID);

    SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIDOfFovIdx() failed. Not support yet", 0U, 0U);
    RetVal = SVC_NG;

    return RetVal;
}

/**
 * Get the Sensor indices of certain fov idx
 * @param [in] FovIdx The fov idx
 * @param [out] pVinID The VinID contains the fov idx
 * @param [out] pSensorIdx The sensor idx based on input fov idx
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSensorIdxOfFovIdx(UINT32 FovIdx, UINT32 *pVinID, UINT32 *pSensorIdx)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIdxOfFovIdx() start", 0U, 0U);

    if (pSensorIdx == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIdxOfFovIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        pNode = SvcVinTree_SearchFov(FovIdx);
        if (pNode == NULL) {
            SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIdxOfFovIdx() failed. VinTree cannot find Fov Idx %u", FovIdx, 0U);
            RetVal = SVC_NG;
        } else {
            /* Find previous node until it is sensor */
            while (pNode != NULL) {
                if (pNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_VIN) {
                    *pVinID = pNode->Content.Value.Vin.VinID;
                } else if (pNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR) {
                    *pSensorIdx = pNode->Content.Value.Sensor.SensorIdx;
                } else {
                    /* Do nothging */
                }
                pNode = pNode->pPrevNode;
            }
        }
    }

    return RetVal;
}

/**
 * Get the SensorID of certain VinID
 * @param [in] VinID The Vin ID
 * @param [out] pSensorID The Sensor ID based on input Vin ID
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSensorIDInVinID(UINT32 VinID, UINT32 *pSensorID)
{
    UINT32 RetVal = SVC_OK;
    UINT32 SensorIdx;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIDInVinID() start", 0U, 0U);

    if (pSensorID == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIDInVinID() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        /* Clean pSensorID to avoid output error */
        *pSensorID = 0U;

        for (SensorIdx = 0U; SensorIdx < SVC_MAX_NUM_SENSOR_PER_VIN; SensorIdx++) {
            pNode = SvcVinTree_SearchSensor(VinID, SensorIdx);
            if (pNode != NULL) {
                *pSensorID |= pNode->Content.Value.Sensor.SensorID;
            } else {
                ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIDInVinID() failed. VinTree cannot find Sensor Idx", 0U, 0U);
            }
        }
    }

    return RetVal;
}

/**
 * Get the sensor index of certain VinID
 * @param [in] VinID The vin ID
 * @param [out] pSensorIdx The array of sensor index based on input Vin ID
 * @param [out] pSensorNum The number of valid value in the array
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSensorIdxsInVinID(UINT32 VinID, UINT32 *pSensorIdx, UINT32 *pSensorNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 SensorIdx, i = 0U;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIdxsInVinID() start", 0U, 0U);

    if ((pSensorIdx == NULL) || (pSensorNum == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIdxsInVinID() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (SensorIdx = 0U; SensorIdx < SVC_MAX_NUM_SENSOR_PER_VIN; SensorIdx++) {
            pNode = SvcVinTree_SearchSensor(VinID, SensorIdx);
            if (pNode != NULL) {
                pSensorIdx[i] = SensorIdx;
                i = i + 1U;
            } else {
                ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSensorIdxsInVinID() failed. VinTree cannot find Sensor Idx", 0U, 0U);
            }
        }
        *pSensorNum = i;
    }

    return RetVal;
}

/**
 * Get the FovIdxs
 * @param [out] pFovIdx The array of fov idx
 * @param [out] pFovNum The number of valid value in the array
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetFovIdxs(UINT32 *pFovIdx, UINT32 *pFovNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FovIdx, i = 0U;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxs() start", 0U, 0U);

    if ((pFovIdx == NULL) || (pFovNum == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxs() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx++) {
            pNode = SvcVinTree_SearchFov(FovIdx);
            if (pNode != NULL) {
                pFovIdx[i] = pNode->Content.Value.Fov.FovIdx;
                i = i + 1U;
            }
        }
        *pFovNum = i;
    }
    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxs() done", 0U, 0U);

    return RetVal;
}

/**
 * Get the FovIdx of certain Vin ID
 * @param [in] VinID The Vin ID
 * @param [out] pFovIdx The array of fov idx based on input Vin ID
 * @param [out] pFovNum The number of valid value in the array
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetFovIdxsInVinID(UINT32 VinID, UINT32 *pFovIdx, UINT32 *pFovNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FovIdx, i = 0U;
    const SVC_VIN_TREE_NODE_s *pSearchNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInVinID() start", 0U, 0U);

    if ((pFovIdx == NULL) || (pFovNum == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInVinID() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx++) {
            pSearchNode = SvcVinTree_SearchFov(FovIdx);
            /* Find previous node until it is VIN */
            while ((pSearchNode != NULL) && (pSearchNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_VIN)) {
                pSearchNode = pSearchNode->pPrevNode;
            }
            if (pSearchNode != NULL) {
                if (pSearchNode->Content.Value.Vin.VinID == VinID) {
                    pFovIdx[i] = FovIdx;
                    i = i + 1U;
                }
            } else {
                ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInVinID() failed. VinTree cannot find Fov Idx", 0U, 0U);
            }
        }
        *pFovNum = i;
    }

    return RetVal;

}

/**
 * Get the FovIdx of certain Sensor Idx
 * @param [in] VinID The desired Vin ID
 * @param [in] SensorIdx The desired sensor idx
 * @param [out] pFovIdx The array of fov idx based on input Vin ID
 * @param [out] pFovNum The number of valid value in the array
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetFovIdxsInSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pFovIdx, UINT32 *pFovNum)
{
    UINT32 RetVal = SVC_OK, SearchResult;
    UINT32 FovIdx, i = 0U;
    const SVC_VIN_TREE_NODE_s *pSearchNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInSensorIdx() start", 0U, 0U);

    if ((pFovIdx == NULL) || (pFovNum == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInSensorIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx++) {
            SearchResult = 0U;
            pSearchNode = SvcVinTree_SearchFov(FovIdx);
            if (pSearchNode != NULL) {
                if (pSearchNode->Content.Value.Fov.Attribute == SVC_VIN_TREE_NODE_VALUE_FOV_BYPASS) {
                    continue;
                }
            }
            /* Find previous node until it is SENSOR */
            while (pSearchNode != NULL) {
                if (pSearchNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_VIN) {
                    if (pSearchNode->Content.Value.Vin.VinID == VinID) {
                        SearchResult |= ((UINT32) 1U << SVC_VIN_TREE_NODE_TYPE_VIN);
                    }
                } else if (pSearchNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_SENSOR) {
                    if (pSearchNode->Content.Value.Sensor.SensorIdx == SensorIdx) {
                        SearchResult |= ((UINT32) 1U << SVC_VIN_TREE_NODE_TYPE_SENSOR);
                    }
                } else {
                    /* do nothing */
                }
                pSearchNode = pSearchNode->pPrevNode;
            }
            if (((SearchResult & ((UINT32) 1U << SVC_VIN_TREE_NODE_TYPE_VIN)) > 0U) && ((SearchResult & ((UINT32) 1U << SVC_VIN_TREE_NODE_TYPE_SENSOR)) > 0U)) {
                pFovIdx[i] = FovIdx;
                i = i + 1U;
            } else {
                ResolCfg_Err(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInSensorIdx() failed. VinTree cannot find Fov Idx", 0U, 0U);
            }
        }
        *pFovNum = i;
    }

    return RetVal;
}

/**
 * Get the SensorGroup of certain Sensor Idx
 * @param [in] VinID The desired Vin ID
 * @param [in] SensorIdx The desired sensor idx
 * @param [out] pSensorGroup The sensor group of input SensorIdx in VinID
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSensorGroupOfSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pSensorGroup)
{
    UINT32 Rval;

    *pSensorGroup = SvcResCfg.SensorCfg[VinID][SensorIdx].SensorGroup;
    Rval = SVC_OK;

    return Rval;
}

/**
 * Get the serdes idx of certain sendor idx in certain VinID
 * @param [in] VinID The VinID
 * @param [in] SensorIdx The sensor idx in the input VinID
 * @param [out] pSerdesIdx The serdes idx containing the sensor idx in VinID, 0xDeadbeafU means no bridge find
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSerdesIdxOfSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pSerdesIdx)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSerdesIdxOfSensorIdx() start", 0U, 0U);

    if (pSerdesIdx == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSerdesIdxOfSensorIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        *pSerdesIdx = 0xDeadbeafU;
    }

    if (RetVal == SVC_OK) {
        pNode = SvcVinTree_SearchSensor(VinID, SensorIdx);
        if (pNode == NULL) {
            SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSerdesIdxOfSensorIdx() failed. VinTree cannot find sensor with VinID %d and SensorIdx Idx %d", VinID, SensorIdx);
            RetVal = SVC_NG;
        } else {
            /* Find previous node until it is sensor */
            while (pNode != NULL) {
                if (pNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                    *pSerdesIdx = pNode->Content.Value.Bridge.BridgeIdx;
                    break;
                } else {
                    /* Do nothing */
                }
                pNode = pNode->pPrevNode;
            }
        }
    }

    return RetVal;
}

/**
 * Get the serdes idx of certain fov idx
 * @param [in] FovIdx The fov idx
 * @param [out] pSerdesIdx The serdes idx containing the sensor idx in VinID, 0xDeadbeafU means no bridge find
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetSerdesIdxOfFovIdx(UINT32 FovIdx, UINT32 *pSerdesIdx)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSerdesIdxOfFovIdx() start", 0U, 0U);

    if (pSerdesIdx == NULL) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSerdesIdxOfFovIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        *pSerdesIdx = 0xDeadbeafU;
    }

    if (RetVal == SVC_OK) {
        pNode = SvcVinTree_SearchFov(FovIdx);
        if (pNode == NULL) {
            SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetSerdesIdxOfFovIdx() failed. VinTree cannot find Fov Idx", 0U, 0U);
            RetVal = SVC_NG;
        } else {
            /* Find previous node until it is sensor */
            while (pNode != NULL) {
                if (pNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_BRIDGE) {
                    *pSerdesIdx = pNode->Content.Value.Bridge.BridgeIdx;
                    break;
                } else {
                    /* Do nothing */
                }
                pNode = pNode->pPrevNode;
            }
        }
    }

    return RetVal;
}

/**
 * Get the vin bits of record stream index
 * @param [in] RecIdx record stream index
 * @param [out] pVinBits vin bits
 * return 0-OK, 1-NG
 */
void SvcResCfg_GetVinBitsOfRecIdx(UINT32 RecIdx, UINT32 *pVinBits)
{
    UINT32  i, VinId, FovId, Bits = 0, Src = 0U;

    for (i = 0U; i < SvcResCfg.RecStrm[RecIdx].StrmCfg.NumChan; i++) {
        FovId = SvcResCfg.RecStrm[RecIdx].StrmCfg.ChanCfg[i].FovId;

        if (SvcResCfg_GetFovSrc(FovId, &Src) == SVC_OK) {
            if ((Src == SVC_VIN_SRC_SENSOR) || (Src == SVC_VIN_SRC_YUV)) {
                if (SvcResCfg_GetVinIDOfFovIdx(FovId, &VinId) == SVC_OK) {
                    Bits = SetBits(Bits, ((UINT32)0x01U << VinId));
                }
            }
        }
    }

    *pVinBits = Bits;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
/**
 * Get the vin id of record stream index
 * @param [in] RecIdx record stream index
 * @param [out] pVinBits vin bits
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetVinIDOfRecIdx(UINT32 RecIdx, UINT32 *pVinId)
{
    UINT32 FovId, RetVal;

    FovId = SvcResCfg.RecStrm[RecIdx].StrmCfg.ChanCfg[0U].FovId;
    RetVal = SvcResCfg_GetVinIDOfFovIdx(FovId, pVinId);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetVinIDOfFovIdx failed for FovId %u", FovId, 0U);
    }
    return RetVal;
}
#endif

/**
 * Get the fov bits of record stream index
 * @param [in] RecIdx record stream index
 * @param [in] pFovBits fov bits
 * return 0-OK, 1-NG
 */
void SvcResCfg_GetFovBitsOfRecIdx(UINT32 RecIdx, UINT32 *pFovBits)
{
    UINT32  i, FovId, Bits = 0;;

    for (i = 0U; i < SvcResCfg.RecStrm[RecIdx].StrmCfg.NumChan; i++) {
        FovId = SvcResCfg.RecStrm[RecIdx].StrmCfg.ChanCfg[i].FovId;
        Bits = SetBits(Bits, ((UINT32)0x01U << FovId));
    }

    *pFovBits = Bits;
}

/**
 * Get the fov indices from decorder
 * @param [out] Array for receiving fov indices
 * @param [out] The index number of array
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetFovIdxsFromDec(UINT32 *pFovIdx, UINT32 *pFovNum)
{
    UINT32 FovIdx, i = 0U;
    const SVC_VIN_TREE_NODE_s *pSearchNode;

    UINT32 RetVal = SVC_OK;

    if ((pFovIdx == NULL) || (pFovNum == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInSensorIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx++) {
            pSearchNode = SvcVinTree_SearchFov(FovIdx);

            while ((pSearchNode != NULL) && (pSearchNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_DEC)) {
                pSearchNode = pSearchNode->pPrevNode;
            }
            if (pSearchNode != NULL) {
                if (pSearchNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_DEC) {
                    pFovIdx[i] = FovIdx;
                    i = i + 1U;
                }
            }
        }
        *pFovNum = i;
    }

    return RetVal;

}

/**
 * Get the fov indices from memory input
 * @param [out] Array for receiving fov indices
 * @param [out] The index number of array
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetFovIdxsFromMem(UINT32 *pFovIdx, UINT32 *pFovNum)
{
    UINT32 FovIdx, i = 0U;
    const SVC_VIN_TREE_NODE_s *pSearchNode;

    UINT32 RetVal = SVC_OK;

    if ((pFovIdx == NULL) || (pFovNum == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsInSensorIdx() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for (FovIdx = 0U; FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM; FovIdx++) {
            pSearchNode = SvcVinTree_SearchFov(FovIdx);
            while ((pSearchNode != NULL) && (pSearchNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_MEM)) {
                pSearchNode = pSearchNode->pPrevNode;
            }
            if (pSearchNode != NULL) {
                if (pSearchNode->Content.Type == SVC_VIN_TREE_NODE_TYPE_MEM) {
                    pFovIdx[i] = FovIdx;
                    i = i + 1U;
                }
            }
        }
        *pFovNum = i;
    }

    return RetVal;
}

/**
 * Get the fov indices which is bypass
 * @param [out] Array for receiving fov indices
 * @param [out] The index number of array
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetFovIdxBypassFrom(UINT32 FovIdx, UINT32 *pFovIdxFrom, UINT32 *pValid)
{
    UINT32 RetVal = SVC_OK;
    const SVC_VIN_TREE_NODE_s *pSearchNode;

    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsBypass() start", 0U, 0U);

    if ((pFovIdxFrom == NULL) || (pValid == NULL)) {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsBypass() failed. Input param should not be null", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        pSearchNode = SvcVinTree_SearchFov(FovIdx);
        /* Search previous node until it is memory node */
        while ((pSearchNode != NULL) && (pSearchNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_MEM)) {
            pSearchNode = pSearchNode->pPrevNode;
        }

        /* If it is memory node, check whether previous one is FOV node. If yes, then the FOV is from another FOV */
        if (pSearchNode != NULL) {
            do {
                pSearchNode = pSearchNode->pPrevNode;
            } while ((pSearchNode != NULL) && (pSearchNode->Content.Type != SVC_VIN_TREE_NODE_TYPE_FOV));

            if (pSearchNode != NULL) {
                *pFovIdxFrom = pSearchNode->Content.Value.Fov.FovIdx;
                *pValid = 1U;
            }
        }
    }
    ResolCfg_Dbg(SVC_LOG_RESOL_CFG, "SvcResCfg_GetFovIdxsBypass() done", 0U, 0U);

    return RetVal;
}

/**
 * Change display layout index in resolution configuration
 * @param [in] DispAltIdx desired user defined display layout index
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_ConfigDispAlt(UINT32 DispAltIdx)
{
    UINT32 RetVal = SVC_OK;

    if (DispAltIdx < SvcResCfg.DispAltNum) {

        UINT32 AltIdx = DispAltIdx;

        SvcResCfg.DispNum  = SvcResCfg.DispAlt[AltIdx].DispNum;
        SvcResCfg.DispBits = SvcResCfg.DispAlt[AltIdx].DispBits;
        SvcResCfg.DispAltIdx = AltIdx;

        RetVal = AmbaWrap_memcpy(SvcResCfg.DispStrm, SvcResCfg.DispAlt[AltIdx].DispStrm, sizeof(SVC_DISP_STRM_s) * AMBA_DSP_MAX_VOUT_NUM);
        if (SVC_OK == RetVal) {
            SvcLog_OK(SVC_LOG_RESOL_CFG, "SvcResCfg.Disp setting is switched to AltIdx (%d)", DispAltIdx, 0U);
        } else {
            SvcLog_NG(SVC_LOG_RESOL_CFG, "Copy AltDispStrm failed with %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_RESOL_CFG, "AltIdx(%d) should smaller than %d", DispAltIdx, SvcResCfg.DispAltNum);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 * Set debug enable flag
 * @param [in] DebugEnable Enable the debug message
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_SetDebugEnable(UINT32 DebugEnable)
{
    SvcResolCfgDebugEnable = DebugEnable;

    return SVC_OK;
}

/**
 * Get Still catpure enable or not
 * @param [out] pStillEnable Still enable or not
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_GetStillEnable(UINT8 *pStillEnable)
{
    UINT32 Rval;

    *pStillEnable = SvcResCfg.StillCfg.EnableStill;
    Rval = SVC_OK;

    return Rval;
}

/**
 * Dump resolution configuration
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_Dump(void)
{
    UINT32 RetVal;
    UINT32 VinID, SensorIdx, FovIdx;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM] = {0U}, VinNum = 0U;
    UINT32 SensorIdxs[SVC_MAX_NUM_SENSOR_PER_VIN] = {0U}, SensorNum = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0U}, FovNum = 0U;

    UINT32 i, j, k;

    UINT32 PtrToU32;

    AmbaMisra_TouchUnused(&PtrToU32);

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "====================", 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, "FormatName = ", 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, SvcResCfg.FormatName, 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, "VinTree = ", 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, SvcResCfg.VinTree, 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, "", 0U, 0U);

    RetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
    if (SVC_OK == RetVal) {
        for (i = 0U; i < VinNum; i++) {
            VinID = VinIDs[i];
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "VinCfg[%d] = ", VinID, 0);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "CapWin = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  OffsetX = %d", SvcResCfg.VinCfg[VinID].CapWin.OffsetX, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  OffsetY = %d", SvcResCfg.VinCfg[VinID].CapWin.OffsetY, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width   = %d", SvcResCfg.VinCfg[VinID].CapWin.Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height  = %d", SvcResCfg.VinCfg[VinID].CapWin.Height, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "FrameRate = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  TimeScale       = %d", SvcResCfg.VinCfg[VinID].FrameRate.TimeScale, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  NumUnitsInTick  = %d", SvcResCfg.VinCfg[VinID].FrameRate.NumUnitsInTick, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "SubChanNum = %d", SvcResCfg.VinCfg[VinID].SubChanNum, 0U);
            for (j = 0U; j < SvcResCfg.VinCfg[VinID].SubChanNum; j++) {
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "  SubChan .IsVirtual = %d, .Index = %d", SvcResCfg.VinCfg[VinID].SubChanCfg[j].SubChan.IsVirtual, SvcResCfg.VinCfg[VinID].SubChanCfg[j].SubChan.Index);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Option = %d, ConCatNum = %d", SvcResCfg.VinCfg[VinID].SubChanCfg[j].Option, SvcResCfg.VinCfg[VinID].SubChanCfg[j].ConCatNum);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "  IntcNum = %d", SvcResCfg.VinCfg[VinID].SubChanCfg[j].IntcNum, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "  CaptureWindow = ", 0U, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    OffsetX = %d", SvcResCfg.VinCfg[VinID].SubChanCfg[j].CaptureWindow.OffsetX, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    OffsetY = %d", SvcResCfg.VinCfg[VinID].SubChanCfg[j].CaptureWindow.OffsetY, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Width   = %d", SvcResCfg.VinCfg[VinID].SubChanCfg[j].CaptureWindow.Width, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Height  = %d", SvcResCfg.VinCfg[VinID].SubChanCfg[j].CaptureWindow.Height, 0U);
            }
        }
    }

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "", 0U, 0U);
    if (SVC_OK == RetVal) {
        for (i = 0U; i < VinNum; i++) {
            VinID = VinIDs[i];
            RetVal = SvcResCfg_GetSensorIdxsInVinID(VinID, SensorIdxs, &SensorNum);
            if (SVC_OK == RetVal) {
                for (j = 0U; j < SensorNum; j++) {
                    SensorIdx = SensorIdxs[j];
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "SensorCfg[%d][%d]", VinID, SensorIdx);
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "SensorMode = %d", SvcResCfg.SensorCfg[VinID][SensorIdx].SensorMode, 0U);
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "SensorGroup = %d", SvcResCfg.SensorCfg[VinID][SensorIdx].SensorGroup, 0U);
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "IQTable = %d", SvcResCfg.SensorCfg[VinID][SensorIdx].IQTable, 0U);
                    for (k = 0U; k < AMBA_DSP_MAX_VIN_TD_NUM; k++) {
                        SvcLog_DBG(SVC_LOG_RESOL_CFG, "TimeDivisionIQTable[%d] = %d", k, SvcResCfg.SensorCfg[VinID][SensorIdx].TimeDivisionIQTable[k]);
                    }
                }
            }
        }
    }

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "", 0U, 0U);
    RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
    if (SVC_OK == RetVal) {
        for (i = 0U; i < FovNum; i++) {
            FovIdx = FovIdxs[i];
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "FovCfg[%d] =", FovIdx, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "RawWin = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  OffsetX = %d", SvcResCfg.FovCfg[FovIdx].RawWin.OffsetX, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  OffsetY = %d", SvcResCfg.FovCfg[FovIdx].RawWin.OffsetY, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width   = %d", SvcResCfg.FovCfg[FovIdx].RawWin.Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height  = %d", SvcResCfg.FovCfg[FovIdx].RawWin.Height, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "ActWin = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  OffsetX = %d", SvcResCfg.FovCfg[FovIdx].ActWin.OffsetX, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  OffsetY = %d", SvcResCfg.FovCfg[FovIdx].ActWin.OffsetY, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width   = %d", SvcResCfg.FovCfg[FovIdx].ActWin.Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height  = %d", SvcResCfg.FovCfg[FovIdx].ActWin.Height, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "MainWin = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width   = %d", SvcResCfg.FovCfg[FovIdx].MainWin.Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height  = %d", SvcResCfg.FovCfg[FovIdx].MainWin.Height, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "PyramidBis = 0x%x", SvcResCfg.FovCfg[FovIdx].PyramidBits, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "HierWin = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width   = %d", SvcResCfg.FovCfg[FovIdx].HierWin.Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height  = %d", SvcResCfg.FovCfg[FovIdx].HierWin.Height, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "PipeCfg = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  RotateFlip     = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.RotateFlip, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  HdrExposureNum = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.HdrExposureNum, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  HdrBlendHieght = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.HdrBlendHieght, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  MctfDisable    = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.MctfDisable, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  MctsDisable    = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.MctsDisable, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  LinearCE       = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.LinearCE, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  RawCompression = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.RawCompression, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  PipeMode       = %d", SvcResCfg.FovCfg[FovIdx].PipeCfg.PipeMode, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "VirtChan .Enable = 0x%x .Id = 0x%x = ", SvcResCfg.FovCfg[FovIdx].VirtChan.Enable, SvcResCfg.FovCfg[FovIdx].VirtChan.Id);
        }
    }

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "", 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, "DispNum = %d, DispBits = 0x%x", SvcResCfg.DispNum, SvcResCfg.DispBits);
    for (i = 0U; i < SvcResCfg.DispNum; i++) {
        if ((SvcResCfg.DispBits & ((UINT32) 1U << i)) > 0U) {
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "DispStrm[%d] = ", i, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "VoutID = %d", SvcResCfg.DispStrm[i].VoutID, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "DevMode = %d", SvcResCfg.DispStrm[i].DevMode, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "FrameRate = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Interlace = %d", SvcResCfg.DispStrm[i].FrameRate.Interlace, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  TimeScale = %d", SvcResCfg.DispStrm[i].FrameRate.TimeScale, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  NumUnitsInTick = %d", SvcResCfg.DispStrm[i].FrameRate.NumUnitsInTick, 0U);

            SvcLog_DBG(SVC_LOG_RESOL_CFG, "StrmCfg = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "Win = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width   = %d", SvcResCfg.DispStrm[i].StrmCfg.Win.Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height  = %d", SvcResCfg.DispStrm[i].StrmCfg.Win.Height, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "MaxWin = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width   = %d", SvcResCfg.DispStrm[i].StrmCfg.MaxWin.Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height  = %d", SvcResCfg.DispStrm[i].StrmCfg.MaxWin.Height, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "NumChan = %d", SvcResCfg.DispStrm[i].StrmCfg.NumChan, 0U);

            for (j = 0U; j < SvcResCfg.DispStrm[i].StrmCfg.NumChan; j++) {
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "  ChanCfg[%d] = ", j, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    FovId = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].FovId, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    RoatateFlip = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].RotateFlip, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    SrcWin = ", 0U, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     OffsetX = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.OffsetX, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     OffsetY = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.OffsetY, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     Width   = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.Width, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     Height  = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.Height, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    DstWin = ", 0U, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     OffsetX = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].DstWin.OffsetX, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     OffsetY = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].DstWin.OffsetY, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     Width   = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Width, 0U);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "     Height  = %d", SvcResCfg.DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Height, 0U);
            }

            SvcLog_DBG(SVC_LOG_RESOL_CFG, "SyncVinID / DelayTime: %d / %d", SvcResCfg.DispStrm[i].VinVoutSyncCfg.SyncVinID, SvcResCfg.DispStrm[i].VinVoutSyncCfg.DelayTime);
        }
    }

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "", 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, "RecNum = %d, RecBits = 0x%x", SvcResCfg.RecNum, SvcResCfg.RecBits);
    for (i = 0U; i < SvcResCfg.RecNum; i++) {
        if ((SvcResCfg.RecBits & ((UINT32) 1U << i)) > 0U) {
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "RecStrm[%d] = ", i, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "RecSetting = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  SrcBits  = 0x%x", SvcResCfg.RecStrm[i].RecSetting.SrcBits, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  DestBits = 0x%x", SvcResCfg.RecStrm[i].RecSetting.DestBits, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  RecId = %d", SvcResCfg.RecStrm[i].RecSetting.RecId, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  M     = %d", SvcResCfg.RecStrm[i].RecSetting.M, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  N     = %d", SvcResCfg.RecStrm[i].RecSetting.N, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  IdrInterval = %d", SvcResCfg.RecStrm[i].RecSetting.IdrInterval, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  BitRate = %d", SvcResCfg.RecStrm[i].RecSetting.BitRate, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  FrameRate = ", 0U, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "     Interlace = %d", SvcResCfg.RecStrm[i].RecSetting.FrameRate.Interlace, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "     TimeScale = %d", SvcResCfg.RecStrm[i].RecSetting.FrameRate.TimeScale, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "     NumUnitsInTick = %d", SvcResCfg.RecStrm[i].RecSetting.FrameRate.NumUnitsInTick, 0U);
        }
    }
#if defined(CONFIG_BUILD_CV)
    SvcLog_DBG(SVC_LOG_RESOL_CFG, "CvFlowNum = %d, CvFlowBits = 0x%x", SvcResCfg.CvFlowNum, SvcResCfg.CvFlowBits);
    for (i = 0U; i < SvcResCfg.CvFlowNum; i++) {
        if ((SvcResCfg.CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            AmbaMisra_TypeCast(&PtrToU32, &SvcResCfg.CvFlow[i].pCvObj);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "CvFlow[%d] = ", i, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Chan       = %d", SvcResCfg.CvFlow[i].Chan, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  CvFlowType = %d", SvcResCfg.CvFlow[i].CvFlowType, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  pCvObj     = 0x%x", PtrToU32, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  CvModeID   = %d", SvcResCfg.CvFlow[i].CvModeID, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  InputCfg.SrcRate    = %d", SvcResCfg.CvFlow[i].InputCfg.SrcRate, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  InputCfg.InputNum   = %d", SvcResCfg.CvFlow[i].InputCfg.InputNum, 0U);
            for (j = 0U; j < SvcResCfg.CvFlow[i].InputCfg.InputNum; j++) {
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Input[%d].DataSrc     = %d", j, SvcResCfg.CvFlow[i].InputCfg.Input[j].DataSrc);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Input[%d].StrmId      = %d", j, SvcResCfg.CvFlow[i].InputCfg.Input[j].StrmId);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Input[%d].FrameWidth  = %d", j, SvcResCfg.CvFlow[i].InputCfg.Input[j].FrameWidth);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Input[%d].FrameHeight = %d", j, SvcResCfg.CvFlow[i].InputCfg.Input[j].FrameHeight);
                SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Input[%d].NumRoi      = %d", j, SvcResCfg.CvFlow[i].InputCfg.Input[j].NumRoi);
                for (k = 0U; k <SvcResCfg.CvFlow[i].InputCfg.Input[j].NumRoi; k++) {
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "      Roi[%d].Index     = %d", k, SvcResCfg.CvFlow[i].InputCfg.Input[j].Roi[k].Index);
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "      Roi[%d].StartX    = %d", k, SvcResCfg.CvFlow[i].InputCfg.Input[j].Roi[k].StartX);
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "      Roi[%d].StartY    = %d", k, SvcResCfg.CvFlow[i].InputCfg.Input[j].Roi[k].StartY);
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "      Roi[%d].Reserved0 = %d", k, SvcResCfg.CvFlow[i].InputCfg.Input[j].Roi[k].Reserved0);
                    SvcLog_DBG(SVC_LOG_RESOL_CFG, "      Roi[%d].Reserved1 = %d", k, SvcResCfg.CvFlow[i].InputCfg.Input[j].Roi[k].Reserved1);
                }
            }
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "    OutputTag[%d]   = %d", 0U, SvcResCfg.CvFlow[i].OutputCfg.OutputTag[0U]);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "    OutputTag[%d]   = %d", 1U, SvcResCfg.CvFlow[i].OutputCfg.OutputTag[1U]);
        }
    }
#endif
    return RetVal;
}

/**
 * Dump maximum resolution configuration
 * return 0-OK, 1-NG
 */
UINT32 SvcResCfg_DumpMax(void)
{
    UINT32 i, j;

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "====================", 0U, 0U);
    SvcLog_DBG(SVC_LOG_RESOL_CFG, "VinBits = %d", SvcResCfgMax.VinBits, 0U);
    for (i = 0U; i < AMBA_DSP_MAX_VIN_NUM; i++) {
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "VinWin[%d] = ", i, 0U);
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width = %d", SvcResCfgMax.VinWin[i].Width, 0U);
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height = %d", SvcResCfgMax.VinWin[i].Height, 0U);
    }

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "FovNum = %d", SvcResCfgMax.FovNum, 0U);
    for (i = 0U; i < SvcResCfgMax.FovNum; i++) {
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "FovWin[%d] = ", i, 0U);
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Width = %d", SvcResCfgMax.FovWin[i].Width, 0U);
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "  Height = %d", SvcResCfgMax.FovWin[i].Height, 0U);
    }

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "DispNum = %d", SvcResCfgMax.DispNum, 0U);
    for (i = 0U; i < SvcResCfgMax.DispNum; i++) {
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "Disp[%d] = ", i, 0U);
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "DispChanBits[%d] = %d", i, SvcResCfgMax.DispChanBits[i]);
        for (j = 0U; j < SvcResCfgMax.DispChanNum[i]; j++) {
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  DispChan[%d][%d]", i, j);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Width = %d", SvcResCfgMax.DispChan[i][j].Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Height = %d", SvcResCfgMax.DispChan[i][j].Height, 0U);
        }
    }

    SvcLog_DBG(SVC_LOG_RESOL_CFG, "RecNum = %d", SvcResCfgMax.RecNum, 0U);
    for (i = 0U; i < SvcResCfgMax.RecNum; i++) {
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "Rec[%d] = ", i, 0U);
        SvcLog_DBG(SVC_LOG_RESOL_CFG, "RecChanBits[%d] = %d", i, SvcResCfgMax.RecChanBits[i]);
        for (j = 0U; j < SvcResCfgMax.RecChanNum[i]; j++) {
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "  RecChan[%d][%d]", i, j);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Width = %d", SvcResCfgMax.RecChan[i][j].Width, 0U);
            SvcLog_DBG(SVC_LOG_RESOL_CFG, "    Height = %d", SvcResCfgMax.RecChan[i][j].Height, 0U);
        }
    }

    return SVC_OK;
}
