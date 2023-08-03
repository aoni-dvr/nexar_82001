/**
*  @file SvcStillProc.c
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
*  @details svc boot device
*
*/

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"

#include "AmbaSensor.h"

#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_Liveview.h"

#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcResCfg.h"
#include "SvcDSP.h"
#include "SvcPlat.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillProc.h"
#include "SvcStillEnc.h"
#include "SvcStillMux.h"
#include "SvcTask.h"
#include "SvcIsoCfg.h"
#include "SvcIK.h"
#include "SvcClock.h"
#if defined(CONFIG_BUILD_COMSVC_IMGFRW)
#include "SvcImg.h"
#endif
#if defined(CONFIG_ICAM_IMGCAL_USED)
#include "SvcCalibMgr.h"
#endif

//#define ENABLE_R2R

static inline UINT32 SPROC_ALIGN(UINT32 AlignVal, UINT32 Size)
{
    UINT32 RetVal = 0U;

    if ((AlignVal >= 2U) && (Size > 0U)) {
        RetVal = ( Size + ( AlignVal - 1U ) ) & ( ~ ( AlignVal - 1U ) );
    }

    return RetVal;
}

typedef struct {
    UINT8 MainStat;
    UINT8 ScrnStat;
    UINT8 ThmbStat;
    UINT8 QviewStat;
} SVC_STL_PROC_STAT_s;

typedef struct {
    SVC_STL_PROC_STAT_s    RawStat;
    SVC_CAP_MSG_s          StlProcQueBuf[SVC_STL_MUX_NUM_FILES * SVC_STL_NUM_VIEW];
    SVC_CAP_MSG_s          Input[SVC_MAX_PIV_STREAMS];
    SVC_YUV_IMG_BUF_s      OutMain[SVC_MAX_PIV_STREAMS];
    SVC_YUV_IMG_BUF_s      OutScrn[SVC_MAX_PIV_STREAMS];
    SVC_YUV_IMG_BUF_s      OutThmb[SVC_MAX_PIV_STREAMS];
    SVC_YUV_IMG_BUF_s      OutQview[SVC_MAX_PIV_STREAMS];
    ULONG                  WorkBufAddr;
    UINT32                 WorkBufSize;
    UINT32                 IkCtxId;
    UINT8                  PicType[SVC_MAX_PIV_STREAMS];
    UINT8                  StreamId[SVC_MAX_PIV_STREAMS];
    UINT8                  SensorNum[SVC_MAX_PIV_STREAMS];
    UINT16                 RawSeq;
    UINT32                 NumCap;
    UINT32                 NumThumb;
    UINT32                 StlR2rTime;
    UINT32                 StlR2rRescaleTime;
    UINT32                 StlProcTime[SVC_STL_NUM_VIEW];
    UINT32                 StlProcMainStart;
    UINT32                 StlProcScrnStart;
    UINT32                 StlProcThmbStart;
    UINT8                  FixPipe;
    UINT8                  FixHiso;
    UINT8                  RestartLiveview;
    UINT8                  TestFlag;
    AMBA_KAL_MSG_QUEUE_t   StlProcQue;
    AMBA_KAL_EVENT_FLAG_t  StlProcFlg;
    SVC_TASK_CTRL_s        TaskCtrl;
    AMBA_IK_CFA_3A_DATA_s  CfaData;
    SVC_STILL_QVIEW_CFG_s  Qview;
} SVC_STL_PROC_CTRL_s;

static UINT8 StlProc_DebugOn = 0;
static UINT8 StlProc_Created = 0;
static SVC_STL_PROC_CTRL_s StlProc GNU_SECTION_NOZEROINIT;

#define SVC_STL_PROC_READY (1)

/**
* set pipeline of still process
* @param [in] FixPipe 0 - by 3A; 1 - by user
* @param [in] FixHiso 0 - LISO; 1 - HISO
*/
void SvcStillProc_SetPipe(UINT8 FixPipe, UINT8 FixHiso)
{
    StlProc.FixPipe = FixPipe;
    StlProc.FixHiso = FixHiso;
    SvcLog_DBG(SVC_LOG_STL_PROC, "FixPipe %u FixHiso %u", StlProc.FixPipe, StlProc.FixHiso);
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
static UINT16 StlProc_MaxYuvInputW = 0;
#endif

/**
* set maximun yuv input width for CV2FS y2y
* @param [in] MaxYuvInputWidth maximun yuv input width
*/
void SvcStillProc_SetMaxYuvInputWidth(UINT16 MaxYuvInputWidth)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    StlProc_MaxYuvInputW = MaxYuvInputWidth;
    SvcLog_DBG(SVC_LOG_STL_PROC, "MaxYuvInputWidth %u", StlProc_MaxYuvInputW, 0U);
#else
    AmbaMisra_TouchUnused(&MaxYuvInputWidth);
#endif
}

/**
* set number of capture instance in this process
* @param [in] NumCap number of capture instance
*/
void SvcStillProc_SetNumCap(UINT32 NumCap)
{
    StlProc.NumCap = NumCap;
}

/**
* set testing flag
*/
void SvcStillProc_SetTestFlag(UINT8 Flag)
{
    StlProc.TestFlag = Flag;
}

static UINT32 CfaStatReady(const void *pEventInfo)
{
    UINT32 Rval, t;
    Rval = AmbaWrap_memcpy(&StlProc.CfaData, pEventInfo, sizeof(AMBA_IK_CFA_3A_DATA_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
    } else {
#if (!defined(CONFIG_SOC_CV2FS)) && (!defined(CONFIG_SOC_CV5)) && (!defined (CONFIG_SOC_CV52))
        SvcLog_OK(SVC_LOG_STL_PROC, "[CfaStatReady] AwbTileWidth %u AwbTileHeight %u", StlProc.CfaData.Header.AwbTileWidth, StlProc.CfaData.Header.AwbTileHeight);
#endif
        Rval = AmbaKAL_EventFlagSet(&StlProc.StlProcFlg, SVC_STL_PROC_READY);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_PROC, "Event flag set failed with 0x%x", Rval, 0U);
        } else {
            Rval = AmbaKAL_GetSysTickCount(&t);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            } else {
                if (t >= StlProc.StlProcMainStart) {
                    StlProc.StlR2rTime = t - StlProc.StlProcMainStart;
                } else {
                    StlProc.StlR2rTime = (0xFFFFFFFFU - StlProc.StlProcMainStart) + t;
                }
            }
        }
    }
    return Rval;
}

static UINT32 YuvDataReady(const void *pEventInfo)
{
    UINT32 Rval = OK, DoProcess = 0, t;
    const AMBA_DSP_YUV_IMG_BUF_s *pInfo;
    AmbaMisra_TypeCast(&pInfo, &pEventInfo);
    // SvcLog_OK(SVC_LOG_STL_PROC, "RawStat %u %u", StlProc.RawStat.MainStat, StlProc.RawStat.ScrnStat);
    // SvcLog_OK(SVC_LOG_STL_PROC, "        %u", StlProc.RawStat.ThmbStat, 0);
    SvcLog_OK(SVC_LOG_STL_PROC, "[YuvDataReady] w %u h %u", pInfo->Window.Width, pInfo->Window.Height);
    SvcLog_OK(SVC_LOG_STL_PROC, "               pitch %u", pInfo->Pitch, 0);
    if ((StlProc.RawStat.MainStat == 2U) && (StlProc.RawStat.ScrnStat == 0U)) {
        Rval = AmbaKAL_GetSysTickCount(&t);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
        }
        if (t >= StlProc.StlProcMainStart) {
            StlProc.StlProcTime[SVC_STL_MAIN_VIEW] = t - StlProc.StlProcMainStart;
        } else {
            StlProc.StlProcTime[SVC_STL_MAIN_VIEW] = (0xFFFFFFFFU - StlProc.StlProcMainStart) + t;
        }
        StlProc.StlProcTime[SVC_STL_MAIN_VIEW] += StlProc.StlProcTime[SVC_STL_MAIN_VIEW];
        SvcStillLog("[YuvDataReady] main yuv done, addr %p %p", pInfo->BaseAddrY, pInfo->BaseAddrUV, 0, 0, 0);
        StlProc.RawStat.MainStat = 3;
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "MainStat %u", StlProc.RawStat.MainStat, 0);
        }
        DoProcess = 1;
    } else {
        if ((StlProc.RawStat.ScrnStat == 1U) && (StlProc.RawStat.ThmbStat == 0U)) {
            Rval = AmbaKAL_GetSysTickCount(&t);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            }
            if (t >= StlProc.StlProcScrnStart) {
                StlProc.StlProcTime[SVC_STL_SCRN_VIEW] = t - StlProc.StlProcScrnStart;
            } else {
                StlProc.StlProcTime[SVC_STL_SCRN_VIEW] = (0xFFFFFFFFU - StlProc.StlProcScrnStart) + t;
            }
            SvcStillLog("[YuvDataReady] scrn yuv done, addr %p %p", pInfo->BaseAddrY, pInfo->BaseAddrUV, 0, 0, 0);
            StlProc.RawStat.ScrnStat = 2;
            if (StlProc_DebugOn > 0U) {
                SvcLog_OK(SVC_LOG_STL_PROC, "ScrnStat %u", StlProc.RawStat.ScrnStat, 0);
            }
            DoProcess = 1;
        } else {
            if (StlProc.RawStat.ThmbStat == 1U) {
                Rval = AmbaKAL_GetSysTickCount(&t);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
                }
                if (t >= StlProc.StlProcThmbStart) {
                    StlProc.StlProcTime[SVC_STL_THMB_VIEW] = t - StlProc.StlProcThmbStart;
                } else {
                    StlProc.StlProcTime[SVC_STL_THMB_VIEW] = (0xFFFFFFFFU - StlProc.StlProcThmbStart) + t;
                }
                SvcStillLog("[YuvDataReady] thmb yuv done, addr %p %p", pInfo->BaseAddrY, pInfo->BaseAddrUV, 0, 0, 0);
                StlProc.RawStat.ThmbStat = 2;
                if (StlProc_DebugOn > 0U) {
                    SvcLog_OK(SVC_LOG_STL_PROC, "ThmbStat %u", StlProc.RawStat.ThmbStat, 0);
                }
                DoProcess = 1;
#ifdef CONFIG_SOC_CV2FS
                {
                    SVC_STL_MUX_INPUT_s MuxInput;
                    Rval = AmbaWrap_memset(&MuxInput, 0, sizeof(MuxInput));
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                    } else {
                        MuxInput.PicType = SVC_STL_TYPE_LUMA;
                        MuxInput.PicRdy.StartAddr = pInfo->BaseAddrY;
                        MuxInput.PicRdy.PicSize = (UINT32)pInfo->Pitch * (UINT32)pInfo->Window.Height;
                        Rval = SvcStillMux_InputPic(&MuxInput);
                        if (Rval == OK) {
                            MuxInput.PicType = SVC_STL_TYPE_CHROMA;
                            MuxInput.PicRdy.StartAddr = pInfo->BaseAddrUV;
                            MuxInput.PicRdy.PicSize = MuxInput.PicRdy.PicSize >> 1U;
                            Rval = SvcStillMux_InputPic(&MuxInput);
                        }
                    }
                }
#endif
            }
        }
    }

    if (StlProc.RawStat.QviewStat == 1U) {
        SvcStillLog("[YuvDataReady] qview yuv done, addr %p %p,  %u x %u", pInfo->BaseAddrY, pInfo->BaseAddrUV, pInfo->Window.Width, pInfo->Window.Height, 0);
        StlProc.RawStat.QviewStat = 2;
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "QviewStat %u", StlProc.RawStat.QviewStat, 0);
        }
        DoProcess = 1;
    }

    if (DoProcess == 1U) {
        Rval = AmbaKAL_EventFlagSet(&StlProc.StlProcFlg, SVC_STL_PROC_READY);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_PROC, "Event flag set failed with 0x%x", Rval, 0U);
        }
    }
    return Rval;
}
#ifdef SVC_STL_ENABLE_CLEAN_RAW
static UINT32 CleanRawReady(const void *pEventInfo)
{
    AMBA_DSP_RAW_DATA_RDY_s  *pStillRawData = NULL;
    UINT32 Rval, t;

    AmbaMisra_TypeCast(&pStillRawData, &pEventInfo);
    if (StlProc_DebugOn > 0U) {
        void *pPointer;
        AmbaMisra_TypeCast(&pPointer, &(pStillRawData->RawBuffer.BaseAddr));
        SVC_WRAP_PRINT "[CleanRawReady] RawAddr = %p" SVC_PRN_ARG_S SVC_LOG_STL_PROC
            SVC_PRN_ARG_CPOINT pPointer SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        SvcLog_DBG(SVC_LOG_STL_PROC, "[CleanRawReady] Pitch = %u", pStillRawData->RawBuffer.Pitch, 0);
        SvcLog_DBG(SVC_LOG_STL_PROC, "[CleanRawReady] Width = %u", pStillRawData->RawBuffer.Window.Width, 0);
        SvcLog_DBG(SVC_LOG_STL_PROC, "[CleanRawReady] Height = %u", pStillRawData->RawBuffer.Window.Height, 0);
    }

    Rval = AmbaKAL_GetSysTickCount(&t);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
    } else {
        StlProc.StlR2rRescaleTime = t - StlProc.StlProcMainStart;
        if (1U == SvcStillCap_GetCleanRawDump()) {
            SVC_STL_MUX_INPUT_s MuxInput;
            Rval = AmbaWrap_memset(&MuxInput, 0, sizeof(MuxInput));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                MuxInput.PicType = SVC_STL_TYPE_CLEAN_RAW;
                MuxInput.PicRdy.StartAddr = pStillRawData->RawBuffer.BaseAddr;
                MuxInput.PicRdy.PicSize = (UINT32)(pStillRawData->RawBuffer.Pitch) * (UINT32)(pStillRawData->RawBuffer.Window.Height);
                Rval = SvcStillMux_InputPic(&MuxInput);
            }
        }
    }

    Rval = AmbaKAL_EventFlagSet(&StlProc.StlProcFlg, SVC_STL_PROC_READY);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_PROC, "Event flag set failed with 0x%x", Rval, 0U);
    }
    return 0U;
}
#endif
/**
* query memory size needed for still process task
* @param [in] pSetup pointer to configuration of still process
* @param [in] pMemSize pointer to needed memory size
* @return 0-OK, 1-NG
*/
UINT32 SvcStillProc_QueryMem(const SVC_STL_PROC_SETUP_s *pSetup, UINT32 *pMemSize)
{
    *pMemSize = SPROC_ALIGN(YUV_ALIGN_BYTE, pSetup->MaxMainYuvW) * SPROC_ALIGN(2, pSetup->MaxMainYuvH);
    *pMemSize += SPROC_ALIGN(YUV_ALIGN_BYTE, pSetup->ScrnYuvW) * SPROC_ALIGN(2, pSetup->ScrnYuvH);
    *pMemSize += SPROC_ALIGN(YUV_ALIGN_BYTE, SVC_STL_THMB_W) * SPROC_ALIGN(2, (SVC_STL_THMB_W * (UINT32)pSetup->ScrnYuvH / (UINT32)pSetup->ScrnYuvW));
    *pMemSize += SPROC_ALIGN(YUV_ALIGN_BYTE, pSetup->QviewYuvW) * SPROC_ALIGN(2, pSetup->QviewYuvH) * AMBA_DSP_MAX_VOUT_NUM;
#ifdef CONFIG_SOC_H22
    *pMemSize = *pMemSize << 1U; // 422
#else
    *pMemSize = (*pMemSize * 3U) >> 1U; // 420
#endif
    SvcLog_OK(SVC_LOG_STL_PROC, "[SvcStillProc_QueryMem] size %u", *pMemSize, 0);
    return SVC_OK;
}

/**
* input yuv to still process task
* @param [in] pInfo pointer to info block of still process
* @return 0-OK, 1-NG
*/
UINT32 SvcStillProc_InputData(const SVC_STL_PROC_INFO_s *pInfo)
{
    UINT32 Rval;

    if (StlProc_DebugOn > 0U) {
        SvcLog_OK(SVC_LOG_STL_PROC, "CapInstance %u", pInfo->CapInstance, 0);
        SvcLog_OK(SVC_LOG_STL_PROC, "Input w %u h %u", pInfo->InputInfo.Width, pInfo->InputInfo.Height);
    }
    Rval = AmbaWrap_memset(&StlProc.Input[pInfo->CapInstance], 0, sizeof(SVC_CAP_MSG_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&StlProc.OutMain[pInfo->CapInstance], 0, sizeof(SVC_YUV_IMG_BUF_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&StlProc.OutScrn[pInfo->CapInstance], 0, sizeof(SVC_YUV_IMG_BUF_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&StlProc.OutThmb[pInfo->CapInstance], 0, sizeof(SVC_YUV_IMG_BUF_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }

    Rval = AmbaWrap_memcpy(&StlProc.Input[pInfo->CapInstance], &pInfo->InputInfo, sizeof(SVC_CAP_MSG_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
    } else {
        SvcStillCap_SetStatus(SVC_STL_PROC_BUSY);

        StlProc.Input[pInfo->CapInstance].CapInstance = pInfo->CapInstance;

        StlProc.OutMain[pInfo->CapInstance].Width = pInfo->OutMainW;
        StlProc.OutMain[pInfo->CapInstance].Height = pInfo->OutMainH;
        StlProc.OutMain[pInfo->CapInstance].Pitch = (UINT16)SPROC_ALIGN(YUV_ALIGN_BYTE, pInfo->OutMainW);
        StlProc.OutMain[pInfo->CapInstance].BaseAddrY = StlProc.WorkBufAddr;
        StlProc.OutMain[pInfo->CapInstance].BaseAddrUV = StlProc.OutMain[pInfo->CapInstance].BaseAddrY + ((ULONG)StlProc.OutMain[pInfo->CapInstance].Pitch * (ULONG)StlProc.OutMain[pInfo->CapInstance].Height);

        StlProc.OutScrn[pInfo->CapInstance].Width = pInfo->OutScrnW;
        StlProc.OutScrn[pInfo->CapInstance].Height = pInfo->OutScrnH;
        StlProc.OutScrn[pInfo->CapInstance].Pitch = (UINT16)SPROC_ALIGN(YUV_ALIGN_BYTE, pInfo->OutScrnW);

        StlProc.OutThmb[pInfo->CapInstance].Width = pInfo->OutThmbW;
        StlProc.OutThmb[pInfo->CapInstance].Height = pInfo->OutThmbH;
        StlProc.OutThmb[pInfo->CapInstance].Pitch = (UINT16)SPROC_ALIGN(YUV_ALIGN_BYTE, pInfo->OutThmbW);

#ifdef CONFIG_SOC_H22
        StlProc.OutMain[pInfo->CapInstance].DataFmt = AMBA_DSP_YUV422;
        StlProc.OutScrn[pInfo->CapInstance].DataFmt = AMBA_DSP_YUV422;
        StlProc.OutThmb[pInfo->CapInstance].DataFmt = AMBA_DSP_YUV422;
        StlProc.OutScrn[pInfo->CapInstance].BaseAddrY = StlProc.OutMain[pInfo->CapInstance].BaseAddrUV + (StlProc.OutMain[pInfo->CapInstance].BaseAddrUV - StlProc.OutMain[pInfo->CapInstance].BaseAddrY);
        StlProc.OutScrn[pInfo->CapInstance].BaseAddrUV = StlProc.OutScrn[pInfo->CapInstance].BaseAddrY + ((ULONG)StlProc.OutScrn[pInfo->CapInstance].Pitch * (ULONG)StlProc.OutScrn[pInfo->CapInstance].Height);
        StlProc.OutThmb[pInfo->CapInstance].BaseAddrY = StlProc.OutScrn[pInfo->CapInstance].BaseAddrUV + (StlProc.OutScrn[pInfo->CapInstance].BaseAddrUV - StlProc.OutScrn[pInfo->CapInstance].BaseAddrY);
        StlProc.OutThmb[pInfo->CapInstance].BaseAddrUV = StlProc.OutThmb[pInfo->CapInstance].BaseAddrY + ((ULONG)StlProc.OutThmb[pInfo->CapInstance].Pitch * (ULONG)StlProc.OutThmb[pInfo->CapInstance].Height);
#else
        StlProc.OutMain[pInfo->CapInstance].DataFmt = AMBA_DSP_YUV420;
        StlProc.OutScrn[pInfo->CapInstance].DataFmt = AMBA_DSP_YUV420;
        StlProc.OutThmb[pInfo->CapInstance].DataFmt = AMBA_DSP_YUV420;
        StlProc.OutScrn[pInfo->CapInstance].BaseAddrY = StlProc.OutMain[pInfo->CapInstance].BaseAddrUV + ((StlProc.OutMain[pInfo->CapInstance].BaseAddrUV - StlProc.OutMain[pInfo->CapInstance].BaseAddrY) >> 1U);
        StlProc.OutScrn[pInfo->CapInstance].BaseAddrUV = StlProc.OutScrn[pInfo->CapInstance].BaseAddrY + ((ULONG)StlProc.OutScrn[pInfo->CapInstance].Pitch * (ULONG)StlProc.OutScrn[pInfo->CapInstance].Height);
        StlProc.OutThmb[pInfo->CapInstance].BaseAddrY = StlProc.OutScrn[pInfo->CapInstance].BaseAddrUV + ((StlProc.OutScrn[pInfo->CapInstance].BaseAddrUV - StlProc.OutScrn[pInfo->CapInstance].BaseAddrY) >> 1U);
        StlProc.OutThmb[pInfo->CapInstance].BaseAddrUV = StlProc.OutThmb[pInfo->CapInstance].BaseAddrY + ((ULONG)StlProc.OutThmb[pInfo->CapInstance].Pitch * (ULONG)StlProc.OutThmb[pInfo->CapInstance].Height);
#endif
        if (pInfo->Qview.Enable == 1U) {
            ULONG   BaseAddr;
            UINT8   DataFmt;
#if defined(CONFIG_SOC_H22)
            DataFmt     = AMBA_DSP_YUV422;
            BaseAddr    = StlProc.OutThmb[pInfo->CapInstance].BaseAddrUV + (StlProc.OutThmb[pInfo->CapInstance].BaseAddrUV - StlProc.OutThmb[pInfo->CapInstance].BaseAddrY);
#else
            DataFmt     = AMBA_DSP_YUV420;
            BaseAddr    = StlProc.OutThmb[pInfo->CapInstance].BaseAddrUV + ((StlProc.OutThmb[pInfo->CapInstance].BaseAddrUV - StlProc.OutThmb[pInfo->CapInstance].BaseAddrY) >> 1U);
#endif
            for (UINT32 i = 0u; i < pInfo->Qview.NumQview; i++) {
                UINT32 Pitch;
                UINT32 YBufSize;
                UINT32 UvBufSize;

                Pitch = SPROC_ALIGN(YUV_ALIGN_BYTE, pInfo->Qview.Win[i].Width);
                YBufSize = Pitch * pInfo->Qview.Win[i].Height;
#if defined(CONFIG_SOC_H22)
                UvBufSize = YBufSize;       /* AMBA_DSP_YUV422  */
#else
                UvBufSize = YBufSize >> 1U; /* AMBA_DSP_YUV420  */
#endif

                StlProc.OutQview[i].DataFmt = DataFmt;
                StlProc.OutQview[i].Width   = (UINT16) pInfo->Qview.Win[i].Width;
                StlProc.OutQview[i].Height  = (UINT16) pInfo->Qview.Win[i].Height;
                StlProc.OutQview[i].Pitch   = (UINT16) Pitch;

                StlProc.OutQview[i].BaseAddrY  = BaseAddr;
                StlProc.OutQview[i].BaseAddrY  += (UINT32)((YBufSize + UvBufSize) * i);
                StlProc.OutQview[i].BaseAddrUV = StlProc.OutQview[i].BaseAddrY + YBufSize;
            }

            Rval = AmbaWrap_memcpy(&(StlProc.Qview), &(pInfo->Qview), sizeof(SVC_STILL_QVIEW_CFG_s));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
        }

        if (StlProc_DebugOn > 0U) {
            SvcStillLog("[SvcStillProc_InputData] out main y %p uv %p", StlProc.OutMain[pInfo->CapInstance].BaseAddrY, StlProc.OutMain[pInfo->CapInstance].BaseAddrUV, 0, 0, 0);
            SvcStillLog("[SvcStillProc_InputData] out scrn y %p uv %p", StlProc.OutScrn[pInfo->CapInstance].BaseAddrY, StlProc.OutScrn[pInfo->CapInstance].BaseAddrUV, 0, 0, 0);
            SvcStillLog("[SvcStillProc_InputData] out thum y %p uv %p", StlProc.OutThmb[pInfo->CapInstance].BaseAddrY, StlProc.OutThmb[pInfo->CapInstance].BaseAddrUV, 0, 0, 0);

            if(pInfo->Qview.Enable == 1U) {
                for (UINT8 i = 0u; i < pInfo->Qview.NumQview; i++) {
                    SvcStillLog("[SvcStillProc_InputData] out qview[%u] y %p uv %p", i, StlProc.OutQview[i].BaseAddrY, StlProc.OutQview[i].BaseAddrUV, 0, 0);
                }
            }
        }

        StlProc.PicType[pInfo->CapInstance] = pInfo->PicType;
        StlProc.StreamId[pInfo->CapInstance] = pInfo->StreamId;
        StlProc.SensorNum[pInfo->CapInstance] = pInfo->SensorNum;
        StlProc.RestartLiveview = pInfo->RestartLiveview;

        Rval = AmbaKAL_MsgQueueSend(&StlProc.StlProcQue, &StlProc.Input[pInfo->CapInstance], 1000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_PROC, "Message send failed with 0x%x", Rval, 0U);
        } else {
            if (StlProc_DebugOn > 0U) {
                SvcLog_OK(SVC_LOG_STL_PROC, "[SvcStillProc_InputData] CapInstance %u PicType %u", pInfo->CapInstance, StlProc.PicType[pInfo->CapInstance]);
            }
        }
    }
    return Rval;
}

#ifndef CONFIG_SOC_H22
static UINT32 GetSensorType(UINT8 BayerPattern, UINT32 *pSensorType)
{
    UINT32 Rval = SVC_OK;
    switch (BayerPattern & 0xF0U) {
        case 0x00:
            *pSensorType = SVC_IK_SENSOR_RGB;
        break;
        case 0x10:
            *pSensorType = SVC_IK_SENSOR_RGB_IR;
        break;
        case 0x20:
            *pSensorType = SVC_IK_SENSOR_RCCC;
        break;
        default:
            *pSensorType = SVC_IK_SENSOR_RGB;
            SvcLog_NG(SVC_LOG_STL_PROC, "Unknown Bayer Pattern (0x%x). Please check sensor driver", BayerPattern, 0U);
            Rval = SVC_NG;
        break;
    }
    return Rval;
}

static UINT32 GetSensorPattern(UINT8 BayerPattern, UINT32 *pSensorPattern)
{
    UINT32 Rval = SVC_OK;
    switch (BayerPattern & 0x0FU) {
        case 0x00:
        case 0x04:
            *pSensorPattern = 0U;
        break;
        case 0x01:
        case 0x05:
            *pSensorPattern = 1U;
        break;
        case 0x02:
        case 0x06:
            *pSensorPattern = 2U;
        break;
        case 0x03:
        case 0x07:
            *pSensorPattern = 3U;
        break;
        default:
            *pSensorPattern = 0U;
            SvcLog_NG(SVC_LOG_STL_PROC, "Unknown Bayer Pattern (0x%x). Please check sensor driver", BayerPattern, 0U);
            Rval = SVC_NG;
        break;
    }
    return Rval;
}

static UINT32 SetRgbIr(UINT8 BayerPattern, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 Rval;
#if defined(AMBA_SENSOR_BAYER_PATTERN_RGGI)
    UINT32            RgbIrMode = 0U;
    AMBA_IK_RGB_IR_s  IkRgbIr;
    Rval = AmbaWrap_memset(&IkRgbIr, 0, sizeof(IkRgbIr));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        switch (BayerPattern) {
            case AMBA_SENSOR_BAYER_PATTERN_RGGI:
            case AMBA_SENSOR_BAYER_PATTERN_IGGR:
            case AMBA_SENSOR_BAYER_PATTERN_GRIG:
            case AMBA_SENSOR_BAYER_PATTERN_GIRG:
                RgbIrMode = 1U;     /* Look at 2x2 quad square of source CFA pixel. And it has red pixel and IR */
            break;
            case AMBA_SENSOR_BAYER_PATTERN_BGGI:
            case AMBA_SENSOR_BAYER_PATTERN_IGGB:
            case AMBA_SENSOR_BAYER_PATTERN_GBIG:
            case AMBA_SENSOR_BAYER_PATTERN_GIBG:
                RgbIrMode = 2U;     /* Look at 2x2 quad square of source CFA pixel. And it has blue pixel and IR */
            break;
            default:
                SvcLog_NG(SVC_LOG_STL_PROC, "Mismatch BayerPattern (0x%x) and RGB-IR sensor", BayerPattern, 0U);
                RgbIrMode = 0U;
                Rval = SVC_NG;
            break;
        }
        if (Rval != SVC_NG) {
            IkRgbIr.Mode = RgbIrMode;
            Rval = AmbaIK_SetRgbIr(pImgMode, &IkRgbIr);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "## fail to set RgbIr info", 0U, 0U);
            }
        }
    }
#else
    AmbaMisra_TouchUnused(&BayerPattern);
    AmbaMisra_TouchUnused(&pImgMode);
    Rval = SVC_OK;
#endif
    return Rval;
}

static UINT32 SetForBayer(const AMBA_SENSOR_STATUS_INFO_s *pSensorStatus, const SVC_CAP_MSG_s *pMsg, const AMBA_IK_MODE_CFG_s *pImgMode, UINT8 Pipe)
{
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo;
    UINT32 Rval = SVC_OK;

    if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
        UINT32 SensorPattern = 0, SensorType = 0;
        Rval = AmbaWrap_memset(&VinSensorInfo, 0, sizeof(VinSensorInfo));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        } else {
            VinSensorInfo.VinId = pMsg->Index;
            VinSensorInfo.SensorResolution = pSensorStatus->ModeInfo.OutputInfo.NumDataBits;
            /* Determine SensorType */
            Rval = GetSensorType(pSensorStatus->ModeInfo.OutputInfo.BayerPattern, &SensorType);
            if (Rval == OK) {
                /* Determine BayerPattern based on different sensor type refering to DGG */
                Rval = GetSensorPattern(pSensorStatus->ModeInfo.OutputInfo.BayerPattern, &SensorPattern);
            }
            if (Rval == OK) {
                VinSensorInfo.SensorPattern = SensorPattern;
                VinSensorInfo.SensorMode = SensorType;
                VinSensorInfo.Compression = pMsg->RawCmpr;
                VinSensorInfo.CompressionOffset = 0; //TBD
                VinSensorInfo.SensorReadoutMode = 0;
                Rval = AmbaIK_SetVinSensorInfo(pImgMode, &VinSensorInfo);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "## fail to set sensor info", 0U, 0U);
                }
            }
            if ((Rval == OK) && (VinSensorInfo.SensorMode == SVC_IK_SENSOR_RGB_IR) && (Pipe != AMBA_IK_PIPE_STILL)) {
                Rval = SetRgbIr(pSensorStatus->ModeInfo.OutputInfo.BayerPattern, pImgMode);
            }
        }
    }
    return Rval;
}

static UINT32 SetForGeometry(UINT32 WidthIn, UINT32 HeightIn, const AMBA_SENSOR_STATUS_INFO_s *pSensorStatus, const SVC_CAP_MSG_s *pMsg, const AMBA_IK_MODE_CFG_s *pImgMode, const AMBA_IK_ABILITY_s *pImgAbility, const AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo)
{
    UINT32 Rval;
    AMBA_IK_WINDOW_SIZE_INFO_s VinSizeInfo;

    Rval = AmbaWrap_memset(&VinSizeInfo, 0, sizeof(VinSizeInfo));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        VinSizeInfo.VinSensor.StartX = pSensorStatus->ModeInfo.InputInfo.PhotodiodeArray.StartX;
        VinSizeInfo.VinSensor.StartY = pSensorStatus->ModeInfo.InputInfo.PhotodiodeArray.StartY;
        VinSizeInfo.VinSensor.HSubSample.FactorDen = (UINT32)1U;
        VinSizeInfo.VinSensor.HSubSample.FactorNum = (UINT32)1U;
        VinSizeInfo.VinSensor.VSubSample.FactorDen = (UINT32)1U;
        VinSizeInfo.VinSensor.VSubSample.FactorNum = (UINT32)1U;
        if (StlProc_DebugOn > 0U) {
            SvcLog_DBG(SVC_LOG_STL_PROC, "WidthIn %u HeightIn %u", WidthIn, HeightIn);
        }
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define ALIGN_W (128U)
#else
#define ALIGN_W (64U)
#endif
#define ALIGN_H (8U)
        VinSizeInfo.VinSensor.Width = SPROC_ALIGN(ALIGN_W, WidthIn);
        VinSizeInfo.VinSensor.Height = SPROC_ALIGN(ALIGN_H, HeightIn);
        if (WidthIn != VinSizeInfo.VinSensor.Width) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetWindowSizeInfo WidthIn w %u is not %u aligned", WidthIn, ALIGN_W);
        }
        if (HeightIn != VinSizeInfo.VinSensor.Height) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetWindowSizeInfo HeightIn w %u is not %u aligned", HeightIn, ALIGN_H);
        }
#else
        VinSizeInfo.VinSensor.Width = WidthIn;
        VinSizeInfo.VinSensor.Height = HeightIn;
#endif
        if (StlProc_DebugOn > 0U) {
            SvcLog_DBG(SVC_LOG_STL_PROC, "AmbaIK_SetWindowSizeInfo VinSensor %u %u", VinSizeInfo.VinSensor.Width, VinSizeInfo.VinSensor.Height);
        }
        VinSizeInfo.Main.Width = WidthIn;
        VinSizeInfo.Main.Height = HeightIn;
        if (StlProc_DebugOn > 0U) {
            SvcLog_DBG(SVC_LOG_STL_PROC, "AmbaIK_SetWindowSizeInfo Main %u %u", VinSizeInfo.Main.Width, VinSizeInfo.Main.Height);
        }
        Rval = AmbaIK_SetWindowSizeInfo(pImgMode, &VinSizeInfo);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetWindowSizeInfo failed! return 0x%x", Rval, 0U);
        }
    }
    if (Rval == OK) {
        /* set active window */
        if ((pImgAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y) || (pImgAbility->Pipe == AMBA_IK_PIPE_STILL)) {
            AMBA_IK_VIN_ACTIVE_WINDOW_s     ActWin;
            Rval = AmbaWrap_memset(&ActWin, 0, sizeof(ActWin));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                ActWin.Enable           = 0U;
                ActWin.ActiveGeo.Width  = WidthIn;//pSensorStatus->ModeInfo.InputInfo.PhotodiodeArray.Width;
                ActWin.ActiveGeo.Height = HeightIn;//pSensorStatus->ModeInfo.InputInfo.PhotodiodeArray.Height;
                ActWin.ActiveGeo.StartX = pSensorStatus->ModeInfo.InputInfo.PhotodiodeArray.StartX + (UINT32)pMsg->OffsetX;
                ActWin.ActiveGeo.StartY = pSensorStatus->ModeInfo.InputInfo.PhotodiodeArray.StartY + (UINT32)pMsg->OffsetY;
                if (StlProc_DebugOn > 0U) {
                    SvcLog_DBG(SVC_LOG_STL_PROC, "AmbaIK_SetVinActiveWin %u %u", ActWin.ActiveGeo.Width, ActWin.ActiveGeo.Height);
                }
                Rval = AmbaIK_SetVinActiveWin(pImgMode, &ActWin);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetVinActiveWin failed! return 0x%x", Rval, 0U);
                }
            }
        }
    }

    if (Rval == OK) {
        Rval = AmbaIK_SetWarpEnb(pImgMode, 0);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetWarpEnb failed! return 0x%x", Rval, 0U);
        }
    }
    if ((Rval == OK) && (pMsg->CapType == SVC_FRAME_TYPE_RAW) && (pSensorStatus->ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)) {
#ifndef CONFIG_BUILD_COMSVC_IMGFRW
#ifdef CONFIG_SOC_H22
        ULONG Temp;
        AmbaMisra_TypeCast(&Temp, &pHdrRawInfo);
        SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetHdrRawOffset not defined", 0, 0U);
#else
        Rval = AmbaIK_SetHdrRawOffset(pImgMode, pHdrRawInfo);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetHdrRawOffset failed! return 0x%x", Rval, 0U);
        }
#endif
#else
        AmbaMisra_TouchUnused(&pHdrRawInfo);
#endif
    }
    return Rval;
}
#endif

static UINT32 SetForCalibration(const AMBA_SENSOR_STATUS_INFO_s *pSensorStatus, const SVC_CAP_MSG_s *pMsg, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 Rval = SVC_OK;

    if ((pSensorStatus != NULL) && (pMsg != NULL) && (pImgMode != NULL)) {
        if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
#ifdef CONFIG_ICAM_IMGCAL_USED
            SVC_CALIB_CHANNEL_s CalibChan;

            Rval = AmbaWrap_memset(&CalibChan, 0, sizeof(CalibChan));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else if (pMsg->SensorIdx >= SVC_CAP_MAX_SENSOR_PER_VIN) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] invalid SensorIdx(%d)", __LINE__, pMsg->SensorIdx);
            } else {
                CalibChan.VinID      = pMsg->Index;
                CalibChan.SensorID   = ((UINT32)1 << pMsg->SensorIdx);
                CalibChan.ExtendData = pSensorStatus->ModeInfo.Config.ModeID;

                if (0U == SvcCalib_ItemTableUpdate(SVC_CALIB_LDC_ID, &CalibChan, pImgMode)) {
                    SvcLog_DBG(SVC_LOG_STL_PROC, "proc calib ldc done", 0U, 0U);
                }
                if (0U == SvcCalib_ItemTableUpdate(SVC_CALIB_CA_ID, &CalibChan, pImgMode)) {
                    SvcLog_DBG(SVC_LOG_STL_PROC, "proc calib ca done", 0U, 0U);
                }
                if (0U == SvcCalib_ItemTableUpdate(SVC_CALIB_VIGNETTE_ID, &CalibChan, pImgMode)) {
                    SvcLog_DBG(SVC_LOG_STL_PROC, "proc calib vig done", 0U, 0U);
                }
                if (0U == SvcCalib_ItemTableUpdate(SVC_CALIB_BPC_ID, &CalibChan, pImgMode)) {
                    SvcLog_DBG(SVC_LOG_STL_PROC, "proc calib bpc done", 0U, 0U);
                }
            }
#else
            SvcLog_DBG(SVC_LOG_STL_PROC, "calibration does not enable", 0U, 0U);
#endif
        }
    }

    return Rval;
}

#ifndef CONFIG_SOC_H22
static UINT32 CheckStillHiso(const SVC_CAP_MSG_s *pMsg, AMBA_SENSOR_STATUS_INFO_s *pSensorStatus, UINT32 *pIsHiso)
{
    UINT32 Rval;
    AMBA_SENSOR_CHANNEL_s SensorChan;
    if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
        SensorChan.VinID = pMsg->Index;
        SensorChan.SensorID = 0;
        Rval = AmbaSensor_GetStatus(&SensorChan, pSensorStatus);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaSensor_GetStatus failed! return 0x%x", Rval, 0U);
        }
        SvcLog_DBG(SVC_LOG_STL_PROC, "FixPipe %u FixHiso %u", StlProc.FixPipe, StlProc.FixHiso);
        if (StlProc.FixPipe == 1U) {
            *pIsHiso = (UINT32)StlProc.FixHiso;
            SvcLog_DBG(SVC_LOG_STL_PROC, "FixHiso %u", *pIsHiso, 0U);
        } else {
            UINT32 SensorType = 0;
            /* Determine SensorType */
            Rval = GetSensorType(pSensorStatus->ModeInfo.OutputInfo.BayerPattern, &SensorType);
            if (SensorType == SVC_IK_SENSOR_RGB_IR) {
                SvcLog_DBG(SVC_LOG_STL_PROC, "using RGB IR sensor", 0, 0U);
                if (pMsg->IsHiso != 0U) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "Do not support HISO for RGB IR sensor", 0, 0U);
                }
                *pIsHiso = 0;
            } else {
                *pIsHiso = (UINT32)pMsg->IsHiso;
            }
            SvcLog_DBG(SVC_LOG_STL_PROC, "IsHiso %u", *pIsHiso, 0U);
        }
    } else {
        Rval = OK;
    }
    return Rval;
}

static UINT32 SetIkPipe(const SVC_CAP_MSG_s *pMsg, const AMBA_SENSOR_STATUS_INFO_s *pSensorStatus, AMBA_IK_ABILITY_s *pImgAbility, const AMBA_IK_MODE_CFG_s *pImgMode, UINT32 *pIsHiso, AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo)
{
    UINT32 Rval;
    UINT32 IkPipeMode = 0xFFFFFFFFU;

    if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
        if (pSensorStatus->ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) { /* DOL HDR */
            UINT8 i;
#if !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52)
            *pIsHiso = 0; /* Only CV5x support DOL-HDR HISO */
#endif
            if (*pIsHiso == 0U) {
                pImgAbility->Pipe = AMBA_IK_PIPE_VIDEO;
                if (pSensorStatus->ModeInfo.HdrInfo.ActiveChannels == 2U) {
                    pImgAbility->VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_2;
                } else {
                    if (pSensorStatus->ModeInfo.HdrInfo.ActiveChannels == 3U) {
                        pImgAbility->VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;
                    }
                }
            } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                pImgAbility->Pipe = AMBA_IK_PIPE_STILL;
                pImgAbility->StillPipe = AMBA_IK_STILL_HISO;

                if (pSensorStatus->ModeInfo.HdrInfo.ActiveChannels == 2U) {
                    IkPipeMode = (UINT32)AMBA_IK_STILL_HDR2x;
                } else {
                    if (pSensorStatus->ModeInfo.HdrInfo.ActiveChannels == 3U) {
                        IkPipeMode = (UINT32)AMBA_IK_STILL_HDR3x;
                    }
                }

#else
                SvcLog_NG(SVC_LOG_STL_PROC, "Not support DOL-HDR HISO", 0U, 0U);
#endif
            }
            for (i = 0; i < pSensorStatus->ModeInfo.HdrInfo.ActiveChannels; i ++) {
                pHdrRawInfo->YOffset[i] = ((UINT32)pSensorStatus->ModeInfo.HdrInfo.ChannelInfo[i].EffectiveArea.StartY *
                                         (UINT32)pSensorStatus->ModeInfo.HdrInfo.ActiveChannels) + (UINT32)i;
                SvcLog_DBG(SVC_LOG_STL_PROC, "YOffset[%u] = %u", i, pHdrRawInfo->YOffset[i]);
            }
        } else {
#ifndef CONFIG_SOC_CV2FS
            if (*pIsHiso == 1U) {
                pImgAbility->Pipe = AMBA_IK_PIPE_STILL;
                pImgAbility->StillPipe = AMBA_IK_STILL_HISO;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                if (pMsg->CeAvail == 1U) {
                    IkPipeMode = (UINT32)AMBA_IK_STILL_LINEAR_CE;
                }
#endif
            } else {
#else
            {
                *pIsHiso = 0;
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                if (pMsg->CeAvail == 1U) {
                    pImgAbility->Pipe = AMBA_IK_PIPE_STILL;
                    pImgAbility->StillPipe = AMBA_IK_STILL_LISO;
                    IkPipeMode = (UINT32)AMBA_IK_STILL_LINEAR_CE;
                } else {
                    pImgAbility->Pipe = AMBA_IK_PIPE_VIDEO;
                    pImgAbility->VideoPipe = AMBA_IK_VIDEO_LINEAR;
                }
#else
                pImgAbility->Pipe = AMBA_IK_PIPE_VIDEO;
                if (pSensorStatus->ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) { /* sensor HDR */
                    pImgAbility->VideoPipe = AMBA_IK_VIDEO_LINEAR_CE;
                } else {
                    pImgAbility->VideoPipe = AMBA_IK_VIDEO_LINEAR;
                }
#endif
            }
        }

        AmbaMisra_TouchUnused(&IkPipeMode);
        AmbaMisra_TouchUnused(pIsHiso);
    } else {
        pImgAbility->Pipe = AMBA_IK_PIPE_VIDEO;
        pImgAbility->VideoPipe = AMBA_IK_VIDEO_Y2Y;
    }

    if (pImgAbility->Pipe == AMBA_IK_PIPE_VIDEO) {
        SvcLog_DBG(SVC_LOG_STL_PROC, "ik Pipe %u VideoPipe %u", pImgAbility->Pipe, pImgAbility->VideoPipe);
    } else {
        SvcLog_DBG(SVC_LOG_STL_PROC, "ik Pipe %u StillPipe %u", pImgAbility->Pipe, pImgAbility->StillPipe);
    }

    Rval = AmbaIK_InitContext(pImgMode, pImgAbility);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_InitContext failed! return 0x%x", Rval, 0U);
    } else {
        if (IkPipeMode != 0xFFFFFFFFU) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            Rval = AmbaIK_SetPipeMode(pImgMode, IkPipeMode);
            if (Rval != 0U) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetPipeMode failed! return 0x%x", Rval, 0U);
            }
#endif
        }
    }
    return Rval;
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
UINT32 SvcStillProc_SetIkTile(const AMBA_IK_MODE_CFG_s *pImgMode, UINT16 SliceType, UINT16 ImgInWidth, UINT16 ImgInHeight, UINT16 ImgOutWidth, UINT16 ImgOutHeight)
{
    UINT32 Rval;
    AMBA_IK_STITCH_INFO_s Info;
    UINT32 TileNumX;
    if (ImgInWidth > 2048U) {
#ifdef CONFIG_SOC_CV2FS
        const UINT32 CalcWith = 1920U;
        UINT32 CurWidth = (UINT32)ImgInWidth;

        CurWidth -= 2048U;
        TileNumX = CurWidth / CalcWith;
        if ((CurWidth % CalcWith) > 0U) {
            TileNumX += 1U;
        }
        TileNumX += 1U;

        // align 2
        if ((TileNumX % 2U) > 0U) {
            TileNumX +=1U;
        }

        AmbaMisra_TouchUnused(&SliceType);
        AmbaMisra_TouchUnused(&ImgInWidth);
        AmbaMisra_TouchUnused(&ImgInHeight);
        AmbaMisra_TouchUnused(&ImgOutWidth);
        AmbaMisra_TouchUnused(&ImgOutHeight);

#else   // defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

        AMBA_DSP_WINDOW_DIMENSION_s ImgWinIn;
        AMBA_DSP_WINDOW_DIMENSION_s ImgWinOut;
        AMBA_DSP_SLICE_CFG_s        CurSliceCfg;

        Rval  = AmbaWrap_memset(&ImgWinIn, 0, sizeof(ImgWinIn));
        Rval |= AmbaWrap_memset(&ImgWinOut, 0, sizeof(ImgWinOut));
        Rval |= AmbaWrap_memset(&CurSliceCfg, 0, sizeof(CurSliceCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }

        ImgWinIn.Width  = ImgInWidth;
        ImgWinIn.Height = ImgInHeight;
        ImgWinOut.Width  = ImgOutWidth;
        ImgWinOut.Height = ImgOutHeight;

        if (StlProc_DebugOn > 0U) {
            SvcLog_DBG(SVC_LOG_STL_PROC, "Calc capture slice with 0x%X", (UINT32) SliceType, 0);
            SvcLog_DBG(SVC_LOG_STL_PROC, "  ImgInWin : %dx%d", (UINT32) ImgWinIn.Width, (UINT32) ImgWinIn.Height);
            SvcLog_DBG(SVC_LOG_STL_PROC, "  ImgOutWin: %dx%d", (UINT32) ImgWinOut.Width, (UINT32) ImgWinOut.Height);
        }

        Rval = AmbaDSP_SliceCfgCalc(SliceType,
                                    &ImgWinIn, &ImgWinOut,
                                    &CurSliceCfg);
        if (Rval != 0U) {
            SvcLog_NG(SVC_LOG_STL_PROC, "Calculate Slice failed! return 0x%x", Rval, 0U);
        } else {
            if (StlProc_DebugOn > 0U) {
                SvcLog_DBG(SVC_LOG_STL_PROC, "  CurSliceCfg.SliceNumCol       : %d", CurSliceCfg.SliceNumCol       , 0U);
                SvcLog_DBG(SVC_LOG_STL_PROC, "  CurSliceCfg.SliceNumRow       : %d", CurSliceCfg.SliceNumRow       , 0U);
                SvcLog_DBG(SVC_LOG_STL_PROC, "  CurSliceCfg.EncSyncOpt        : %d", CurSliceCfg.EncSyncOpt        , 0U);
                SvcLog_DBG(SVC_LOG_STL_PROC, "  CurSliceCfg.WarpLumaWaitLine  : %d", CurSliceCfg.WarpLumaWaitLine  , 0U);
                SvcLog_DBG(SVC_LOG_STL_PROC, "  CurSliceCfg.WarpChromaWaitLine: %d", CurSliceCfg.WarpChromaWaitLine, 0U);
                SvcLog_DBG(SVC_LOG_STL_PROC, "  CurSliceCfg.WarpOverLap       : %d", CurSliceCfg.WarpOverLap       , 0U);
                SvcLog_DBG(SVC_LOG_STL_PROC, "  CurSliceCfg.VinDragLine       : %d", CurSliceCfg.VinDragLine       , 0U);
            }
        }

        TileNumX = (UINT32)(CurSliceCfg.SliceNumCol);

#endif
    } else {
        TileNumX = 1;
    }

    Rval = AmbaWrap_memset(&Info, 0, sizeof(Info));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }

    Info.Enable = (TileNumX == 1U)? (UINT32)0U: (UINT32)1U;
    Info.TileNum_x = TileNumX;
    Info.TileNum_y = 1;
    SvcLog_DBG(SVC_LOG_STL_PROC, "AmbaIK_SetStitchingInfo TileNumX %u, TileNumY %u", Info.TileNum_x, Info.TileNum_y);
    Rval = AmbaIK_SetStitchingInfo(pImgMode, &Info);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_SetStitchingInfo failed! return 0x%x", Rval, 0U);
    }
    return Rval;
}
#endif

#ifdef CONFIG_BUILD_COMSVC_IMGFRW
#define STILL_PROC_TYPE_R2R (1U)
#define STILL_PROC_TYPE_CLEAN_RAW (2U)
#endif
#ifdef CONFIG_BUILD_COMSVC_IMGFRW
static UINT32 SetIso(const SVC_CAP_MSG_s *pMsg, const UINT32 IsHiso, AMBA_IK_MODE_CFG_s *pImgMode, UINT8 ProcType)
#else
static UINT32 SetIso(const SVC_CAP_MSG_s *pMsg, UINT32 IsHiso, const AMBA_IK_MODE_CFG_s *pImgMode, UINT8 ProcType)
#endif
{
    UINT32 Rval;
    if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
#if defined(CONFIG_BUILD_COMSVC_IMGFRW)
        if (StlProc_DebugOn > 0U) {
            SvcLog_DBG(SVC_LOG_STL_PROC, "IsHiso %u", IsHiso, 0);
        }
        if (ProcType == STILL_PROC_TYPE_R2R) {
            SvcImg_StillIso(pMsg->Index, StlProc.SensorNum[pMsg->CapInstance], NULL, 3U, IsHiso, pImgMode);
        } else if (ProcType == STILL_PROC_TYPE_CLEAN_RAW) {
            SvcIsoCfg_Fixed(pImgMode, 3);
        } else {
#ifdef ENABLE_R2R
            SvcImg_StillIso(pMsg->Index, StlProc.SensorNum[pMsg->CapInstance], NULL, 0U, IsHiso, pImgMode);
#else
            SvcImg_StillIso(pMsg->Index, StlProc.SensorNum[pMsg->CapInstance], &StlProc.CfaData, 0U, IsHiso, pImgMode);
#endif
        }
#else
        SvcIsoCfg_Fixed(pImgMode, 1);
        AmbaMisra_TouchUnused(&ProcType);
        AmbaMisra_TouchUnused(&IsHiso);
#endif
        Rval = OK;
    } else {
        SvcIsoCfg_Fixed(pImgMode, 1);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        Rval = AmbaIK_SetYuvMode(pImgMode, pMsg->DataFmt); // for CV2FS y2y
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_InitContext failed! return 0x%x", Rval, 0U);
        }
#else
        Rval = OK;
#endif
    }

    return Rval;
}
#endif

#ifdef CONFIG_SOC_H22
static UINT32 PrepareIsoCfg(UINT32 WidthIn, UINT32 HeightIn, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const SVC_CAP_MSG_s *pMsg, UINT8 ProcType)
#else
static UINT32 PrepareIsoCfg(UINT32 WidthIn, UINT32 HeightIn, AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const SVC_CAP_MSG_s *pMsg, UINT8 ProcType)
#endif
{
#ifdef CONFIG_SOC_H22
    AmbaMisra_TouchUnused(&pIsoCfg);
    AmbaMisra_TouchUnused(&WidthIn);
    AmbaMisra_TouchUnused(&HeightIn);
    AmbaMisra_TouchUnused(&pMsg);
    AmbaMisra_TouchUnused(&ProcType);
    return OK;
#else
    AMBA_IK_MODE_CFG_s ImgMode = {0};
    AMBA_IK_ABILITY_s ImgAbility = {0};
    AMBA_IK_EXECUTE_CONTAINER_s CfgExecInfo = {0};
    UINT32 Rval;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo;
    UINT32 IsHiso = 0;

    Rval = AmbaWrap_memset(&ImgMode, 0, sizeof(ImgMode));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&ImgAbility, 0, sizeof(ImgAbility));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&CfgExecInfo, 0, sizeof(CfgExecInfo));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&SensorStatus, 0, sizeof(SensorStatus));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&HdrRawInfo, 0, sizeof(HdrRawInfo));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }

    Rval = CheckStillHiso(pMsg, &SensorStatus, &IsHiso);

#ifdef CONFIG_SOC_CV2FS
    ImgMode.ContextId = StlProc.IkCtxId;
#else
    if (IsHiso == 1U) {
        ImgMode.ContextId = StlProc.IkCtxId; // HISO
    } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (pMsg->CeAvail == 1U) {
            ImgMode.ContextId = StlProc.IkCtxId - 1U; // LISO with CE
        } else {
            ImgMode.ContextId = StlProc.IkCtxId - 2U; // LISO without CE
        }
#else
        ImgMode.ContextId = StlProc.IkCtxId - 1U; // LISO
#endif
    }
#endif
    SvcLog_DBG(SVC_LOG_STL_PROC, "ContextId %u", ImgMode.ContextId, 0U);

    if (Rval == OK) {
        Rval = SetIkPipe(pMsg, &SensorStatus, &ImgAbility, &ImgMode, &IsHiso, &HdrRawInfo);
    }
    if (Rval == OK) {
        Rval = SetIso(pMsg, IsHiso, &ImgMode, ProcType);
    }
    if (Rval == OK) {
        Rval = SetForBayer(&SensorStatus, pMsg, &ImgMode, ImgAbility.Pipe);
    }
    if (Rval == OK) {
        Rval = SetForGeometry(WidthIn, HeightIn, &SensorStatus, pMsg, &ImgMode, &ImgAbility, &HdrRawInfo);
    }
    if (Rval == OK) {
        Rval = SetForCalibration(&SensorStatus, pMsg, &ImgMode);
    }

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (Rval == OK) {
        UINT32 SliceType = 0U;                      // Liv
        if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
#ifdef CONFIG_BUILD_COMSVC_IMGFRW
#if defined(CONFIG_ICAM_STLCAP_CLEAN_RAW_ENABLED)
            if (ProcType == STILL_PROC_TYPE_CLEAN_RAW) {
                SliceType = (UINT32)2U << 12U;          // R2R for clean raw
            } else
#endif
            if (ProcType == STILL_PROC_TYPE_R2R) {
                SliceType = (UINT32)2U << 12U;          // R2R
            } else
#endif
            {
                SliceType = (UINT32)1U << 12U;          // R2Y
            }
        } else {
            SliceType = (UINT32)3U << 12U;          // Y2Y
        }

        Rval = SvcStillProc_SetIkTile(&ImgMode, (UINT16) SliceType
                                     , (UINT16) WidthIn
                                     , (UINT16) HeightIn
                                     , (UINT16) WidthIn
                                     , (UINT16) HeightIn);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "SvcStillProc_SetIkTile failed! return 0x%x", Rval, 0U);
        }
    }
#endif

    if (Rval == OK) {
        SvcLog_DBG(SVC_LOG_STL_PROC, "AmbaIK_ExecuteConfig", 0, 0U);
        Rval = AmbaIK_ExecuteConfig(&ImgMode, &CfgExecInfo);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaIK_ExecuteConfig failed! return 0x%x", Rval, 0U);
        } else {
            pIsoCfg->CtxIndex = ImgMode.ContextId;
            pIsoCfg->CfgIndex = CfgExecInfo.IkId;
            AmbaMisra_TypeCast(&pIsoCfg->CfgAddress, &CfgExecInfo.pIsoCfg);
        }
    }

    if ((StlProc_DebugOn == 2U) || (StlProc_DebugOn == 3U)) {
        if ((pMsg->CapType == SVC_FRAME_TYPE_RAW) && (ProcType == 0U)) {  // dump in r2y process
            if (0U != SvcStillMux_CreatePicFile(SVC_STL_TYPE_IDSP, SVC_STL_IDSP_ITUNER, (UINT16)ImgMode.ContextId, 0U, 0U, 0U)) {
                SvcLog_NG(SVC_LOG_STL_PROC, "Dump ituner fail!", 0U, 0U);
            }
            if (0U != SvcStillMux_CreatePicFile(SVC_STL_TYPE_IDSP, SVC_STL_IDSP_IK_CR, (UINT16)ImgMode.ContextId, 0U, 0U, 0U)) {
                SvcLog_NG(SVC_LOG_STL_PROC, "Dump ik cr fail!", 0U, 0U);
            }
        }
    }

    return Rval;
#endif
}

static void SvcStillProc_WaitProcStart(void)
{
    UINT32 Rval = AmbaKAL_EventFlagClear(&StlProc.StlProcFlg, SVC_STL_PROC_READY);
    if (Rval != 0U) {
        SvcLog_NG(SVC_LOG_STL_PROC, "Event flag clear failed with 0x%x", Rval, 0U);
    }
}

static UINT32 SvcStillProc_WaitProcDone(void)
{
    UINT32 Rval, ActualFlags = 0;
    Rval = AmbaKAL_EventFlagGet(&StlProc.StlProcFlg, SVC_STL_PROC_READY, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 1000U);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_STL_PROC, "Event flag get failed with 0x%x", Rval, 0U);
    }
    return Rval;
}

static UINT32 CopyMainYuv(const SVC_CAP_MSG_s *pMsg)
{
    UINT8 *pSrc, *pDst;
    UINT32 Rval;
    ULONG ReqSize;

    AmbaMisra_TypeCast(&pSrc, &pMsg->BaseAddrY);
    AmbaMisra_TypeCast(&pDst, &StlProc.OutMain[pMsg->CapInstance].BaseAddrY);
    if (StlProc_DebugOn > 0U) {
        SvcStillLog("[CopyMainYuv] copy main y from %p to %p", pMsg->BaseAddrY, StlProc.OutMain[pMsg->CapInstance].BaseAddrY, 0, 0, 0);
    }
    ReqSize = (ULONG)(pMsg->Pitch) * (ULONG)(pMsg->Height);
    Rval = SvcStillBufCopy(pDst, pSrc, (UINT32)ReqSize);
    if (Rval == SVC_OK) {
        if (0U != SvcPlat_CacheClean(StlProc.OutMain[pMsg->CapInstance].BaseAddrY, ReqSize)) {
            if (StlProc_DebugOn > 0U) {
                SvcStillLog("[CopyMainYuv] proc dst Y buf %p cache clean fail. if memory is non-cahce, ignore it.", StlProc.OutMain[pMsg->CapInstance].BaseAddrY, 0, 0, 0, 0);
            }
        }
        AmbaMisra_TypeCast(&pSrc, &pMsg->BaseAddrUV);
        AmbaMisra_TypeCast(&pDst, &StlProc.OutMain[pMsg->CapInstance].BaseAddrUV);
        if (StlProc_DebugOn > 0U) {
            SvcStillLog("[CopyMainYuv] copy main uv from %p to %p", pMsg->BaseAddrUV, StlProc.OutMain[pMsg->CapInstance].BaseAddrUV, 0, 0, 0);
        }
#ifndef CONFIG_SOC_H22
        ReqSize = ReqSize >> 1;     //yuv420
#endif
        Rval = SvcStillBufCopy(pDst, pSrc, (UINT32)ReqSize);
        if (Rval == SVC_OK) {
            if (0U != SvcPlat_CacheClean(StlProc.OutMain[pMsg->CapInstance].BaseAddrUV, ReqSize)) {
                if (StlProc_DebugOn > 0U) {
                    SvcStillLog("[CopyMainYuv] proc dst UV buf %p cache clean fail. if memory is non-cahce, ignore it.", StlProc.OutMain[pMsg->CapInstance].BaseAddrUV, 0, 0, 0, 0);
                }
            }
        }
    }

    if (Rval == SVC_OK) {
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "copy for main done", 0, 0U);
        }
        StlProc.RawStat.MainStat = 2;
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "MainStat %u", StlProc.RawStat.MainStat, 0);
        }
    }
    return Rval;
}

static UINT32 LiveviewRestart(UINT32 LastRval)
{
    UINT32 Rval;

    if ((LastRval == OK) && (StlProc.RestartLiveview == 1U)) {
        if (pSvcLivStart != NULL) {
            Rval = SvcStillCap_ImgStop();
            if (Rval == OK) {
                Rval = pSvcLivStart();
            }
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = LastRval;
    }

    return Rval;
}

#ifdef ENABLE_R2R
static UINT32 R2rForMain(const SVC_CAP_MSG_s *pMsg)
{
    AMBA_DSP_RAW_BUF_s RawIn;
    AMBA_DSP_RAW_BUF_s RawOut;
    AMBA_DSP_BUF_s CeInfo;
    AMBA_DSP_ISOCFG_CTRL_s IsoCfg;
    UINT32 Rval;
    AMBA_SENSOR_CHANNEL_s SensorChan;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    Rval = AmbaWrap_memset(&SensorStatus, 0, sizeof(SensorStatus));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
        SensorChan.VinID = pMsg->Index;
        SensorChan.SensorID = 0;
        Rval = AmbaSensor_GetStatus(&SensorChan, &SensorStatus);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaSensor_GetStatus failed! return 0x%x", Rval, 0U);
        }
    }

    if (SensorStatus.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {

        Rval = AmbaWrap_memset(&IsoCfg, 0, sizeof(IsoCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        Rval = PrepareIsoCfg(pMsg->Width, pMsg->Height, &IsoCfg, pMsg, STILL_PROC_TYPE_R2R);

        if (Rval == OK) {
            Rval = AmbaWrap_memset(&RawIn, 0, sizeof(RawIn));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            Rval = AmbaWrap_memset(&RawOut, 0, sizeof(RawOut));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            RawIn.Compressed = pMsg->RawCmpr;     /* 1 - compressed raw data, 0 - uncompressed raw data */
            RawIn.BaseAddr = pMsg->BaseAddrY;       /* raw buffer address */
            RawIn.Pitch = pMsg->Pitch;          /* raw buffer pitch */
            RawIn.Window.Width =  pMsg->Width;         /* Window position and size */
            RawIn.Window.Height =  pMsg->Height;
            Rval = AmbaWrap_memset(&CeInfo, 0, sizeof(CeInfo));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            if (pMsg->CeAvail == 1U) {
                UINT16 RawWidth, Pitch;
                CeInfo.BaseAddr = pMsg->BaseAddrUV;
                Rval = AmbaDSP_GetCmprRawBufInfo((pMsg->Width >> 2U), pMsg->RawCmpr, &RawWidth, &Pitch);
                CeInfo.Pitch = Pitch;
                if (Rval == OK) {
                    CeInfo.Window.Width =  CeInfo.Pitch;         /* Window position and size */
                    CeInfo.Window.Height =  RawIn.Window.Height;
                } else {
                    SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_GetCmprRawBufInfo failed, return 0x%x", Rval, 0);
                }
            }
            if (Rval == OK) {
#ifdef CONFIG_SOC_H22
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillRaw2Raw not defined for H22 yet", 0, 0U);
#else
                SvcLog_OK(SVC_LOG_STL_PROC, "r2r for main", 0, 0U);
                Rval = AmbaDSP_StillRaw2Raw(&RawIn, &CeInfo, &RawOut, &IsoCfg, STL_R2R_OPT_GEN_AAA, NULL);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillRaw2Raw failed! return 0x%x", Rval, 0U);
                } else {
                    Rval = SvcStillProc_WaitProcDone();
                }
#endif
            }
        }
        if (Rval == OK) {
            Rval = AmbaKAL_GetSysTickCount(&StlProc.StlProcMainStart);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == OK) {
            StlProc.RawStat.MainStat = 1;
            if (StlProc_DebugOn > 0U) {
                SvcLog_OK(SVC_LOG_STL_PROC, "MainStat %u", StlProc.RawStat.MainStat, 0);
            }
        }
    }
    return Rval;
}
#endif

#ifdef SVC_STL_ENABLE_CLEAN_RAW
static UINT32 R2rForCleanRaw(const SVC_CAP_MSG_s *pMsg)
{
    AMBA_DSP_RAW_BUF_s RawIn;
    AMBA_DSP_RAW_BUF_s RawOut;
    AMBA_DSP_BUF_s CeInfo;
    AMBA_DSP_ISOCFG_CTRL_s IsoCfg;
    UINT32 Rval;
    AMBA_SENSOR_CHANNEL_s SensorChan;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    SvcLog_DBG(SVC_LOG_STL_PROC, "Start R2rForCleanRaw", 0, 0);

    Rval = AmbaWrap_memset(&SensorStatus, 0, sizeof(SensorStatus));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
        SensorChan.VinID = pMsg->Index;
        SensorChan.SensorID = 0;
        Rval = AmbaSensor_GetStatus(&SensorChan, &SensorStatus);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaSensor_GetStatus failed! return 0x%x", Rval, 0U);
        }
    }

    if (SensorStatus.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {

        Rval = AmbaWrap_memset(&IsoCfg, 0, sizeof(IsoCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        Rval = PrepareIsoCfg(pMsg->Width, pMsg->Height, &IsoCfg, pMsg, STILL_PROC_TYPE_CLEAN_RAW);

        if (Rval == OK) {
            Rval = AmbaWrap_memset(&RawIn, 0, sizeof(RawIn));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            Rval = AmbaWrap_memset(&RawOut, 0, sizeof(RawOut));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            //SvcLog_DBG(SVC_LOG_STL_PROC, "[R2rForCleanRaw] RawIn.BaseAddr 0x%x", pMsg->BaseAddrY, 0);
            RawIn.Compressed = pMsg->RawCmpr;     /* 1 - compressed raw data, 0 - uncompressed raw data */
            RawIn.BaseAddr = pMsg->BaseAddrY;       /* raw buffer address */
            RawIn.Pitch = pMsg->Pitch;          /* raw buffer pitch */
            RawIn.Window.Width =  pMsg->Width;         /* Window position and size */
            RawIn.Window.Height =  pMsg->Height;
            Rval = AmbaWrap_memset(&CeInfo, 0, sizeof(CeInfo));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            if (pMsg->CeAvail == 1U) {
                UINT16 RawWidth, Pitch;
                CeInfo.BaseAddr = pMsg->BaseAddrUV;
                Rval = AmbaDSP_GetCmprRawBufInfo((pMsg->Width >> 2U), pMsg->RawCmpr, &RawWidth, &Pitch);
                CeInfo.Pitch = Pitch;
                if (Rval == OK) {
                    CeInfo.Window.Width =  CeInfo.Pitch;         /* Window position and size */
                    CeInfo.Window.Height =  RawIn.Window.Height;
                } else {
                    SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_GetCmprRawBufInfo failed, return 0x%x", Rval, 0);
                }
            }
            RawOut.BaseAddr = pMsg->BaseAddrY + ((ULONG)(RawIn.Pitch)*(ULONG)(RawIn.Window.Height));       /* clean raw output buffer address */
            //SvcLog_DBG(SVC_LOG_STL_PROC, "[R2rForCleanRaw] RawOut.BaseAddr 0x%x", RawOut.BaseAddr, 0);
            RawOut.Pitch = pMsg->Pitch;          /* clean raw buffer pitch */
            RawOut.Window.Width =  pMsg->Width;         /* Window position and size */
            RawOut.Window.Height =  pMsg->Height;
            if (Rval == OK) {
                SvcLog_OK(SVC_LOG_STL_PROC, "r2r for clean raw", 0, 0U);
                Rval = AmbaDSP_StillRaw2Raw(&RawIn, &CeInfo, &RawOut, &IsoCfg, STL_R2R_OPT_RESCALE, NULL);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillRaw2Raw failed! return 0x%x", Rval, 0U);
                }
            }
        }
        if (Rval == OK) {
            if (1U == SvcStillCap_GetCleanRawDump()) {
                Rval = SvcStillMux_WaitAvail();
            }
        }
        if (Rval == OK) {
            Rval = AmbaKAL_GetSysTickCount(&StlProc.StlProcMainStart);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == OK) {
            StlProc.RawStat.MainStat = 1;
            if (StlProc_DebugOn > 0U) {
                SvcLog_OK(SVC_LOG_STL_PROC, "MainStat %u", StlProc.RawStat.MainStat, 0);
            }
            Rval = SvcStillProc_WaitProcDone();
        }
    }
    return Rval;
}
#endif

static UINT32 R2yForMain(const SVC_CAP_MSG_s *pMsg)
{
    AMBA_DSP_RAW_BUF_s RawInfo;
    AMBA_DSP_BUF_s CeInfo;
    AMBA_DSP_YUV_IMG_BUF_s SspYuvDst;
    AMBA_DSP_ISOCFG_CTRL_s IsoCfg;
    UINT32 Rval;

    Rval = AmbaWrap_memset(&IsoCfg, 0, sizeof(IsoCfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = PrepareIsoCfg(pMsg->Width, pMsg->Height, &IsoCfg, pMsg, 0);

    if (Rval == OK) {
        Rval = AmbaWrap_memset(&RawInfo, 0, sizeof(RawInfo));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        RawInfo.Compressed = pMsg->RawCmpr;     /* 1 - compressed raw data, 0 - uncompressed raw data */
        RawInfo.BaseAddr = pMsg->BaseAddrY;       /* raw buffer address */
        RawInfo.Pitch = pMsg->Pitch;          /* raw buffer pitch */
        RawInfo.Window.Width =  pMsg->Width;         /* Window position and size */
        RawInfo.Window.Height =  pMsg->Height;
        Rval = AmbaWrap_memset(&CeInfo, 0, sizeof(CeInfo));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        if (pMsg->CeAvail == 1U) {
            UINT16 RawWidth, Pitch;
            CeInfo.BaseAddr = pMsg->BaseAddrUV;
            if (StlProc_DebugOn > 0U) {
                SvcStillLog("[R2yForMain] Pitch %u Height %u, CeInfo.BaseAddr %p", RawInfo.Pitch, RawInfo.Window.Height, CeInfo.BaseAddr, 0, 0);
            }
            Rval = AmbaDSP_GetCmprRawBufInfo((pMsg->Width >> 2U), pMsg->RawCmpr, &RawWidth, &Pitch);
            CeInfo.Pitch = Pitch;
            if (Rval == OK) {
                CeInfo.Window.Width =  CeInfo.Pitch;         /* Window position and size */
                CeInfo.Window.Height =  RawInfo.Window.Height;
            } else {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_GetCmprRawBufInfo failed, return 0x%x", Rval, 0);
            }
        }
        if (Rval == OK) {
            SvcStill_SvcYuv2SspYuv(&StlProc.OutMain[pMsg->CapInstance], &SspYuvDst);
#ifdef CONFIG_SOC_H22
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillRaw2Yuv not defined for H22 yet", 0, 0U);
#else
            StlProc.RawStat.MainStat = 2;
            if (StlProc_DebugOn > 0U) {
                SvcLog_OK(SVC_LOG_STL_PROC, "MainStat %u", StlProc.RawStat.MainStat, 0);
            }
            Rval = AmbaKAL_GetSysTickCount(&StlProc.StlProcMainStart);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            }
            SvcStillProc_WaitProcStart();
            SvcStillLog("[R2yForMain] r2y for main, y addr %p", SspYuvDst.BaseAddrY, 0, 0, 0, 0);
            Rval = AmbaDSP_StillRaw2Yuv(&RawInfo, &CeInfo, &SspYuvDst, &IsoCfg, 0, NULL);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillRaw2Yuv failed! return 0x%x", Rval, 0U);
            } else {
                Rval = SvcStillProc_WaitProcDone();
            }
#endif
        }
    }
    if ((StlProc_DebugOn == 2U) || (StlProc_DebugOn == 3U)) {
        // TBD: using viewzone id 1 to dump still r2y uCode CR
        if (0U != SvcStillMux_CreatePicFile(SVC_STL_TYPE_IDSP, SVC_STL_IDSP_UCODE_CR, 1U, 0U, 0U, 0U)) {
            SvcLog_NG(SVC_LOG_STL_PROC, "Dump ik cr fail!", 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 Y2yForMain(const SVC_CAP_MSG_s *pMsg)
{
    AMBA_DSP_YUV_IMG_BUF_s SspYuvSrc, SspYuvDst;
    AMBA_DSP_ISOCFG_CTRL_s IsoCfg;
    UINT32 Rval;
    ULONG NewInputUVAddr = 0;
    Rval = AmbaWrap_memset(&IsoCfg, 0, sizeof(IsoCfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = PrepareIsoCfg(pMsg->Width, pMsg->Height, &IsoCfg, pMsg, 0);
    /* move source uv data right after y data */
    if (Rval == OK) {
        UINT8 *pSrc, *pDst;
        ULONG UVSize = (ULONG)pMsg->Pitch * (ULONG)pMsg->Height;
        NewInputUVAddr = pMsg->BaseAddrY + UVSize;
        AmbaMisra_TypeCast(&pSrc, &pMsg->BaseAddrUV);
        AmbaMisra_TypeCast(&pDst, &NewInputUVAddr);
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "copy for main yuv", 0, 0);
        }
        Rval = SvcStillBufCopy(pDst, pSrc, ((UINT32)pMsg->Pitch * (UINT32)pMsg->Height));
        if (Rval == SVC_OK) {
            if (0U != SvcPlat_CacheClean(NewInputUVAddr, UVSize)) {
                if (StlProc_DebugOn > 0U) {
                    SvcStillLog("[Y2yForMain] proc dst UV buf %p cache clean fail. if memory is non-cahce, ignore it.", NewInputUVAddr, 0, 0, 0, 0);
                }
            }
        }
    }
    if (Rval == OK) {
        Rval = AmbaWrap_memset(&SspYuvSrc, 0, sizeof(SspYuvSrc));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        Rval = AmbaWrap_memset(&SspYuvDst, 0, sizeof(SspYuvDst));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        SspYuvSrc.DataFmt = pMsg->DataFmt;
        SspYuvSrc.BaseAddrY = pMsg->BaseAddrY;
        SspYuvSrc.BaseAddrUV = NewInputUVAddr;
        SspYuvSrc.Pitch = pMsg->Pitch;
        SspYuvSrc.Window.Width = pMsg->Width;
        SspYuvSrc.Window.Height = pMsg->Height;
        SvcStill_SvcYuv2SspYuv(&StlProc.OutMain[pMsg->CapInstance], &SspYuvDst);

        SvcLog_OK(SVC_LOG_STL_PROC, "y2y for main", 0, 0U);
        Rval = AmbaKAL_GetSysTickCount(&StlProc.StlProcMainStart);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
        }
        StlProc.RawStat.MainStat = 2;
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "MainStat %u", StlProc.RawStat.MainStat, 0);
        }
        SvcStillProc_WaitProcStart();
        Rval = AmbaDSP_StillYuv2Yuv(&SspYuvSrc, &SspYuvDst, &IsoCfg, 0, NULL);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillYuv2Yuv failed! return 0x%x", Rval, 0U);
        } else {
            Rval = SvcStillProc_WaitProcDone();
        }
    }
    return Rval;
}

static UINT32 Y2yForScrn(const UINT16 CapInstance, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg)
{
    AMBA_DSP_YUV_IMG_BUF_s SspYuvSrc, SspYuvDst;
    UINT32 Rval = SVC_OK;
    if (StlProc.OutScrn[CapInstance].Width != 0U) {
        Rval = AmbaWrap_memset(&SspYuvSrc, 0, sizeof(SspYuvSrc));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        Rval = AmbaWrap_memset(&SspYuvDst, 0, sizeof(SspYuvDst));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
        SvcStill_SvcYuv2SspYuv(&StlProc.OutMain[CapInstance], &SspYuvSrc);
        SvcStill_SvcYuv2SspYuv(&StlProc.OutScrn[CapInstance], &SspYuvDst);
        SvcLog_OK(SVC_LOG_STL_PROC, "y2y for scrn", 0, 0U);
        StlProc.RawStat.ScrnStat = 1;
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "ScrnStat %u", StlProc.RawStat.ScrnStat, 0);
        }
        Rval = AmbaKAL_GetSysTickCount(&StlProc.StlProcScrnStart);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
        }
        SvcStillProc_WaitProcStart();
        Rval = AmbaDSP_StillYuv2Yuv(&SspYuvSrc, &SspYuvDst, pIsoCfg, 0, NULL);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillYuv2Yuv failed! return 0x%x", Rval, 0U);
        } else {
            Rval = SvcStillProc_WaitProcDone();
        }
    } else {
        StlProc.RawStat.ScrnStat = 2;
    }
    return Rval;
}

#ifndef CONFIG_SOC_CV2FS
static UINT32 Y2jForMain(const UINT16 CapInstance)
{
    UINT32 Rval = SVC_OK;
    SVC_STL_ENC_INFO_s EncInfo;
    if ((StlProc.PicType[CapInstance] == SVC_STL_TYPE_JPEG) || (StlProc.PicType[CapInstance] == SVC_STL_TYPE_NULL_WR)) {
        Rval = SvcStillMux_WaitAvail();
        if (Rval == OK) {
            EncInfo.RawSeq = StlProc.RawSeq;
            EncInfo.PicView = SVC_STL_MAIN_VIEW;
            Rval = AmbaWrap_memcpy(&EncInfo.YuvBufInfo, &StlProc.OutMain[CapInstance], sizeof(SVC_YUV_IMG_BUF_s));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
            } else {
                EncInfo.PicType = StlProc.PicType[CapInstance];
                SvcLog_OK(SVC_LOG_STL_PROC, "y2j for main", 0, 0U);
                Rval = SvcStillEnc_InputYuv(&EncInfo);
            }
        }
    }
    return Rval;
}
#endif

static UINT32 Y2yForThmb(const UINT16 CapInstance, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg)
{
    AMBA_DSP_YUV_IMG_BUF_s SspYuvSrc, SspYuvDst;
    UINT32 Rval;
    Rval = AmbaWrap_memset(&SspYuvSrc, 0, sizeof(SspYuvSrc));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(&SspYuvDst, 0, sizeof(SspYuvDst));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    if (StlProc.OutScrn[CapInstance].Width != 0U) {
        SvcStill_SvcYuv2SspYuv(&StlProc.OutScrn[CapInstance], &SspYuvSrc);
    } else {
        SvcStill_SvcYuv2SspYuv(&StlProc.OutMain[CapInstance], &SspYuvSrc);
    }
    SvcStill_SvcYuv2SspYuv(&StlProc.OutThmb[CapInstance], &SspYuvDst);
    SvcLog_OK(SVC_LOG_STL_PROC, "y2y for thmb", 0, 0U);
#ifdef CONFIG_SOC_CV2FS
    Rval = SvcStillMux_WaitAvail();
    if (Rval == OK) {
#else
    {
#endif
        StlProc.RawStat.ThmbStat = 1;
        if (StlProc_DebugOn > 0U) {
            SvcLog_OK(SVC_LOG_STL_PROC, "ThmbStat %u", StlProc.RawStat.ThmbStat, 0);
        }
        Rval = AmbaKAL_GetSysTickCount(&StlProc.StlProcThmbStart);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
        }
        SvcStillProc_WaitProcStart();
        Rval = AmbaDSP_StillYuv2Yuv(&SspYuvSrc, &SspYuvDst, pIsoCfg, 0, NULL);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillYuv2Yuv failed! return 0x%x", Rval, 0U);
        } else {
            Rval = SvcStillProc_WaitProcDone();
        }
    }
    return Rval;
}

#ifndef CONFIG_SOC_CV2FS
static UINT32 Y2jForScrn(const UINT16 CapInstance)
{
    UINT32 Rval = SVC_OK;
    SVC_STL_ENC_INFO_s EncInfo;
    if ((StlProc.PicType[CapInstance] == SVC_STL_TYPE_JPEG) || (StlProc.PicType[CapInstance] == SVC_STL_TYPE_NULL_WR)) {
        Rval = SvcStillEnc_WaitJpegDone();
        if (Rval == OK) {
            Rval = SvcStillMux_WaitAvail();
        }
        if (Rval == OK) {
            if (StlProc.OutScrn[CapInstance].Width != 0U) {
                EncInfo.RawSeq = StlProc.RawSeq;
                EncInfo.PicView = SVC_STL_SCRN_VIEW;
                Rval = AmbaWrap_memcpy(&EncInfo.YuvBufInfo, &StlProc.OutScrn[CapInstance], sizeof(SVC_YUV_IMG_BUF_s));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
                } else {
                    EncInfo.PicType = StlProc.PicType[CapInstance];
                    SvcLog_OK(SVC_LOG_STL_PROC, "y2j for scrn", 0, 0U);
                    Rval = SvcStillEnc_InputYuv(&EncInfo);
                }
            } else {
                Rval = SvcStillEnc_DummyInput(SVC_STL_SCRN_VIEW);
            }
        }
    }
    return Rval;
}
#endif

#ifndef CONFIG_SOC_CV2FS
static void Y2jForThmb(const UINT16 CapInstance)
{
    UINT32 Rval = SVC_OK;
    SVC_STL_ENC_INFO_s EncInfo;
    if (((StlProc.PicType[CapInstance] == SVC_STL_TYPE_JPEG) || (StlProc.PicType[CapInstance] == SVC_STL_TYPE_NULL_WR)) && (StlProc.OutScrn[CapInstance].Width != 0U)) {
        Rval = SvcStillEnc_WaitJpegDone();
        if (Rval == OK) {
            Rval = SvcStillMux_WaitAvail();
        }
    }
    if (Rval == OK) {
        EncInfo.RawSeq = StlProc.RawSeq;
        EncInfo.PicView = SVC_STL_THMB_VIEW;
        Rval = AmbaWrap_memcpy(&EncInfo.YuvBufInfo, &StlProc.OutThmb[CapInstance], sizeof(SVC_YUV_IMG_BUF_s));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
        } else {
            EncInfo.PicType = StlProc.PicType[CapInstance];
            SvcLog_OK(SVC_LOG_STL_PROC, "y2j for thmb", 0, 0U);
            Rval = SvcStillEnc_InputYuv(&EncInfo);
        }
    }
    if (Rval == OK) {
        Rval = SvcStillEnc_WaitJpegDone();
    }
    if (Rval == OK) {
        StlProc.RawSeq ++;
        Rval = AmbaWrap_memset(&StlProc.RawStat, 0, sizeof(SVC_STL_PROC_STAT_s));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        }
    }
}
#endif

static UINT32 RawProcess(const SVC_CAP_MSG_s *pInput)
{
    UINT32 Rval;

#ifdef SVC_STL_ENABLE_CLEAN_RAW
    Rval = R2rForCleanRaw(pInput);
#endif
#ifdef ENABLE_R2R
    Rval = R2rForMain(pInput);
    if (OK == Rval) {
        Rval = R2yForMain(pInput);
        //AmbaKAL_TaskSleep(5000);
    }
#else
    Rval = R2yForMain(pInput);
#endif

    return Rval;
}

static UINT32 YuvProcess(const SVC_CAP_MSG_s *pInput)
{
    UINT32 Rval;
    if ((pInput->Width == StlProc.OutMain[pInput->CapInstance].Width) && (pInput->Height == StlProc.OutMain[pInput->CapInstance].Height)) {
        Rval = CopyMainYuv(pInput);
    } else {
        Rval = Y2yForMain(pInput);
    }
    return Rval;
}

static void Y2yForQview(const UINT16 CapInstance, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg)
{
    UINT32 Rval = SVC_OK;
    const SVC_STILL_QVIEW_CFG_s * pQview = &(StlProc.Qview);

    if (pQview->Enable == 1U) {
        UINT32 i;

        UINT32 NumQview = pQview->NumQview;

        if ( NumQview > AMBA_DSP_MAX_VOUT_NUM ) {
            SvcLog_NG(SVC_LOG_STL_PROC, "CheckQview NumQview %u >  AMBA_DSP_MAX_VOUT_NUM %u", NumQview, AMBA_DSP_MAX_VOUT_NUM);
            Rval = SVC_NG;
        }
        if (SVC_OK == Rval) {
            UINT32 TimeBeg, TimeEnd;
            Rval = AmbaKAL_GetSysTickCount(&TimeBeg);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            }

            for (i = 0; i < NumQview; i ++) {
                const SVC_STILL_QVIEW_WINDOW_s *pWin = &(pQview->Win[i]);
                AMBA_DSP_YUV_IMG_BUF_s SspYuvSrc, SspYuvDst;

                AmbaPrint_PrintUInt5("Y2yForQview qview[%u] %u x %u VoutId %u", i, pWin->Width, pWin->Height, pWin->VoutId, 0u);

                Rval = AmbaWrap_memset(&SspYuvSrc, 0, sizeof(SspYuvSrc));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                }
                Rval = AmbaWrap_memset(&SspYuvDst, 0, sizeof(SspYuvDst));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                }
                SvcStill_SvcYuv2SspYuv(&(StlProc.OutMain[CapInstance]), &SspYuvSrc);
                SvcStill_SvcYuv2SspYuv(&(StlProc.OutQview[i]), &SspYuvDst);

                SvcLog_OK(SVC_LOG_STL_PROC, "y2y for quick view [%u]", i, 0U);
                StlProc.RawStat.QviewStat = 1;
                SvcStillProc_WaitProcStart();
                Rval = AmbaDSP_StillYuv2Yuv(&SspYuvSrc, &SspYuvDst, pIsoCfg, 0, NULL);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_StillYuv2Yuv failed! return 0x%x", Rval, 0U);
                } else {
                    Rval = SvcStillProc_WaitProcDone();
                    if ((OK == Rval) && (StlProc.RawStat.QviewStat == 2U)) {
                        SvcStillCap_SetQview(&SspYuvSrc, (UINT8)pWin->VoutId);
                    }
                }

                if (Rval != 0U) {
                    break;
                }
            }

            Rval = AmbaKAL_GetSysTickCount(&TimeEnd);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            }
            if (TimeEnd >= TimeBeg) {
                StlProc.StlProcTime[SVC_STL_QVIEW_VIEW] = TimeEnd - TimeBeg;
            } else {
                StlProc.StlProcTime[SVC_STL_QVIEW_VIEW] = (0xFFFFFFFFU - TimeBeg) + TimeEnd;
            }
        }
    }

}

static void* SvcStillProc_TaskEntry(void* EntryArg)
{
    SVC_CAP_MSG_s Input;
    UINT32 Rval;
    AMBA_DSP_ISOCFG_CTRL_s IsoCfg = {0};
    ULONG  ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while ( ArgVal != 0xCafeU ) {
        Rval = AmbaKAL_MsgQueueReceive(&StlProc.StlProcQue, &Input, AMBA_KAL_WAIT_FOREVER);
        if (OK == Rval) {
            //SvcLog_OK(SVC_LOG_STL_PROC, "*** RawStat %u %u", StlProc.RawStat.MainStat, StlProc.RawStat.ScrnStat);
            //SvcLog_OK(SVC_LOG_STL_PROC, "***         %u", StlProc.RawStat.ThmbStat, 0);
            Rval = AmbaWrap_memset(&StlProc.StlProcTime[0], 0, (UINT32)4 * SVC_STL_NUM_VIEW);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            if (StlProc_DebugOn > 0U) {
                SvcLog_OK(SVC_LOG_STL_PROC, "Input w %u h %u", Input.Width, Input.Height);
            }
            SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_MCTF);
            if (Input.CapType == SVC_FRAME_TYPE_RAW) {
                Rval = RawProcess(&Input);
                while (StlProc.TestFlag == SVC_STL_TEST_PAUSE_AFTER_R2Y) {
                    Rval = AmbaKAL_TaskSleep(10);
                }
            } else {
                Rval = YuvProcess(&Input);
            }
            Input.CapType = SVC_FRAME_TYPE_YUV;
            if (OK == Rval) {
                Rval = PrepareIsoCfg(StlProc.OutMain[Input.CapInstance].Width, StlProc.OutMain[Input.CapInstance].Height, &IsoCfg, &Input, 0);
            }

            if (Rval == OK) {
                Y2yForQview(Input.CapInstance, &IsoCfg);
            }

#ifndef CONFIG_SOC_CV2FS
            if (Rval == OK) {
                Rval = Y2jForMain(Input.CapInstance);
            }
#endif
            if (Rval == OK) {
                Rval = Y2yForScrn(Input.CapInstance, &IsoCfg);
            }
            if (Rval == OK) {
                if (StlProc.OutScrn[Input.CapInstance].Width != 0U) {
                    Rval = PrepareIsoCfg(StlProc.OutScrn[Input.CapInstance].Width, StlProc.OutScrn[Input.CapInstance].Height, &IsoCfg, &Input, 0);
                } else {
                    StlProc.StlProcTime[SVC_STL_SCRN_VIEW] = 0;
                }
            }
            if (Rval == OK) {
                Rval = Y2yForThmb(Input.CapInstance, &IsoCfg);
                if (Rval == OK) {
                    StlProc.NumThumb ++;
                    if (StlProc_DebugOn > 0U) {
                        SvcLog_DBG(SVC_LOG_STL_PROC, "NumThumb %u NumCap %u", StlProc.NumThumb, StlProc.NumCap);
                    }
                    if (StlProc.NumCap == StlProc.NumThumb) {
                        Rval = LiveviewRestart(Rval);
                    }
                }
            }

#ifndef CONFIG_SOC_CV2FS
            if (Rval == OK) {
                Rval = Y2jForScrn(Input.CapInstance);
                //AmbaKAL_TaskSleep(1000);
            }
#endif
            if (Rval == OK) {
                SvcLog_OK(SVC_LOG_STL_PROC, "still r2r time: %u ms", StlProc.StlR2rTime, 0);
                SvcLog_OK(SVC_LOG_STL_PROC, "still proc time: main %u ms, scrn %u ms", StlProc.StlProcTime[SVC_STL_MAIN_VIEW], StlProc.StlProcTime[SVC_STL_SCRN_VIEW]);
                SvcLog_OK(SVC_LOG_STL_PROC, "          qview %u ms", StlProc.StlProcTime[SVC_STL_QVIEW_VIEW], 0);
                SvcLog_OK(SVC_LOG_STL_PROC, "          thm %u ms, total %u ms", StlProc.StlProcTime[SVC_STL_THMB_VIEW], StlProc.StlProcTime[SVC_STL_MAIN_VIEW] + StlProc.StlProcTime[SVC_STL_SCRN_VIEW] + StlProc.StlProcTime[SVC_STL_THMB_VIEW]);
#ifdef CONFIG_SOC_CV2FS
                StlProc.RawSeq ++;
                Rval = AmbaWrap_memset(&StlProc.RawStat, 0, sizeof(SVC_STL_PROC_STAT_s));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                }
#else
                Y2jForThmb(Input.CapInstance);
                //AmbaKAL_TaskSleep(1000);
#endif
                if (StlProc.NumCap == StlProc.NumThumb) {
                    SvcStillCap_ClearStatus(SVC_STL_PROC_BUSY);
                    StlProc.NumThumb = 0;
                }
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

/**
* turn on/off debugging
* @param [in] On 1 to turn on debugging
*/
void SvcStillProc_Debug(UINT8 On)
{
    StlProc_DebugOn = On;
    SvcLog_OK(SVC_LOG_STL_PROC, "SvcStillProc_Debug %u", On, 0);
}

/**
* initialize and create still process task
* @param [in] pCfg pointer to configuration of still process
* @return 0-OK, 1-NG
*/
UINT32 SvcStillProc_Create(const SVC_STL_PROC_CONFIG_s *pCfg)
{
    UINT32                  Rval;
    static UINT8            SvcStillProc[SVC_STL_PROC_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char             MsgQueName[] = "StlProcQue";
    static char             FlgName[] = "StlProcFlg";

    if (StlProc_Created == 0U) {
        Rval = AmbaWrap_memset(&StlProc, 0, sizeof(StlProc));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_MUX, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        } else {
            /* task create */
            StlProc.TaskCtrl.Priority    = pCfg->Priority;
            StlProc.TaskCtrl.EntryFunc   = SvcStillProc_TaskEntry;
            StlProc.TaskCtrl.EntryArg    = 0U;
            StlProc.TaskCtrl.pStackBase  = SvcStillProc;
            StlProc.TaskCtrl.StackSize   = SVC_STL_PROC_STACK_SIZE;
            StlProc.TaskCtrl.CpuBits     = pCfg->CpuBits;
            StlProc.WorkBufAddr = pCfg->WorkBufAddr;
            StlProc.WorkBufSize = pCfg->WorkBufSize;
            StlProc.IkCtxId = pCfg->IkCtxId;
            Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STL_YUV_DATA_RDY, YuvDataReady);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_EventHandlerRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_YUV_DATA_RDY, Rval);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STL_CFA_AAA_RDY, CfaStatReady);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_EventHandlerRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_CFA_AAA_RDY, Rval);
            }
        }
#ifdef SVC_STL_ENABLE_CLEAN_RAW
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY, CleanRawReady);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_EventHandlerRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY, Rval);
            }
        }
#endif
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueCreate(&StlProc.StlProcQue, MsgQueName, (UINT32)sizeof(SVC_CAP_MSG_s), StlProc.StlProcQueBuf, (UINT32)sizeof(StlProc.StlProcQueBuf));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "StlProcQue created failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagCreate(&StlProc.StlProcFlg, FlgName);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "Event flag create failed with 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagClear(&StlProc.StlProcFlg, 0xFFFFFFFFU);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_ENC, "Event flag clear failed with 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = SvcTask_Create("SvcStillProc", &StlProc.TaskCtrl);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_PROC, "SvcStillProc_Task created failed! return 0x%x", Rval, 0U);
            } else {
                StlProc_Created = 1;
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

/**
* de-initialize and delete still process task
* @return 0-OK, 1-NG
*/
UINT32 SvcStillProc_Delete(void)
{
    UINT32 Rval;

    if (StlProc_Created == 1U) {
        Rval = SvcTask_Destroy(&StlProc.TaskCtrl);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_PROC, "SvcStillProc_Task delete failed! return 0x%x", Rval, 0U);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagDelete(&StlProc.StlProcFlg);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "Flag delete failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueDelete(&StlProc.StlProcQue);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "StlProcQue delete failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STL_CFA_AAA_RDY, CfaStatReady);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_EventHandlerUnRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_CFA_AAA_RDY, Rval);
            }
        }
#ifdef SVC_STL_ENABLE_CLEAN_RAW
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY, CleanRawReady);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_EventHandlerUnRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY, Rval);
            }
        }
#endif
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STL_YUV_DATA_RDY, YuvDataReady);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_PROC, "AmbaDSP_EventHandlerUnRegister event %u fail, return 0x%x", AMBA_DSP_EVENT_STL_YUV_DATA_RDY, Rval);
            }
        }
        if (Rval == SVC_OK) {
            StlProc_Created = 0;
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}
