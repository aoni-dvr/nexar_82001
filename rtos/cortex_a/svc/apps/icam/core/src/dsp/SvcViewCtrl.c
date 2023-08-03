/**
 *  @file SvcViewCtrl.c
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
 *  @details svc view control
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaSensor.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaCalib_WarpDef.h"

/* SSP */
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"

/* commonsvc-image-warp */
#include "AmbaWU_WarpUtility.h"

/* svc-framework */
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcLog.h"

/* svc-shared */
#include "SvcIK.h"
#include "SvcViewCtrl.h"
#include "SvcResCfg.h"
#include "SvcVinSrc.h"
#include "SvcLiveview.h"

static UINT32 ViewCtrl_RawDataReady(const void *pEventInfo);
static UINT32 ViewCtrl_TaskCreate(void);
static UINT32 ViewCtrl_TaskDelete(void);
static UINT32 ViewCtrl_Validate(void);
static UINT32 ViewCtrl_Restore(const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32 ViewCtrl_PanTilt(const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32 ViewCtrl_Warp(const AMBA_IK_MODE_CFG_s *pImgMode);
static UINT32 ViewCtrl_PrevSetSrcWin(UINT32 VoutID, UINT32 FovIdx);
static UINT32 ViewCtrl_PrevSetDstWin(UINT32 VoutID, UINT32 FovIdx);
static void*  ViewCtrl_TaskEntry(void* EntryArg);
static UINT32 ViewCtrl_InfoUpdate(SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg);
static UINT32 ViewCtrl_InfoValidCheck(SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg);

static UINT32 SvcViewCtrlDebugEnable = 1U;
static UINT32 SvcViewCtrlInit = 0U;
static UINT32 SvcViewCtrlValidate_Config = 0U;
static UINT32 SvcViewCtrlValidate_YuvStream = 0U;
static SVC_VIEW_CTRL_INFO_s SvcViewCtrlInfo[AMBA_DSP_MAX_VIN_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VIEW_CTRL_TASK_s SvcViewCtrlTaskInfo[AMBA_DSP_MAX_VIN_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VIEW_CTRL_WARP_INFO_s SvcViewCtrlWarpInfo[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
static SVC_VIEW_CTRL_WARP_DEFAULT_INFO_s SvcViewCtrlDefaultInfo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VIEW_CTRL_PREV_INFO_s SvcViewCtrlPrevInfo[AMBA_DSP_MAX_VOUT_NUM][AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static SVC_VIEW_CTRL_PREV_INFO_s SvcViewCtrlDefaultPrevInfo[AMBA_DSP_MAX_VOUT_NUM][AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;

#define VIEW_CTRL_TILE_EXP_NUM    (5U)
#define VIEW_CTRL_ZOOM_RATIO_MAX  (2.5)
#define VIEW_CTRL_ROTATE_MAX      (5.0)
#define VIEW_CTRL_ROTATE_MIN      (-5.0)

#define VIEW_CTRL_DBG_LVL_2       (0x2U)

#define VIEW_CTRL_PI  (3.14159265)

#define SVC_LOG_VIEW_CTRL "VIEW_CTRL"

static void ViewCtrl_Dbg(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcViewCtrlDebugEnable > 0U) {
        SvcLog_OK(pModule, pFormat, Arg1, Arg2);
    }
}

static void ViewCtrl_DbgLvl(UINT32 Lvl, const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((SvcViewCtrlDebugEnable & Lvl) > 0U) {
        SvcLog_OK(pModule, pFormat, Arg1, Arg2);
    }
}

static void ViewCtrl_Err(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_NG(pModule, pFormat, Arg1, Arg2);
}

/**
* initialization of view control
* @return 0-OK, 1-NG
*/
UINT32 SvcViewCtrl_Init(void)
{
    UINT32 RetVal;
    UINT32 i;

    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "@@ Init Begin", 0U, 0U);

    if (SvcViewCtrlInit == 0U) {
        for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
            SvcViewCtrlTaskInfo[i].TaskCreated = 0U;
        }
        RetVal = ViewCtrl_TaskCreate();
        if (SVC_OK != RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskCreate() failed", 0U, 0U);
        } else {
            SvcViewCtrlInit = 1U;
        }
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, ViewCtrl_RawDataReady);
        if (SVC_OK != RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "AmbaDSP_EventHandlerRegister() failed", 0U, 0U);
        }
    } else {
        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Init() has been initialized", 0U, 0U);
        RetVal = SVC_NG;
    }

    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "@@ Init End", 0U, 0U);

    return RetVal;
}

/**
* de-initialization of view control
* @return 0-OK, 1-NG
*/
UINT32 SvcViewCtrl_DeInit(void)
{
    UINT32 RetVal;

    if (SvcViewCtrlInit == 1U) {
        RetVal = ViewCtrl_TaskDelete();
        if (SVC_OK != RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskDelete() failed", 0U, 0U);
        } else {
            SvcViewCtrlInit = 0U;
        }
        RetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, ViewCtrl_RawDataReady);
        if (SVC_OK != RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "AmbaDSP_EventHandlerUnRegister() failed", 0U, 0U);
        }
    } else {
        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_DeInit() has not been initialized before", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* configuration of view control
* @param [in] pMirrorInfo info block of view control
* @return 0-OK, 1-NG
*/
UINT32 SvcViewCtrl_Config(const SVC_VIEW_CTRL_WARP_INFO_s *pMirrorInfo)
{
    UINT32 i, j, k, FovIdx = 0U, FovNum = 0U, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0U};
    UINT32 VoutID;
    UINT32 RetVal, Src = 0U;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 RawWidth;
    UINT32 RawHeight;
    INT32  ActiveOffsetY;
    INT32  ActiveOffsetX;
    UINT32 ActiveWidth;
    UINT32 ActiveHeight;
    UINT32 WarpEnable;

    static AMBA_IK_GRID_POINT_s DefaultWarpTable[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_WARP_TBL_LEN] GNU_SECTION_NOZEROINIT;

    if (SvcViewCtrlInit == 1U) {
        RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
        if (SVC_OK == RetVal) {
            for (i = 0; i < FovNum; i++) {
                AMBA_IK_MODE_CFG_s ImgMode = {0};

                RetVal = SvcResCfg_GetFovSrc(i, &Src);
                if (SVC_OK == RetVal) {
                    if (Src == SVC_VIN_SRC_MEM_DEC) {
                        continue;
                    }
                }

                FovIdx = FovIdxs[i];
                ImgMode.ContextId = FovIdx;

                RawWidth      = pCfg->FovCfg[FovIdx].RawWin.Width;
                RawHeight     = pCfg->FovCfg[FovIdx].RawWin.Height;
                ActiveOffsetX = (INT32) pCfg->FovCfg[FovIdx].ActWin.OffsetX;
                ActiveOffsetY = (INT32) pCfg->FovCfg[FovIdx].ActWin.OffsetY;
                ActiveWidth   = pCfg->FovCfg[FovIdx].ActWin.Width;
                ActiveHeight  = pCfg->FovCfg[FovIdx].ActWin.Height;

                /* Initialized Mirror Handler Warp Info */
                if (SVC_OK == RetVal) {
                    RetVal = AmbaWrap_memcpy(&SvcViewCtrlWarpInfo[FovIdx], &pMirrorInfo[FovIdx], sizeof(SVC_VIEW_CTRL_WARP_INFO_s));
                    if (SVC_OK != RetVal) {
                        ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() memcpy with issue 0x%x", RetVal, 0U);
                    }
                }

                if ((ActiveWidth == 0U) || (ActiveHeight == 0U)) {
                    ActiveWidth = RawWidth;
                    ActiveHeight = RawHeight;
                    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() force to make ActiveWidth and ActiveHeight to (%d, %d)", ActiveWidth, ActiveHeight);
                }

                SvcViewCtrlDefaultInfo[FovIdx].RawWidth  = RawWidth;
                SvcViewCtrlDefaultInfo[FovIdx].RawHeight = RawHeight;
                SvcViewCtrlDefaultInfo[FovIdx].FovWidth  = ActiveWidth;
                SvcViewCtrlDefaultInfo[FovIdx].FovHeight = ActiveHeight;
                SvcViewCtrlDefaultInfo[FovIdx].ShiftX    = (ActiveOffsetX - (((INT32) RawWidth - (INT32) ActiveWidth) / 2));
                SvcViewCtrlDefaultInfo[FovIdx].ShiftY    = (ActiveOffsetY - (((INT32) RawHeight - (INT32) ActiveHeight) / 2));
                SvcViewCtrlDefaultInfo[FovIdx].ZoomX     = (DOUBLE) (RawWidth) / (DOUBLE) (ActiveWidth);
                SvcViewCtrlDefaultInfo[FovIdx].ZoomY     = (DOUBLE) (RawHeight) / (DOUBLE) (ActiveHeight);

                for (j = 0; j < pCfg->DispNum; j++) {
                    if ((pCfg->DispBits & ((UINT32) 1U << j)) > 0U) {
                        VoutID = pCfg->DispStrm[j].VoutID;
                        for (k = 0; k < pCfg->DispStrm[j].StrmCfg.NumChan; k++) {
                            if (pCfg->DispStrm[j].StrmCfg.ChanCfg[k].FovId == FovIdx) {
                                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX = pCfg->DispStrm[j].StrmCfg.ChanCfg[k].SrcWin.OffsetX;
                                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY = pCfg->DispStrm[j].StrmCfg.ChanCfg[k].SrcWin.OffsetY;
                                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width   = pCfg->DispStrm[j].StrmCfg.ChanCfg[k].SrcWin.Width;
                                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height  = pCfg->DispStrm[j].StrmCfg.ChanCfg[k].SrcWin.Height;
                                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomX   = 1.0;
                                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomY   = 1.0;
                            }
                        }
                    }
                }
                if (SVC_OK != AmbaWrap_memcpy(SvcViewCtrlDefaultPrevInfo, SvcViewCtrlPrevInfo, sizeof(SvcViewCtrlDefaultPrevInfo))) {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err. Default PrevInfo copy failed", 0U, 0U);
                }

                RetVal = AmbaIK_GetWarpEnb(&ImgMode, &WarpEnable);
                if (SVC_OK == RetVal) {
                    if (WarpEnable != 0U) {
                        RetVal = AmbaIK_GetWarpInfo(&ImgMode, &SvcViewCtrlDefaultInfo[FovIdx].Warp);
                        if (SVC_OK == RetVal) {
                            /* Copy other module's warp information as default value */
                            RetVal = AmbaWrap_memcpy(DefaultWarpTable[FovIdx], SvcViewCtrlDefaultInfo[FovIdx].Warp.pWarp, sizeof(AMBA_IK_GRID_POINT_s) * SvcViewCtrlDefaultInfo[FovIdx].Warp.HorGridNum * SvcViewCtrlDefaultInfo[FovIdx].Warp.VerGridNum);
                            if (SVC_OK == RetVal) {
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.pWarp = DefaultWarpTable[FovIdx];
                            } else {
                                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err. Warp table copy failed %d", RetVal, 0U);
                            }
                        } else {
                            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err. Warp Info get failed %d", RetVal, 0U);
                        }
                    } else {
                        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
                        RetVal = AmbaIK_GetWindowSizeInfo(&ImgMode, &WindowSizeInfo);
                        if (SVC_OK == RetVal) {
                            UINT32 CapWidth, CapHeight;
                            UINT32 HorGridNum, VerGridNum;
                            UINT32 TileWidthExp = VIEW_CTRL_TILE_EXP_NUM, TileHeightExp = VIEW_CTRL_TILE_EXP_NUM;

                            CapWidth = WindowSizeInfo.VinSensor.Width;
                            CapHeight = WindowSizeInfo.VinSensor.Height;
                            HorGridNum = (UINT32) ((CapWidth + ((1UL << TileWidthExp) - 1U)) >> TileWidthExp) + 1U;
                            VerGridNum = (UINT32) ((CapHeight + ((1UL << TileHeightExp) - 1U)) >> TileHeightExp) + 1U;

                            RetVal |= AmbaWrap_memcpy(&SvcViewCtrlDefaultInfo[FovIdx].Warp.VinSensorGeo, &WindowSizeInfo.VinSensor, sizeof(AMBA_IK_VIN_SENSOR_GEOMETRY_s));
                            RetVal |= AmbaWrap_memset(DefaultWarpTable[FovIdx], 0, sizeof(AMBA_IK_GRID_POINT_s) * MAX_WARP_TBL_LEN);
                            if (SVC_OK == RetVal) {
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.Version = 0x20180401;
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.HorGridNum = HorGridNum;
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.VerGridNum = VerGridNum;
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.TileWidthExp = TileWidthExp;
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.TileHeightExp = TileHeightExp;
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.Enb_2StageCompensation = 0;
                                SvcViewCtrlDefaultInfo[FovIdx].Warp.pWarp = DefaultWarpTable[FovIdx];

                                /* Set default warp table (warp table by default all 0) */
                                RetVal = AmbaIK_SetWarpEnb(&ImgMode, 1U);
                                if (SVC_OK == RetVal) {
                                    AMBA_IK_DUMMY_MARGIN_RANGE_s DummyRange;
                                    DOUBLE Theta = (DOUBLE) (VIEW_CTRL_ROTATE_MAX * VIEW_CTRL_PI) / (DOUBLE) 180;
                                    DOUBLE SinTheta = 0.0, CosTheta = 0.0;
                                    DOUBLE X, Y;
                                    DOUBLE NewX, NewY;
                                    DOUBLE DummyHor = 0.0, DummyVer = 0.0;
                                    if (SVC_OK != AmbaWrap_sin(Theta, &SinTheta)) {
                                        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err, AmbaWrap_sin failed", 0U, 0U);
                                    }
                                    if (SVC_OK != AmbaWrap_cos(Theta, &CosTheta)) {
                                        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err, AmbaWrap_cos failed", 0U, 0U);
                                    }

                                    /* First quadrant */
                                    X = (DOUBLE) ActiveWidth * 0.5;
                                    Y = (DOUBLE) ActiveHeight * 0.5;

                                    NewY = (SinTheta * X) + (CosTheta * Y);
                                    if ((NewY * 2.0) < (DOUBLE) RawHeight) {
                                        DummyVer = (NewY - Y) / (DOUBLE) RawHeight * (DOUBLE) 65536;
                                    }

                                    /* Fourth quadrant */
                                    Y = Y * -1.0;

                                    NewX = (CosTheta * X) + ((-1.0 * SinTheta) * Y);
                                    if ((NewX * 2.0) < (DOUBLE) RawWidth) {
                                        DummyHor = (NewX - X) / (DOUBLE) RawWidth * (DOUBLE) 65536;
                                    }

                                    if ((DummyVer > 0.0) && (DummyHor > 0.0)) {
                                        RetVal = AmbaIK_GetDummyMarginRange(&ImgMode, &DummyRange);

                                        /* Update the dummy range if view control operation requirement is much larger */
                                        if (SVC_OK == RetVal) {
                                            DummyRange.Enable = 1;
                                            if ((DOUBLE) DummyRange.Left < DummyHor) {
                                                DummyRange.Left   = (UINT32) DummyHor;
                                            }
                                            if ((DOUBLE) DummyRange.Right < DummyHor) {
                                                DummyRange.Right  = (UINT32) DummyHor;
                                            }
                                            if ((DOUBLE) DummyRange.Top < DummyVer) {
                                                DummyRange.Top    = (UINT32) DummyVer;
                                            }
                                            if ((DOUBLE) DummyRange.Bottom < DummyVer) {
                                                DummyRange.Bottom = (UINT32) DummyVer;
                                            }
                                            RetVal = AmbaIK_SetDummyMarginRange(&ImgMode, &DummyRange);
                                            if (SVC_OK == RetVal) {
                                                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "DummyRange Left %d Top %d", DummyRange.Left, DummyRange.Top);
                                            }
                                        }
                                    }
                                    /* Enable warp effect */
                                    RetVal = AmbaIK_SetWarpInfo(&ImgMode, &SvcViewCtrlDefaultInfo[FovIdx].Warp);
                                    if (SVC_OK == RetVal) {

                                    } else {
                                        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err. Warp feature enable failed %d", RetVal, 0U);
                                    }
                                } else {
                                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err. Default warp table set failed %d", RetVal, 0U);
                                }
                            } else {
                                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err. Vin sensor geo copy failed %d", RetVal, 0U);
                            }
                        } else {
                            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err. Window Size Info get failed %d", RetVal, 0U);
                        }
                    }

                    /* Update SVC_VIEW_CTRL_WARP_INFO_s */
                    if (SVC_OK != AmbaWrap_memcpy(&SvcViewCtrlWarpInfo[FovIdx].Warp, &SvcViewCtrlDefaultInfo[FovIdx].Warp, sizeof(AMBA_IK_WARP_INFO_s))) {
                        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() err, AmbaWrap_memcpy failed, AMBA_IK_WARP_INFO_s", 0U, 0U);
                    }
                }

                /* Apply to take effect */
                if (SVC_OK == RetVal) {
                    RetVal = ViewCtrl_PanTilt(&ImgMode);
                    if (SVC_OK != RetVal) {
                        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() failed. ViewCtrl_PanTilt failed (%d)", RetVal, 0U);
                    }
                    RetVal = ViewCtrl_Warp(&ImgMode);
                    if (SVC_OK != RetVal) {
                        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() failed. ViewCtrl_Warp failed (%d)", RetVal, 0U);
                    }
                } else {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() failed. AmbaWrap_memcpy() failed (%d)", RetVal, 0U);
                }
            }
        } else {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() failed. SvcResCfg_GetFovIdxs() failed (%d)", RetVal, 0U);
        }
    } else {
        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Config() failed. Need to SvcViewCtrl_Init() first", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* execution of view control
* @param [in] MirrorCmdMsg execution command
* @return none
*/
void SvcViewCtrl_Exec(SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg)
{
    UINT32 RetVal;
    UINT8 VinIdx = MirrorCmdMsg.VinID;

    if (SvcViewCtrlTaskInfo[VinIdx].TaskCreated == 0U) {
        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Exec() failed since task is not created", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        RetVal = SVC_OK;
    }

    if (SVC_OK == RetVal) {
        switch (MirrorCmdMsg.Cmd) {
            case SVC_VIEW_CTRL_WARP_CMD_PAN:
            case SVC_VIEW_CTRL_WARP_CMD_TILT:
            case SVC_VIEW_CTRL_WARP_CMD_ZOOM:
            case SVC_VIEW_CTRL_WARP_CMD_ROTATE:
            case SVC_VIEW_CTRL_WARP_CMD_SET_WIN:
            case SVC_VIEW_CTRL_WARP_CMD_SET_WARP:
            case SVC_VIEW_CTRL_PREV_CMD_PAN:
            case SVC_VIEW_CTRL_PREV_CMD_TILT:
            case SVC_VIEW_CTRL_PREV_CMD_ZOOM:
            case SVC_VIEW_CTRL_PREV_CMD_SET_SRCWIN:
            case SVC_VIEW_CTRL_PREV_CMD_SET_DSTWIN:
            case SVC_VIEW_CTRL_CMD_VALIDATE:
                RetVal = AmbaKAL_MsgQueueSend(&SvcViewCtrlInfo[VinIdx].QueueId, &MirrorCmdMsg, AMBA_KAL_NO_WAIT);
                if (SVC_NG == RetVal) {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_Exec() err. Msg Send failed", 0U, 0U);
                }
            break;
            default:
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "The command %d is not supported yet", MirrorCmdMsg.Cmd, 0U);
            break;
        }
    }
}

/**
* enable/disable debug messages of voew control
* @param [in] DebugEnable 0(disable) 1(enable)
* @return none
*/
void SvcViewCtrl_DebugEnable(UINT32 DebugEnable)
{
    SvcViewCtrlDebugEnable = DebugEnable;
    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "SvcViewCtrlDebugEnable = %d", SvcViewCtrlDebugEnable, 0U);
}

/**
* get info block of view control
* @param [in] FovIdx index of fov
* @param [out] pMirrorInfo info block of view control
* @return 0-OK, 1-NG
*/
UINT32 SvcViewCtrl_GetViewCtrlInfo(UINT32 FovIdx, SVC_VIEW_CTRL_WARP_INFO_s *pMirrorInfo)
{
    UINT32 RetVal;

    if ((FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM) && (pMirrorInfo != NULL)) {
        RetVal = AmbaWrap_memcpy(pMirrorInfo, &SvcViewCtrlWarpInfo[FovIdx], sizeof(SVC_VIEW_CTRL_WARP_INFO_s));
        if (SVC_OK != RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "SvcViewCtrl_GetViewCtrlInfo() failed. AmbaWrap_memcpy() err %d", RetVal, 0U);
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* get window of view control
* @param [in] FovIdx index of fov
* @param [out] pWindowInfo window of view control
* @return 0-OK, 1-NG
*/
UINT32 SvcViewCtrl_GetViewWinInfo(UINT32 FovIdx, SVC_VIEW_CTRL_WINDOW_s *pWindowInfo)
{
    UINT32 RetVal;

    if ((FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM) && (pWindowInfo != NULL)) {
        DOUBLE RawWidthInDouble  = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth;
        DOUBLE RawHeightInDouble = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight;
        DOUBLE WinWidthInDouble;
        DOUBLE WinHeightInDouble;
        DOUBLE WinOffsetXInDouble;
        DOUBLE WinOffsetYInDouble;

        WinWidthInDouble  = RawWidthInDouble / (SvcViewCtrlDefaultInfo[FovIdx].ZoomX + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX);
        WinHeightInDouble = RawHeightInDouble / (SvcViewCtrlDefaultInfo[FovIdx].ZoomY + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY);
        WinOffsetXInDouble = ((RawWidthInDouble - WinWidthInDouble) / 2.0) + ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX);
        WinOffsetYInDouble = ((RawHeightInDouble - WinHeightInDouble) / 2.0) + ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY);

        pWindowInfo->Width   = (UINT16) WinWidthInDouble;
        pWindowInfo->Height  = (UINT16) WinHeightInDouble;
        pWindowInfo->OffsetX = (UINT16) WinOffsetXInDouble;
        pWindowInfo->OffsetY = (UINT16) WinOffsetYInDouble;

        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* get preview window of view control
* @param [in] VoutID id of vout
* @param [in] FovIdx index of fov
* @param [out] pWindowInfo window of view control
* @return 0-OK, 1-NG
*/
UINT32 SvcViewCtrl_GetPrevWinInfo(UINT32 VoutID, UINT32 FovIdx, SVC_VIEW_CTRL_WINDOW_s *pWindowInfo)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, j;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if ((VoutID < 2U) && (FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM) && (pWindowInfo != NULL)) {
        /* Update SvcResCfg */
        for (i = 0; i < pResCfg->DispNum; i++) {
            /* Check whether VoutID matches */
            if (pResCfg->DispStrm[i].VoutID == VoutID) {
                for (j = 0; j < pResCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                    /* Check which channel contains the desired FovIdx */
                    if (pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId == FovIdx) {
                        pWindowInfo->OffsetX = pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.OffsetX;
                        pWindowInfo->OffsetY = pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.OffsetY;
                        pWindowInfo->Width   = pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.Width;
                        pWindowInfo->Height  = pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.Height;
                        break;
                    }
                }
            }
        }
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 ViewCtrl_RawDataReady(const void *pEventInfo)
{
    static UINT32 ViewCtrlRawVinBits = 0U;
    static UINT32 ViewCtrlTargetVinBits = 0U;
    UINT32 VinNum = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM] = {0U}, VinID = 0U, VinIdx = 0U;
    const AMBA_DSP_RAW_DATA_RDY_s *pRawData;

    AmbaMisra_TypeCast(&pRawData, &pEventInfo);

    if (ViewCtrlTargetVinBits == 0U) {
        if (SVC_OK == SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
            for (VinIdx = 0U; VinIdx < VinNum; VinIdx++) {
                VinID = VinIDs[VinIdx];
                ViewCtrlTargetVinBits |= ((UINT32) 1U) << VinID;
            }
        }
    }

    ViewCtrlRawVinBits |= ((UINT32) 1U) << pRawData->VinId;

    if (ViewCtrlTargetVinBits == ViewCtrlRawVinBits) {
        SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg;

        MirrorCmdMsg.Cmd = SVC_VIEW_CTRL_CMD_VALIDATE;
        MirrorCmdMsg.VinID = (UINT8) pRawData->VinId;
        SvcViewCtrl_Exec(MirrorCmdMsg);

        ViewCtrlTargetVinBits = 0U;
        ViewCtrlRawVinBits = 0U;
    }

    return SVC_OK;
}

static UINT32 ViewCtrl_TaskCreate(void)
{
    UINT32 Rval = SVC_OK, RetVal, Length;
    UINT32 VinIdxs[AMBA_DSP_MAX_VIN_NUM] = {0U}, VinNum = 0U, VinIdx = 0U;
    UINT32 i;

    RetVal = SvcResCfg_GetVinIDs(VinIdxs, &VinNum);

    if (SVC_OK == RetVal) {
        for (i = 0; i < VinNum; i++) {
            VinIdx = VinIdxs[i];

            if (SVC_OK == RetVal) {
                if (SvcViewCtrlTaskInfo[VinIdx].TaskCreated != 0U) {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "MirrorHandler Vin(%d) Task has been created", VinIdx, 0U);
                    RetVal = SVC_NG;
                }
            }

            /* Create Queue for ViewCtrl_TaskEntry */
            if (SVC_OK == RetVal) {
                Length = SvcWrap_sprintfU32(SvcViewCtrlInfo[VinIdx].QueueName, 32, "SvcViewCtrlQueue_%d", 1, &VinIdx);
                if (Length <= 0U) {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskCreate() err. SvcWrap queue name length", Length, 0U);
                    RetVal = SVC_NG;
                }
            }
            if (SVC_OK == RetVal) {
                RetVal = AmbaKAL_MsgQueueCreate(&SvcViewCtrlInfo[VinIdx].QueueId,
                                                SvcViewCtrlInfo[VinIdx].QueueName,
                                                (UINT32) sizeof(SVC_VIEW_CTRL_CMD_MSG_s),
                                                SvcViewCtrlInfo[VinIdx].Queue,
                                                (UINT32) SVC_VIEW_CTRL_QUEUE_NUM * (UINT32) sizeof(SVC_VIEW_CTRL_CMD_MSG_s));
                if (SVC_OK != RetVal) {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskCreate() err. Queue create fail %d", RetVal, 0U);
                }
            }

            /* Create ViewCtrl_TaskEntry */
            if (SVC_OK == RetVal) {
                Length = SvcWrap_sprintfU32(SvcViewCtrlTaskInfo[VinIdx].TaskName, 32, "MirrorTask_%d", 1, &VinIdx);
                if (Length <= 0U) {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskCreate() err. SvcWrap task name length %d", Length, 0U);
                    RetVal = SVC_NG;
                } else {
                    SvcViewCtrlTaskInfo[VinIdx].Priority  = 63U;
                    SvcViewCtrlTaskInfo[VinIdx].EntryArg  = VinIdx;
                    SvcViewCtrlTaskInfo[VinIdx].StackSize = SVC_VIEW_CTRL_TASK_STACK_SIZE;
                }
            }
            if (SVC_OK == RetVal) {
                RetVal = AmbaKAL_TaskCreate(&SvcViewCtrlTaskInfo[VinIdx].TaskCtrl,
                                             SvcViewCtrlTaskInfo[VinIdx].TaskName,
                                             SvcViewCtrlTaskInfo[VinIdx].Priority,
                                             ViewCtrl_TaskEntry,
                                             &(SvcViewCtrlTaskInfo[VinIdx].EntryArg),
                                             SvcViewCtrlTaskInfo[VinIdx].Stack,
                                             SvcViewCtrlTaskInfo[VinIdx].StackSize,
                                             AMBA_KAL_AUTO_START);
                if (SVC_OK != RetVal) {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskCreate() err. Task create failed %d", RetVal, 0U);
                }
            }

            if (SVC_OK == RetVal) {
                SvcViewCtrlTaskInfo[VinIdx].TaskCreated = 1;
            }
            Rval += RetVal;
        }
    } else {
        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskCreate() err. SvcResCfg_GetVinIDs() failed %d", RetVal, 0U);
    }

    return Rval;
}

static UINT32 ViewCtrl_TaskDelete(void)
{
    UINT32 Rval = 0, RetVal;
    UINT32 VinIdxs[AMBA_DSP_MAX_VIN_NUM] = {0U}, VinNum = 0U, VinIdx = 0U;
    UINT32 i;

    RetVal = SvcResCfg_GetVinIDs(VinIdxs, &VinNum);

    for (i = 0; i < VinNum; i++) {
        VinIdx = VinIdxs[i];
        if (SvcViewCtrlTaskInfo[VinIdx].TaskCreated == 0U) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "MirrorHandler Vin(%d) Task never created", VinIdx, 0U);
            RetVal = SVC_NG;
        } else {
            RetVal = SVC_OK;
        }

        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_TaskTerminate(&SvcViewCtrlTaskInfo[VinIdx].TaskCtrl);
            if (SVC_OK != RetVal) {
                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry terminate fail %d", RetVal, 0);
            }
        }

        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_TaskDelete(&SvcViewCtrlTaskInfo[VinIdx].TaskCtrl);
            if (SVC_OK != RetVal) {
                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry delete fail %d", RetVal, 0);
            }
        }

        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_MsgQueueDelete(&SvcViewCtrlInfo[VinIdx].QueueId);
            if (SVC_OK != RetVal) {
                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "MirrorHandlerQueue delete fail %d", RetVal, 0);
            }
        }

        if (SVC_OK == RetVal) {
            SvcViewCtrlTaskInfo[VinIdx].TaskCreated = 0U;
        }
        Rval += RetVal;
    }

    return Rval;
}

static UINT32 ViewCtrl_Validate(void)
{
    if (SvcViewCtrlValidate_Config == 1U) {
        SvcLiveview_Update();
        SvcViewCtrlValidate_Config = 0U;
    } else {
        /* Do nothing */
    }

    if (SvcViewCtrlValidate_YuvStream == 1U) {
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT32 StrmNumMax = pResCfg->DispNum + pResCfg->RecNum;
        UINT32 i;

        for (i = 0U; i < StrmNumMax; i++) {
            SvcLiveview_UpdateStream(i);
        }
        SvcViewCtrlValidate_YuvStream = 0U;

    } else {
        /* Do nothing */
    }

    return SVC_OK;
}

static UINT32 ViewCtrl_Restore(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal;
    UINT32 FovIdx = pImgMode->ContextId;

    RetVal = AmbaWrap_memset(&SvcViewCtrlWarpInfo[FovIdx], 0, sizeof(SVC_VIEW_CTRL_WARP_INFO_s));
    RetVal |= AmbaWrap_memcpy(&SvcViewCtrlWarpInfo[FovIdx].Warp, &SvcViewCtrlDefaultInfo[FovIdx].Warp, sizeof(AMBA_IK_WARP_INFO_s));

    if (SVC_OK == RetVal) {
        RetVal = ViewCtrl_PanTilt(pImgMode);
        if (SVC_OK == RetVal) {
            RetVal = ViewCtrl_Warp(pImgMode);
            if (SVC_NG == RetVal) {
                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_Restore() error since ViewCtrl_Warp() failed(%d)", RetVal, 0U);
            }
        } else {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_Restore() error since ViewCtrl_PanTilt() failed(%d)", RetVal, 0U);
        }
    }

    return RetVal;
}

static UINT32 ViewCtrl_PanTilt(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FovIdx = pImgMode->ContextId;
    AMBA_IK_DZOOM_INFO_s DzoomInfo;

    DOUBLE ZoomXRatio, ZoomYRatio;

    SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    /* Use Dzoom to crop desired location and Fov */
    {
        ZoomXRatio = (SvcViewCtrlDefaultInfo[FovIdx].ZoomX + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX) * (DOUBLE) 65536;
        ZoomYRatio = (SvcViewCtrlDefaultInfo[FovIdx].ZoomY + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY) * (DOUBLE) 65536;
        DzoomInfo.Enable = 1U;
        DzoomInfo.ShiftX = (SvcViewCtrlDefaultInfo[FovIdx].ShiftX + SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX) * 65536;
        DzoomInfo.ShiftY = (SvcViewCtrlDefaultInfo[FovIdx].ShiftY + SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY) * 65536;
        DzoomInfo.ZoomX  = (UINT32) ZoomXRatio;
        DzoomInfo.ZoomY  = (UINT32) ZoomYRatio;
    }

    /* Update SvcResCfg */
    {
        if (FovIdx < AMBA_DSP_MAX_VIEWZONE_NUM) {
            DOUBLE ActWinX, ActWinY, ActWinW, ActWinH;
            ActWinW = (DOUBLE) pResCfg->FovCfg[FovIdx].RawWin.Width / (SvcViewCtrlDefaultInfo[FovIdx].ZoomX + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX);
            ActWinH = (DOUBLE) pResCfg->FovCfg[FovIdx].RawWin.Height / (SvcViewCtrlDefaultInfo[FovIdx].ZoomY + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY);
            ActWinX = (((DOUBLE) pResCfg->FovCfg[FovIdx].RawWin.Width - ActWinW) / 2.0) + ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX);
            ActWinY = (((DOUBLE) pResCfg->FovCfg[FovIdx].RawWin.Height - ActWinH) / 2.0) + ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY);

            pResCfg->FovCfg[FovIdx].ActWin.OffsetX = (UINT16) ActWinX;
            pResCfg->FovCfg[FovIdx].ActWin.OffsetY = (UINT16) ActWinY;
            pResCfg->FovCfg[FovIdx].ActWin.Width   = (UINT16) ActWinW;
            pResCfg->FovCfg[FovIdx].ActWin.Height  = (UINT16) ActWinH;
        }
    }

    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "DzoomInfo.Enable = %d", DzoomInfo.Enable, 0U);
    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "DzoomInfo.ShiftX = %d", (UINT32) DzoomInfo.ShiftX, 0U);
    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "DzoomInfo.ShiftY = %d", (UINT32) DzoomInfo.ShiftY, 0U);
    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "DzoomInfo.ZoomX  = %d", DzoomInfo.ZoomX,  0U);
    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "DzoomInfo.ZoomY  = %d", DzoomInfo.ZoomY,  0U);

    if (pImgMode->ContextId < AMBA_DSP_MAX_VIEWZONE_NUM) {
        RetVal = SvcIK_ImgDzoomCtrl(pImgMode, &DzoomInfo);  // SVC wrapped API. API will execute Dzoom ctrl one time only
        if (SVC_NG == RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "Mirror_PanTilt() error since SvcIK_ImgDzoomCtrl() failed(%d)", RetVal, 0U);
        }
    }

    return RetVal;
}

static UINT32 ViewCtrl_Warp(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetVal;
    UINT32 FovIdx = pImgMode->ContextId;
    static AMBA_WU_WARP_ROTATE_IN_s WarpRotateIn;
    static AMBA_WU_WARP_ROTATE_RESULT_s WarpRotateResult;

    static AMBA_IK_GRID_POINT_s ResultWarpTable[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_WARP_TBL_LEN] GNU_SECTION_NOZEROINIT;

    /* Prepare rotation calculation resource */
    WarpRotateIn.Theta = SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta * VIEW_CTRL_PI / 180.0;
    WarpRotateIn.CenterX = (UINT32) (SvcViewCtrlWarpInfo[FovIdx].Warp.HorGridNum * ((1UL << SvcViewCtrlWarpInfo[FovIdx].Warp.TileWidthExp))) >> 1U;
    WarpRotateIn.CenterY = (UINT32) (SvcViewCtrlWarpInfo[FovIdx].Warp.VerGridNum * ((1UL << SvcViewCtrlWarpInfo[FovIdx].Warp.TileHeightExp))) >> 1U;
    RetVal = AmbaWrap_memcpy(&WarpRotateIn.InputInfo, &SvcViewCtrlWarpInfo[FovIdx].Warp, sizeof(AMBA_IK_WARP_INFO_s));

    if (SVC_OK == RetVal) {
        RetVal = AmbaWrap_memcpy(&WarpRotateResult.ResultInfo, &WarpRotateIn.InputInfo, sizeof(AMBA_IK_WARP_INFO_s));
        if (SVC_OK == RetVal) {
            WarpRotateResult.ResultInfo.pWarp = ResultWarpTable[FovIdx];
        } else {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_Warp() err. memcpy from WarpIn to WarpResult failed %d", RetVal, 0U);
        }
    } else {
        ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_Warp() err. Window info get failed %d", RetVal, 0U);
    }


    /* calculation and apply */
    if (SVC_OK == RetVal) {
        RetVal = AmbaWU_WarpRotate(&WarpRotateIn, &WarpRotateResult);
        if (SVC_OK == RetVal) {
            /* Set default warp table (warp table by default all 0) */
            RetVal = AmbaIK_SetWarpEnb(pImgMode, 1U);
            if (SVC_OK == RetVal) {
                RetVal = AmbaIK_SetWarpInfo(pImgMode, &WarpRotateResult.ResultInfo);
                if (SVC_OK == RetVal) {
                    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_Warp() succeed.", 0U, 0U);
                } else {
                    ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_Warp() err. Warp info set failed %d", RetVal, 0U);
                }
            } else {
                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_Warp() err. Warp margin set failed", 0U, 0U);
            }
        } else {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_Warp() err. Warp rotate calculation failed %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        /* Print  */
        SVC_WRAP_PRINT "  Current Theta %3.1f  "
            SVC_PRN_ARG_S SVC_LOG_VIEW_CTRL
            SVC_PRN_ARG_DOUBLE SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta
            SVC_PRN_ARG_E
    }

    return RetVal;
}

static UINT32 ViewCtrl_PrevSetSrcWin(UINT32 VoutID, UINT32 FovIdx)
{
    const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pStrmCfgArr;
    SVC_LIV_INFO_s LivInfo;
    UINT32 NumStrm;
    UINT32 i, j;

    SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    SvcLiveview_InfoGet(&LivInfo);
    pStrmCfgArr = LivInfo.pStrmCfg;
    NumStrm = *LivInfo.pNumStrm;

    /* Update Liveview configuration */
    for (i = 0; i < NumStrm; i++) {
        /* Check existing stream configuration is for VOUT */
        if ((pStrmCfgArr[i].Purpose & SVC_LIV_PURPOSE_VOUT) > 0U) {
            /* Check existing VOUT stream is the desired VoutID */
            if ((pStrmCfgArr[i].DestVout & ((UINT32) 1U << VoutID)) > 0U) {
                for (j = 0; j < pStrmCfgArr[i].NumChan; j++) {
                    /* Check which channel contains the desired FovIdx */
                    if (pStrmCfgArr[i].pChanCfg[j].ViewZoneId == FovIdx) {
                        pStrmCfgArr[i].pChanCfg[j].ROI.OffsetX = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX;
                        pStrmCfgArr[i].pChanCfg[j].ROI.OffsetY = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY;
                        pStrmCfgArr[i].pChanCfg[j].ROI.Width   = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width;
                        pStrmCfgArr[i].pChanCfg[j].ROI.Height  = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height;
                        ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_PrevSetSrcWin() succeed %d %d", VoutID, FovIdx);
                    }
                }
            }
        }
    }

    /* Update SvcResCfg */
    for (i = 0; i < pResCfg->DispNum; i++) {
        /* Check whether VoutID matches */
        if (pResCfg->DispStrm[i].VoutID == VoutID) {
            for (j = 0; j < pResCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                /* Check which channel contains the desired FovIdx */
                if (pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId == FovIdx) {
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.OffsetX = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX;
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.OffsetY = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY;
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.Width   = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width;
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].SrcWin.Height  = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height;
                }
            }
        }
    }

    SvcViewCtrlValidate_Config = 1U;

    return SVC_OK;
}

static UINT32 ViewCtrl_PrevSetDstWin(UINT32 VoutID, UINT32 FovIdx)
{
    const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pStrmCfgArr;
    SVC_LIV_INFO_s LivInfo;
    UINT32 NumStrm;
    UINT32 i, j;

    SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    SvcLiveview_InfoGet(&LivInfo);
    pStrmCfgArr = LivInfo.pStrmCfg;
    NumStrm = *LivInfo.pNumStrm;

    /* Update Liveview configuration */
    for (i = 0; i < NumStrm; i++) {
        /* Check existing stream configuration is for VOUT */
        if ((pStrmCfgArr[i].Purpose & SVC_LIV_PURPOSE_VOUT) > 0U) {
            /* Check existing VOUT stream is the desired VoutID */
            if ((pStrmCfgArr[i].DestVout & ((UINT32) 1U << VoutID)) > 0U) {
                for (j = 0; j < pStrmCfgArr[i].NumChan; j++) {
                    /* Check which channel contains the desired FovIdx */
                    if (pStrmCfgArr[i].pChanCfg[j].ViewZoneId == FovIdx) {
                        pStrmCfgArr[i].pChanCfg[j].Window.OffsetX = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.OffsetX;
                        pStrmCfgArr[i].pChanCfg[j].Window.OffsetY = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.OffsetY;
                        pStrmCfgArr[i].pChanCfg[j].Window.Width   = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.Width;
                        pStrmCfgArr[i].pChanCfg[j].Window.Height  = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.Height;
                        ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_PrevSetDstWin() succeed %d %d", VoutID, FovIdx);
                    }
                }
            }
        }
    }

    /* Update SvcResCfg */
    for (i = 0; i < pResCfg->DispNum; i++) {
        /* Check whether VoutID matches */
        if (pResCfg->DispStrm[i].VoutID == VoutID) {
            for (j = 0; j < pResCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                /* Check which channel contains the desired FovIdx */
                if (pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId == FovIdx) {
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.OffsetX = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.OffsetX;
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.OffsetY = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.OffsetY;
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Width   = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.Width;
                    pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Height  = SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.Height;
                }
            }
        }
    }

    SvcViewCtrlValidate_YuvStream = 1U;

    return SVC_OK;
}

static void* ViewCtrl_TaskEntry(void* EntryArg)
{
    const UINT32  *pArg;
    UINT32 RetVal;
    UINT32 VinIdx, FovIdx, VoutID;
    SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg;
    AMBA_IK_MODE_CFG_s ImgMode;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    VinIdx = (*pArg);

    while (VinIdx < AMBA_DSP_MAX_VIN_NUM) {
        RetVal = AmbaKAL_MsgQueueReceive(&SvcViewCtrlInfo[VinIdx].QueueId, &MirrorCmdMsg, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK != RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry() Receive Warp Msg failed %d", RetVal, 0U);
            continue;
        }

        FovIdx = (UINT32) MirrorCmdMsg.FovIdx;
        VoutID = (UINT32) MirrorCmdMsg.VoutID;
        ImgMode.ContextId = FovIdx;

        switch (MirrorCmdMsg.Cmd) {
            case SVC_VIEW_CTRL_WARP_CMD_PAN:
            case SVC_VIEW_CTRL_WARP_CMD_TILT:
            case SVC_VIEW_CTRL_WARP_CMD_ZOOM:
            case SVC_VIEW_CTRL_WARP_CMD_SET_WIN:
                RetVal = ViewCtrl_InfoUpdate(MirrorCmdMsg);
                if (SVC_OK == RetVal) {
                    RetVal = ViewCtrl_PanTilt(&ImgMode);
                } else {
                    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry() info doesn't update", 0U, 0U);
                    RetVal = SVC_OK;
                }
            break;
            case SVC_VIEW_CTRL_WARP_CMD_ROTATE:
            case SVC_VIEW_CTRL_WARP_CMD_SET_WARP:
                RetVal = ViewCtrl_InfoUpdate(MirrorCmdMsg);
                if (SVC_OK == RetVal) {
                    RetVal = ViewCtrl_Warp(&ImgMode);
                } else {
                    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry() info doesn't update", 0U, 0U);
                    RetVal = SVC_OK;
                }
            break;
            case SVC_VIEW_CTRL_PREV_CMD_PAN:
            case SVC_VIEW_CTRL_PREV_CMD_TILT:
            case SVC_VIEW_CTRL_PREV_CMD_ZOOM:
            case SVC_VIEW_CTRL_PREV_CMD_SET_SRCWIN:
                RetVal = ViewCtrl_InfoUpdate(MirrorCmdMsg);
                if (SVC_OK == RetVal) {
                    RetVal = ViewCtrl_PrevSetSrcWin(VoutID, FovIdx);
                } else {
                    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry() info doesn't update", 0U, 0U);
                    RetVal = SVC_OK;
                }
            break;
            case SVC_VIEW_CTRL_PREV_CMD_SET_DSTWIN:
                RetVal = ViewCtrl_InfoUpdate(MirrorCmdMsg);
                if (SVC_OK == RetVal) {
                    RetVal = ViewCtrl_PrevSetDstWin(VoutID, FovIdx);
                } else {
                    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry() info doesn't update", 0U, 0U);
                    RetVal = SVC_OK;
                }
            break;
            case SVC_VIEW_CTRL_CMD_RESTORE:
                RetVal = ViewCtrl_Restore(&ImgMode);
            break;
            case SVC_VIEW_CTRL_CMD_VALIDATE:
                RetVal = ViewCtrl_Validate();
            break;
            default:
                ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry() err. Non support cmd/operation (%d)", MirrorCmdMsg.Cmd, 0U);
                RetVal = SVC_NG;
            break;
        }

        if (SVC_OK != RetVal) {
            ViewCtrl_Err(SVC_LOG_VIEW_CTRL, "ViewCtrl_TaskEntry() err. Execution err", 0U, 0U);
        }
    }

    return NULL;
}

static UINT32 ViewCtrl_InfoUpdate(SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg)
{
    UINT8 FovIdx = MirrorCmdMsg.FovIdx;
    UINT32 VoutID = MirrorCmdMsg.VoutID;

    UINT32 RetVal;
    DOUBLE NewFovWidthInDouble, NewFovHeightInDouble;
    DOUBLE NewPrevWidthInDouble, NewPrevHeightInDouble;

    DOUBLE DesiredShiftX, DesiredShiftY;
    DOUBLE DesiredZoomX, DesiredZoomY;
    DOUBLE NewZoomXInDouble, NewZoomYInDouble;
    DOUBLE NewShiftXInDouble, NewShiftYInDouble;

    /* To print zoom info */
    UINT32 Zoom1000, ZoomInteger, ZoomFractional;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    RetVal = ViewCtrl_InfoValidCheck(MirrorCmdMsg);
    if (SVC_OK == RetVal) {
        switch (MirrorCmdMsg.Cmd) {
            case SVC_VIEW_CTRL_WARP_CMD_PAN:
                SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX += MirrorCmdMsg.Value;
            break;
            case SVC_VIEW_CTRL_WARP_CMD_TILT:
                SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY += MirrorCmdMsg.Value;
            break;
            case SVC_VIEW_CTRL_WARP_CMD_ZOOM:
                DesiredZoomX = SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX + ((DOUBLE) MirrorCmdMsg.Value / (DOUBLE) 1000);
                DesiredZoomY = SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY + ((DOUBLE) MirrorCmdMsg.Value / (DOUBLE) 1000);
                NewFovWidthInDouble  = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomX + DesiredZoomX));
                NewFovHeightInDouble = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomY + DesiredZoomY));
                SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX = DesiredZoomX;
                SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY = DesiredZoomY;
                /* Zoom out */
                if (MirrorCmdMsg.Value < 0) {
                    DOUBLE RightPositionX, LeftPositionX, DownPositionY, UpPositionY;
                    DOUBLE NewShiftInDouble;

                    RightPositionX = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX + ((DOUBLE) 0.5 * NewFovWidthInDouble);
                    LeftPositionX  = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX - ((DOUBLE) 0.5 * NewFovWidthInDouble);
                    DownPositionY  = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY + ((DOUBLE) 0.5 * NewFovHeightInDouble);
                    UpPositionY    = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY - ((DOUBLE) 0.5 * NewFovHeightInDouble);

                    if (RightPositionX > ((DOUBLE) 0.5 * (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth)) {
                        NewShiftInDouble = ((DOUBLE) 0.5 * ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth - NewFovWidthInDouble)) - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX;
                        SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX = (INT32) NewShiftInDouble;
                    }
                    if (LeftPositionX < ((DOUBLE) -0.5 * (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth)) {
                        NewShiftInDouble = ((DOUBLE) -0.5 * ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth - NewFovWidthInDouble)) - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX;
                        SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX = (INT32) NewShiftInDouble;
                    }
                    if (DownPositionY > ((DOUBLE) 0.5 * (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight)) {
                        NewShiftInDouble = ((DOUBLE) 0.5 * ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight - NewFovHeightInDouble)) - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY;
                        SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY = (INT32) NewShiftInDouble;
                    }
                    if (UpPositionY < ((DOUBLE) -0.5 * (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight)) {
                        NewShiftInDouble = ((DOUBLE) -0.5 * ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight - NewFovHeightInDouble)) - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY;
                        SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY = (INT32) NewShiftInDouble;
                    }
                }

                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "User Zoom Info, ", 0U, 0U);
                DesiredZoomX   = DesiredZoomX * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomX;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "                X:%d.%03d", ZoomInteger, ZoomFractional);
                DesiredZoomY   = DesiredZoomY * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomY;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "                Y:%d.%03d", ZoomInteger, ZoomFractional);
            break;
            case SVC_VIEW_CTRL_WARP_CMD_ROTATE:
                SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta += (DOUBLE) ((DOUBLE) MirrorCmdMsg.Value / (DOUBLE) 10);
            break;
            case SVC_VIEW_CTRL_WARP_CMD_SET_WARP:
                {
                    AMBA_IK_GRID_POINT_s  *pPtr;

                    AmbaMisra_TypeCast(&pPtr, &(MirrorCmdMsg.Value));
                    SvcViewCtrlWarpInfo[FovIdx].Warp.pWarp = pPtr;
                }
            break;
            case SVC_VIEW_CTRL_WARP_CMD_SET_WIN:
                DesiredShiftX = (DOUBLE) MirrorCmdMsg.Reserved[0] - (((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth - (DOUBLE) MirrorCmdMsg.Reserved[2]) / 2.0);
                DesiredShiftY = (DOUBLE) MirrorCmdMsg.Reserved[1] - (((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight - (DOUBLE) MirrorCmdMsg.Reserved[3]) / 2.0);
                DesiredZoomX  = (DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].RawWidth) / (DOUBLE) (MirrorCmdMsg.Reserved[2]);
                DesiredZoomY  = (DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].RawHeight) / (DOUBLE) (MirrorCmdMsg.Reserved[3]);
                NewShiftXInDouble = (DesiredShiftX - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX);
                NewShiftYInDouble = (DesiredShiftY - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY);
                NewZoomXInDouble  = (DesiredZoomX - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ZoomX);
                NewZoomYInDouble  = (DesiredZoomY - (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ZoomY);
                SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX = (INT32) NewShiftXInDouble;
                SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY = (INT32) NewShiftYInDouble;
                SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX  = NewZoomXInDouble;
                SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY  = NewZoomYInDouble;
            break;
            case SVC_VIEW_CTRL_PREV_CMD_PAN:
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX += (UINT16) MirrorCmdMsg.Value;
            break;
            case SVC_VIEW_CTRL_PREV_CMD_TILT:
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY += (UINT16) MirrorCmdMsg.Value;
            break;
            case SVC_VIEW_CTRL_PREV_CMD_ZOOM:
                DesiredZoomX = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomX + ((DOUBLE) MirrorCmdMsg.Value / (DOUBLE) 1000);
                DesiredZoomY = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomY + ((DOUBLE) MirrorCmdMsg.Value / (DOUBLE) 1000);

                NewPrevWidthInDouble  = (DOUBLE) (SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Width) / DesiredZoomX;
                NewPrevHeightInDouble = (DOUBLE) (SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Height) / DesiredZoomY;
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width  = (UINT16) NewPrevWidthInDouble;
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height = (UINT16) NewPrevHeightInDouble;
                if (MirrorCmdMsg.Value < 0) {
                    if ((SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX + SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width) > pCfg->FovCfg[FovIdx].MainWin.Width) {
                        SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX = pCfg->FovCfg[FovIdx].MainWin.Width - SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width;
                    }
                    if ((SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY + SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height) > pCfg->FovCfg[FovIdx].MainWin.Height) {
                        SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY = pCfg->FovCfg[FovIdx].MainWin.Height - SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height;
                    }
                }
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomX = DesiredZoomX;
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomY = DesiredZoomY;

                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "User Zoom Info, ", 0U, 0U);
                DesiredZoomX   = DesiredZoomX * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomX;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "                X:%d.%03d", ZoomInteger, ZoomFractional);
                DesiredZoomY   = DesiredZoomY * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomY;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "                Y:%d.%03d", ZoomInteger, ZoomFractional);
                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "Calc Zoom Info, ", 0U, 0U);
                DesiredZoomX   = (DOUBLE) SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Width / (DOUBLE) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomX;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "                X:%d.%03d", ZoomInteger, ZoomFractional);
                DesiredZoomY   = (DOUBLE) SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Height / (DOUBLE) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomY;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_DbgLvl(VIEW_CTRL_DBG_LVL_2, SVC_LOG_VIEW_CTRL, "                Y:%d.%03d", ZoomInteger, ZoomFractional);
            break;
            case SVC_VIEW_CTRL_PREV_CMD_SET_SRCWIN:
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX = (UINT16) MirrorCmdMsg.Reserved[0U];
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY = (UINT16) MirrorCmdMsg.Reserved[1U];
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width   = (UINT16) MirrorCmdMsg.Reserved[2U];
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height  = (UINT16) MirrorCmdMsg.Reserved[3U];
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomX   = (DOUBLE) SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Width / (DOUBLE) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width;
                SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomY   = (DOUBLE) SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Height / (DOUBLE) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height;
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Update Zoom Info,", 0U, 0U);
                DesiredZoomX   = (DOUBLE) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomX * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomX;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "                  X:%d.%03d", ZoomInteger, ZoomFractional);
                DesiredZoomY   = (DOUBLE) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomY * 1000.0;
                Zoom1000       = (UINT32) DesiredZoomY;
                ZoomInteger    = Zoom1000 / 1000U;
                ZoomFractional = Zoom1000 - (ZoomInteger * 1000U);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "                  Y:%d.%03d", ZoomInteger, ZoomFractional);
            break;
            case SVC_VIEW_CTRL_PREV_CMD_SET_DSTWIN:
                SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.OffsetX = (UINT16) MirrorCmdMsg.Reserved[0U];
                SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.OffsetY = (UINT16) MirrorCmdMsg.Reserved[1U];
                SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.Width   = (UINT16) MirrorCmdMsg.Reserved[2U];
                SvcViewCtrlPrevInfo[VoutID][FovIdx].DstPosition.Height  = (UINT16) MirrorCmdMsg.Reserved[3U];
            break;
            default:
                RetVal = SVC_NG;
            break;
        }
    }

    return RetVal;

}

static UINT32 ViewCtrl_InfoValidCheck(SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg)
{
    UINT32 RetVal;
    UINT8  VinIdx = MirrorCmdMsg.VinID;
    UINT8  FovIdx = MirrorCmdMsg.FovIdx;
    UINT8  VoutID = MirrorCmdMsg.VoutID;
    UINT32 NewFovWidth, NewFovHeight;
    DOUBLE NewFovWidthInDouble, NewFovHeightInDouble;
    UINT32 NewPrevWidth, NewPrevHeight;
    DOUBLE NewPrevWidthInDouble, NewPrevHeightInDouble;
    /* Calculation for pan/tilt */
    INT32  MinShift, MaxShift;
    DOUBLE MinShiftInDouble, MaxShiftInDouble;
    INT32  MaxValueToShift, MinValueToShift;
    /* Calculation for zoom */
    UINT32 ZoomInteger, ZoomFractional;
    UINT32 Zoom1000, ZoomInteger1000;
    DOUBLE Zoom1000InDouble, NewZoomInDouble;
    UINT32 ZoomMaxInteger, ZoomMaxFractional;
    UINT32 ZoomMax1000, ZoomMaxInteger1000;
    DOUBLE ZoomMax1000InDouble;
    /* Calculation for rotation */
    UINT32 ThetaInteger, ThetaFractional;
    UINT32 Theta10, ThetaInteger10;
    DOUBLE Theta10InDouble;
    // DOUBLE TanTheta;
    DOUBLE CosTheta = 0.0;
    DOUBLE SinTheta = 0.0;
    DOUBLE ThetaInRadius;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    if (MirrorCmdMsg.Cmd < SVC_VIEW_CTRL_CMD_NUM) {
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "ViewCtrl_InfoValidCheck: VinIdx = %d, FovIdx = %d", VinIdx, FovIdx);

    switch (MirrorCmdMsg.Cmd) {
        case SVC_VIEW_CTRL_WARP_CMD_PAN:
            NewFovWidthInDouble  = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomX + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX));
            MinShiftInDouble = -0.5 * ((DOUBLE)SvcViewCtrlDefaultInfo[FovIdx].RawWidth - NewFovWidthInDouble);
            MaxShiftInDouble = 0.5 * ((DOUBLE)SvcViewCtrlDefaultInfo[FovIdx].RawWidth - NewFovWidthInDouble);
            MinShift = (INT32)MinShiftInDouble;
            MaxShift = (INT32)MaxShiftInDouble;
            MinValueToShift = (MinShift - SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX) - SvcViewCtrlDefaultInfo[FovIdx].ShiftX;
            MaxValueToShift = (MaxShift - SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX) - SvcViewCtrlDefaultInfo[FovIdx].ShiftX;

            if (MirrorCmdMsg.Value > MaxValueToShift) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! MaxValueToShift should be smaller than %d.", (UINT32)MaxValueToShift, 0U);
                RetVal = SVC_NG;
            } else if (MirrorCmdMsg.Value < MinValueToShift) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! MinValueToShift should be bigger than -%d.", (UINT32)-MinValueToShift, 0U);
                RetVal = SVC_NG;
            } else {
                RetVal = SVC_OK;
            }
        break;
        case SVC_VIEW_CTRL_WARP_CMD_TILT:
            NewFovHeightInDouble = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomY + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY));
            MinShiftInDouble = -0.5 * ((DOUBLE)SvcViewCtrlDefaultInfo[FovIdx].RawHeight - NewFovHeightInDouble);
            MaxShiftInDouble = 0.5 * ((DOUBLE)SvcViewCtrlDefaultInfo[FovIdx].RawHeight - NewFovHeightInDouble);
            MinShift = (INT32)MinShiftInDouble;
            MaxShift = (INT32)MaxShiftInDouble;
            MinValueToShift = (MinShift - SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY) - SvcViewCtrlDefaultInfo[FovIdx].ShiftY;
            MaxValueToShift = (MaxShift - SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY) - SvcViewCtrlDefaultInfo[FovIdx].ShiftY;

            if (MirrorCmdMsg.Value > MaxValueToShift) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! MaxValueToShift should be smaller than %d.", (UINT32)MaxValueToShift, 0U);
                RetVal = SVC_NG;
            } else if (MirrorCmdMsg.Value < MinValueToShift) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! MinValueToShift should be bigger than -%d.", (UINT32)-MinValueToShift, 0U);
                RetVal = SVC_NG;
            } else {
                RetVal = SVC_OK;
            }
        break;
        case SVC_VIEW_CTRL_WARP_CMD_ZOOM:
            ZoomMax1000InDouble = (DOUBLE) VIEW_CTRL_ZOOM_RATIO_MAX * 1000.0;
            ZoomMax1000 = (UINT32) ZoomMax1000InDouble;
            ZoomMaxInteger = ZoomMax1000 / 1000U;
            ZoomMaxInteger1000 = ZoomMaxInteger * 1000U;
            ZoomMaxFractional = ZoomMax1000 - ZoomMaxInteger1000;

            Zoom1000InDouble = SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX * 1000.0;
            Zoom1000 = (UINT32) Zoom1000InDouble;
            ZoomInteger = (UINT32) SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX;
            ZoomInteger1000 = ZoomInteger * 1000U;
            ZoomFractional = Zoom1000 - ZoomInteger1000;

            NewZoomInDouble = (Zoom1000InDouble + (DOUBLE) MirrorCmdMsg.Value) / 1000.0;
            NewFovWidthInDouble  = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomX + NewZoomInDouble));
            NewFovWidth  = (UINT32) NewFovWidthInDouble;

            if (NewFovWidth > SvcViewCtrlDefaultInfo[FovIdx].RawWidth) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! New Width (%d) over Raw Width (%d)", NewFovWidth, SvcViewCtrlDefaultInfo[FovIdx].RawWidth);
                RetVal |= SVC_NG;
            } else if (NewZoomInDouble >= VIEW_CTRL_ZOOM_RATIO_MAX) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ZoomX Ratio cannot be bigger than %d.%03d", ZoomMaxInteger, ZoomMaxFractional);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Current ZoomX Ratio is %d.%03d", ZoomInteger, ZoomFractional);
                RetVal |= SVC_NG;
            } else {
                RetVal |= SVC_OK;
            }

            Zoom1000InDouble = SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY * 1000.0;
            Zoom1000 = (UINT32) Zoom1000InDouble;
            ZoomInteger = (UINT32) SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY;
            ZoomInteger1000 = ZoomInteger * 1000U;
            ZoomFractional = Zoom1000 - ZoomInteger1000;

            NewZoomInDouble = (Zoom1000InDouble + (DOUBLE) MirrorCmdMsg.Value) / 1000.0;
            NewFovHeightInDouble = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomY + NewZoomInDouble));
            NewFovHeight = (UINT32) NewFovHeightInDouble;

            if (NewFovHeight > SvcViewCtrlDefaultInfo[FovIdx].RawHeight) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! New Height (%d) over Raw Height (%d)", NewFovHeight, SvcViewCtrlDefaultInfo[FovIdx].RawHeight);
                RetVal |= SVC_NG;
            } else if (NewZoomInDouble >= VIEW_CTRL_ZOOM_RATIO_MAX) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ZoomY Ratio cannot be bigger than %d.%03d", ZoomMaxInteger, ZoomMaxFractional);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Current ZoomY Ratio is %d.%03d", ZoomInteger, ZoomFractional);
                RetVal |= SVC_NG;
            } else {
                RetVal |= SVC_OK;
            }
        break;
        case SVC_VIEW_CTRL_WARP_CMD_ROTATE:
            NewFovWidthInDouble  = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomX + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomX));
            NewFovHeightInDouble = ((DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight) / ((DOUBLE) (SvcViewCtrlDefaultInfo[FovIdx].ZoomY + SvcViewCtrlWarpInfo[FovIdx].Position.ZoomY));

            if (SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta < 0.0) {
                DOUBLE ThetaD = SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta * -1.0;
                Theta10InDouble = SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta * 10.0 * -1.0;
                ThetaInteger = (UINT32) ThetaD;
            } else {
                Theta10InDouble = SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta * 10.0 * 1.0;
                ThetaInteger = (UINT32) SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta;
            }
            Theta10 = (UINT32) Theta10InDouble;
            ThetaInteger10 = ThetaInteger * 10U;
            ThetaFractional = Theta10 - ThetaInteger10;

            /* Judgement the rotation is smaller than 5 degree */
            if ((SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta + ((DOUBLE)MirrorCmdMsg.Value / 10.0)) < VIEW_CTRL_ROTATE_MIN) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! Theta cannot be smaller than spec -%d", (UINT32) VIEW_CTRL_ROTATE_MAX, 0U);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Current Theta is -%d.%d", ThetaInteger, ThetaFractional);
                RetVal = SVC_NG;
            } else if ((SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta + ((DOUBLE)MirrorCmdMsg.Value / 10.0)) > VIEW_CTRL_ROTATE_MAX) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! Theta cannot be over than spec %d", (UINT32) VIEW_CTRL_ROTATE_MAX, 0U);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Current Theta is %d.%d", ThetaInteger, ThetaFractional);
                RetVal = SVC_NG;
            } else {
                RetVal = SVC_OK;
            }

            /* Judgement the rotated result will not exceed Raw dimension */
            if (SVC_OK == RetVal) {
                DOUBLE X, Y;
                DOUBLE NewX, NewY;
                DOUBLE EndX, EndY;
                ThetaInRadius = (SvcViewCtrlWarpInfo[FovIdx].Rotation.Theta + ((DOUBLE)MirrorCmdMsg.Value / 10.0)) * VIEW_CTRL_PI / 180.0;
                // RetVal = AmbaWrap_tan(ThetaInRadius, &TanTheta);
                RetVal = AmbaWrap_sin(ThetaInRadius, &SinTheta);
                RetVal |= AmbaWrap_cos(ThetaInRadius, &CosTheta);

                if (SVC_OK == RetVal) {
                    /* First quadrant */
                    X = (NewFovWidthInDouble * 0.5) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX;
                    Y = (NewFovHeightInDouble * 0.5) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY;
                    NewX = (CosTheta * X) + ((-1.0 * SinTheta) * Y);
                    NewY = (SinTheta * X) + (CosTheta * Y);
                    EndX = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth * 0.5;
                    EndY = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight * 0.5;

                    if (NewX > EndX) {
                                SVC_WRAP_PRINT "NewX (%f), EndX (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewX
                                SVC_PRN_ARG_DOUBLE EndX
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }
                    if (NewY > EndY) {
                                SVC_WRAP_PRINT "NewY (%f), EndY (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewY
                                SVC_PRN_ARG_DOUBLE EndY
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }

                    /* Second quadrant */
                    X = (NewFovWidthInDouble * 0.5 * (-1.0)) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX;
                    Y = (NewFovHeightInDouble * 0.5) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY;
                    NewX = (CosTheta * X) + ((-1.0 * SinTheta) * Y);
                    NewY = (SinTheta * X) + (CosTheta * Y);
                    EndX = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth * 0.5 * (-1.0);
                    EndY = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight * 0.5;

                    if (NewX < EndX) {
                                SVC_WRAP_PRINT "NewX (%f), EndX (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewX
                                SVC_PRN_ARG_DOUBLE EndX
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }
                    if (NewY > EndY) {
                                SVC_WRAP_PRINT "NewY (%f), EndY (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewY
                                SVC_PRN_ARG_DOUBLE EndY
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }

                    /* Third quadrant */
                    X = (NewFovWidthInDouble * 0.5 * (-1.0)) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX;
                    Y = (NewFovHeightInDouble * 0.5 * (-1.0)) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY;
                    NewX = (CosTheta * X) + ((-1.0 * SinTheta) * Y);
                    NewY = (SinTheta * X) + (CosTheta * Y);
                    EndX = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth * 0.5 * (-1.0);
                    EndY = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight * 0.5 * (-1.0);

                    if (NewX < EndX) {
                                SVC_WRAP_PRINT "NewX (%f), EndX (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewX
                                SVC_PRN_ARG_DOUBLE EndX
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }
                    if (NewY < EndY) {
                                SVC_WRAP_PRINT "NewY (%f), EndY (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewY
                                SVC_PRN_ARG_DOUBLE EndY
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }

                    /* Fourth quadrant */
                    X = (NewFovWidthInDouble * 0.5) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftX + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftX;
                    Y = (NewFovHeightInDouble * 0.5 * (-1.0)) + (DOUBLE) SvcViewCtrlWarpInfo[FovIdx].Position.ShiftY + (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].ShiftY;
                    NewX = (CosTheta * X) + ((-1.0 * SinTheta) * Y);
                    NewY = (SinTheta * X) + (CosTheta * Y);
                    EndX = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawWidth * 0.5;
                    EndY = (DOUBLE) SvcViewCtrlDefaultInfo[FovIdx].RawHeight * 0.5 * (-1.0);

                    if (NewX > EndX) {
                                SVC_WRAP_PRINT "NewX (%f), EndX (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewX
                                SVC_PRN_ARG_DOUBLE EndX
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }
                    if (NewY < EndY) {
                                SVC_WRAP_PRINT "NewY (%f), EndY (%f) "
                                SVC_PRN_ARG_S __func__
                                SVC_PRN_ARG_PROC SvcLog_DBG
                                SVC_PRN_ARG_DOUBLE NewY
                                SVC_PRN_ARG_DOUBLE EndY
                                SVC_PRN_ARG_E
                        RetVal = SVC_NG;
                    }
                }
            }
        break;
        case SVC_VIEW_CTRL_WARP_CMD_SET_WIN:
            RetVal = SVC_OK;
        break;
        case SVC_VIEW_CTRL_PREV_CMD_PAN:
            if (((INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX + MirrorCmdMsg.Value) < 0) {
                MinShift = (INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX;
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ShiftX should be bigger than -%d", (UINT32) MinShift, 0U);
                RetVal = SVC_NG;
            }
            if (((INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX + (INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width + MirrorCmdMsg.Value) > (INT32) pCfg->FovCfg[FovIdx].MainWin.Width) {
                MaxShift = ((INT32) pCfg->FovCfg[FovIdx].MainWin.Width - ((INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetX + (INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Width));
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ShiftX should be smaller than %d", (UINT32) MaxShift, 0U);
                RetVal = SVC_NG;
            }
        break;
        case SVC_VIEW_CTRL_PREV_CMD_TILT:
            if (((INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY + MirrorCmdMsg.Value) < 0) {
                MinShift = (INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY;
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ShiftY should be bigger than -%d", (UINT32) MinShift, 0U);
                RetVal = SVC_NG;
            }
            if (((INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY + (INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height + MirrorCmdMsg.Value) > (INT32) pCfg->FovCfg[FovIdx].MainWin.Height) {
                MaxShift = ((INT32) pCfg->FovCfg[FovIdx].MainWin.Height - ((INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.OffsetY + (INT32) SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.Height));
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ShiftY should be smaller than %d", (UINT32) MaxShift, 0U);
                RetVal = SVC_NG;
            }
        break;
        case SVC_VIEW_CTRL_PREV_CMD_ZOOM:
            ZoomMax1000InDouble = (DOUBLE) VIEW_CTRL_ZOOM_RATIO_MAX * 1000.0;
            ZoomMax1000 = (UINT32) ZoomMax1000InDouble;
            ZoomMaxInteger = ZoomMax1000 / 1000U;
            ZoomMaxInteger1000 = ZoomMaxInteger * 1000U;
            ZoomMaxFractional = ZoomMax1000 - ZoomMaxInteger1000;

            Zoom1000InDouble = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomX * 1000.0;
            Zoom1000 = (UINT32) Zoom1000InDouble;
            ZoomInteger = Zoom1000 / 1000U;
            ZoomInteger1000 = ZoomInteger * 1000U;
            ZoomFractional = Zoom1000 - ZoomInteger1000;

            NewZoomInDouble = (Zoom1000InDouble + (DOUBLE) MirrorCmdMsg.Value) / 1000.0;

            NewPrevWidthInDouble  = ((DOUBLE) SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Width) / NewZoomInDouble;
            NewPrevWidth = (UINT32) NewPrevWidthInDouble;

            if (NewPrevWidth > pCfg->FovCfg[FovIdx].MainWin.Width) {    /* Cannot exceed main full-view */
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! New Width (%d) over Main Width (%d)", NewPrevWidth, pCfg->FovCfg[FovIdx].MainWin.Width);
                RetVal |= SVC_NG;
            } else if (NewZoomInDouble >= VIEW_CTRL_ZOOM_RATIO_MAX) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ZoomX Ratio cannot be bigger than %d.%03d", ZoomMaxInteger, ZoomMaxFractional);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Current ZoomX Ratio is %d.%03d", ZoomInteger, ZoomFractional);
                RetVal |= SVC_NG;
            } else {
                RetVal |= SVC_OK;
            }

            Zoom1000InDouble = SvcViewCtrlPrevInfo[VoutID][FovIdx].SrcPosition.ZoomY * 1000.0;
            Zoom1000 = (UINT32) Zoom1000InDouble;
            ZoomInteger = Zoom1000 / 1000U;
            ZoomInteger1000 = ZoomInteger * 1000U;
            ZoomFractional = Zoom1000 - ZoomInteger1000;

            NewZoomInDouble = (Zoom1000InDouble + (DOUBLE) MirrorCmdMsg.Value) / 1000.0;

            NewPrevHeightInDouble = ((DOUBLE) SvcViewCtrlDefaultPrevInfo[VoutID][FovIdx].SrcPosition.Height) / NewZoomInDouble;
            NewPrevHeight = (UINT32) NewPrevHeightInDouble;

            if (NewPrevHeight > pCfg->FovCfg[FovIdx].MainWin.Height) {    /* Cannot exceed main full-view */
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! New Height (%d) over Main Height (%d)", NewPrevHeight, pCfg->FovCfg[FovIdx].MainWin.Height);
                RetVal |= SVC_NG;
            } else if (NewZoomInDouble >= VIEW_CTRL_ZOOM_RATIO_MAX) {
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Invalid!! ZoomY Ratio cannot be bigger than %d.%03d", ZoomMaxInteger, ZoomMaxFractional);
                ViewCtrl_Dbg(SVC_LOG_VIEW_CTRL, "Current ZoomY Ratio is %d.%03d", ZoomInteger, ZoomFractional);
                RetVal |= SVC_NG;
            } else {
                RetVal |= SVC_OK;
            }
        break;
        case SVC_VIEW_CTRL_PREV_CMD_SET_SRCWIN:
        case SVC_VIEW_CTRL_PREV_CMD_SET_DSTWIN:
            RetVal = SVC_OK;
        break;
        default:
            RetVal = SVC_NG;
        break;
    }

    return RetVal;
}
