/**
 *  @file SvcCvLoggerTask.c
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
 *  @details svc cv logger task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaVfs.h"
#include "AmbaSD.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcWrap.h"
#include "SvcRecMain.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvLogger.h"

/* app-icam */
#include "SvcBufMap.h"
#include "SvcUserPref.h"
#include "SvcCvAppDef.h"
#include "SvcCvLoggerTask.h"

#include "RefFlow_Common.h"
#include "RefFlow_BSD.h"
#include "RefFlow_FC.h"
#include "AmbaOD_2DBbx.h"

#define SVC_LOG_CV_LOGGER_TASK     "CV_LGR_TASK"

static inline UINT32 CV_LOGGER_ADD_BIT     (UINT32 Bits, UINT32 Idx) {return (Bits | ((UINT32)1U << Idx));}
static inline UINT32 CV_LOGGER_REMOVE_BIT  (UINT32 Bits, UINT32 Idx) {return (Bits & (~((UINT32)1U << Idx)));}

static SVC_CV_LOGGER_HDLR_s CvLoggerHdlr[SVC_CV_FLOW_CHAN_MAX];
static UINT8 CvLoggerEnable[SVC_CV_FLOW_CHAN_MAX] = {0};
static UINT32 VidRecStrmId[SVC_CV_FLOW_CHAN_MAX];
static UINT32 VidRecEventBits[CONFIG_ICAM_MAX_REC_STRM] = {0};

static void   CvLoggerTask_GetDefaultFileName(UINT32 CvFlowChan, const char **pFileName);
static UINT32 CvLoggerTask_FindVideoRecIdx(UINT32 CvFlowChan, UINT32 *pRecIdx);
static void   CvLoggerTask_VideoRecCallback(UINT32 Event, const void *pInfo);

/**
 *  Start the CV logger task
 *  @param [in] CvFlowChanBits CvFlow channel bits to indicate which CvFlow channel to be started
 *  @return 0-OK, 1-NG
 */
UINT32 SvcCvLoggerTask_Start(UINT32 CvFlowChanBits)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    SVC_CV_LOGGER_CFG_s LoggerCfg;
    UINT32 VidRecIdx;
    static UINT8  CvLoggerTaskInit = 0U;

    SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLoggerTask_Start CvFlowChanBits(0x%x)", CvFlowChanBits, 0U);

    if (CvLoggerTaskInit == 0U) {
        ULONG BufBase;
        SVC_CV_LOGGER_INIT_CFG_s InitCfg;

        if (SVC_OK == SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_CV_LOGGER, &BufBase, &InitCfg.MemSize)) {
            InitCfg.MaxLogger = CONFIG_ICAM_CV_LOGGER_MAX_NUM;
            InitCfg.MemBase = BufBase;
            if (SVC_OK != SvcCvLogger_Init(&InitCfg)) {
                SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "fail to SvcCvLogger_Init", 0U, 0U);
            }

            CvLoggerTaskInit = 1U;
        } else {
            SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "fail to SvcBuffer_Request", 0U, 0U);
        }

        SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLogger_Init done", 0U, 0U);
    }

    /* Register output callback */
    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowChanBits & ((UINT32) 1U << i)) > 0U) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                /* Create logger service */
                CvLoggerTask_GetDefaultFileName(i, &LoggerCfg.pFileName);
                if (SVC_OK == SvcCvLogger_Create(&LoggerCfg, &CvLoggerHdlr[i])) {
                    CvLoggerEnable[i] = 1U;
                } else {
                    SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLogger_Create(%d) failed(0x%x)", i, RetVal);
                    RetVal = SVC_NG;
                }

                /* Link with video rec */
                if (1U == CvLoggerTask_FindVideoRecIdx(i, &VidRecIdx)) {
                    VidRecStrmId[i] = VidRecIdx;
                    VidRecEventBits[VidRecIdx] = CV_LOGGER_ADD_BIT(VidRecEventBits[VidRecIdx], i);
                    SvcRecMain_EventCbRegister(((UINT32)1U << VidRecIdx), (UINT32)SVC_RCM_EVT_FSTATUS, CvLoggerTask_VideoRecCallback);
                    SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "VidRecIdx = %d VidRecEventBits= 0x%x", VidRecIdx, VidRecEventBits[VidRecIdx]);

                } else {
                    SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "Cannot link chan(%d) with video rec", i, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "CvFlowChan(%d) is not enabled", i, 0U);
            }
        }
    }

    SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLoggerTask_Start Done", 0U, 0U);

    return RetVal;
}

/**
 *  Stop the CV logger task
 *  @param [in] CvFlowChanBits CvFlow channel bits to indicate which CvFlow channel to be stopped
 *  @return 0-OK, 1-NG
 */
UINT32 SvcCvLoggerTask_Stop(UINT32 CvFlowChanBits)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    UINT32 VidRecIdx;

    SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLoggerTask_Stop CvFlowChanBits(0x%x)", CvFlowChanBits, 0U);

    /* Stop CvCtrl */
    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowChanBits & ((UINT32)1U << i)) > 0U) {
            if ((CvFlowBits & ((UINT32)1U << i)) > 0U) {
                CvLoggerEnable[i] = 0U;

                VidRecIdx = VidRecStrmId[i];
                VidRecEventBits[VidRecIdx] = CV_LOGGER_REMOVE_BIT(VidRecEventBits[VidRecIdx], i);
                if (VidRecEventBits[VidRecIdx] == 0U) {
                    SvcRecMain_EventCbRegister(((UINT32)1U << VidRecIdx), (UINT32)SVC_RCM_EVT_FSTATUS, NULL);
                }

                RetVal = SvcCvLogger_Delete(&CvLoggerHdlr[i]);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLogger_Delete(%d) failed(0x%x)", i, RetVal);
                }
            } else {
                SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "CvFlowChan(%d) is not enabled", i, 0U);
            }
        }
    }

    SvcLog_DBG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLoggerTask_Stop Done", 0U, 0U);

    return RetVal;
}

/**
 *  Check if the CV logger task is enabled or not
 *  @param [in] CvFlowChan CvFlow channel
 *  @return 0-OK, 1-NG
 */
UINT32 SvcCvLoggerTask_IsEnable(UINT32 CvFlowChan)
{
    return CvLoggerEnable[CvFlowChan];
}

/**
 *  Get a buffer space to write a trunk
 *  @param [in] CvFlowChan CvFlow channel
 *  @param [in] ReqSize Required buffer size
 *  @param [in] Flag Flag to indicate the attribute of the trunk
 *  @param [out] pWrBuf pointer to write buffer
 *  @return 0-OK, 1-NG
 */
UINT32 SvcCvLoggerTask_TrunkBufferGet(UINT32 CvFlowChan, UINT32 ReqSize, UINT32 Flag, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf)
{
    UINT32 RetVal;

    if (CvLoggerEnable[CvFlowChan] == 1U) {
        RetVal = SvcCvLogger_TrunkBufferGet(&CvLoggerHdlr[CvFlowChan], ReqSize, Flag, pWrBuf);
        if (RetVal != SVC_OK) {
            RetVal = SvcCvLoggerTask_Stop(((UINT32)1U << CvFlowChan));
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
*  Update the written trunk data
*  @param [in] CvFlowChan CvFlow channel
*  @param [in] pWrBuf pointer to write buffer
*  @param [out] WroteSize used size
*  @return 0-OK, 1-NG
*/
UINT32 SvcCvLoggerTask_TrunkBufferUpdate(UINT32 CvFlowChan, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf, UINT32 WroteSize)
{
    UINT32 RetVal;

    if (CvLoggerEnable[CvFlowChan] == 1U) {
        RetVal = SvcCvLogger_TrunkBufferUpdate(&CvLoggerHdlr[CvFlowChan], pWrBuf, WroteSize);
        if (RetVal != SVC_OK) {
            RetVal = SvcCvLoggerTask_Stop(((UINT32)1U << CvFlowChan));
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void CvLoggerTask_GetDefaultFileName(UINT32 CvFlowChan, const char **pFileName)
{
    UINT32 RetVal = SVC_OK;
    static char NewFileName[64] = {"C:\\CvLogger\\cvout_ch00_00.cv"};
    char Text[2];
    UINT8 UnitsD, TensD;
    static UINT8 FileIndex[SVC_CV_FLOW_CHAN_MAX] = {0};
    SVC_USER_PREF_s *pSvcUserPref;

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK == RetVal) {
        if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL0) {
            NewFileName[0] = 'c';
        } else if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
            NewFileName[0] = 'd';
        } else {
            SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "Unsupported storage", 0U, 0U);
        }
    }

    /* Channel index */
    TensD = (UINT8)(CvFlowChan / 10U);
    UnitsD = (UINT8)(CvFlowChan - ((UINT32)TensD * 10U));
    RetVal = AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), TensD, 10);
    if (1U != RetVal) {
        SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
    }
    NewFileName[20] = Text[0];
    RetVal = AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), UnitsD, 10);
    if (1U != RetVal) {
        SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
    }
    NewFileName[21] = Text[0];

    /* File index */
    TensD = FileIndex[CvFlowChan] / 10U;
    UnitsD = FileIndex[CvFlowChan] - (TensD * 10U);
    RetVal = AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), TensD, 10);
    if (1U != RetVal) {
        SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
    }
    NewFileName[23] = Text[0];
    RetVal = AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), UnitsD, 10);
    if (1U != RetVal) {
        SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
    }
    NewFileName[24] = Text[0];
    FileIndex[CvFlowChan]++;
    if (FileIndex[CvFlowChan] >= 100U) {
        FileIndex[CvFlowChan] = 0U;
    }

    *pFileName = &NewFileName[0];
}

static void CvLoggerTask_GenVideoLogName(UINT32 CvFlowChan, const char *pVidFileName, const char **pFileName)
{
    UINT32 RetVal = SVC_OK;
    static const char Path[] = "C:\\CvLogger";
    const char  *pStrBuf[1U];
    static char NewFileName[64];
    char *pVidStr = NULL;
    char *pStr = NULL;
    char Text[2];

    pStrBuf[0] = Path;
    RetVal = AmbaUtility_StringPrintStr(NewFileName, (UINT32)sizeof(NewFileName), "%s", 1U, pStrBuf);
    if (RetVal != 0U) {
        NewFileName[0] = pVidFileName[0];

        AmbaMisra_TypeCast(&pVidStr, &pVidFileName);
        pVidStr = SvcWrap_strrchr(pVidStr, (INT32)'\\');
        if (pVidStr != NULL) {
            pVidStr = SvcWrap_strrchr(pVidStr, (INT32)'\\');
        }

        AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), pVidStr);

        pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
        if (pStr != NULL) {
            RetVal = AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), CvFlowChan, 10);
            if (1U != RetVal) {
                SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
            }

            pStr[0] = '_';
            pStr[1] = Text[0];
            pStr[2] = '.';
            pStr[3] = 'c';
            pStr[4] = 'v';
            pStr[5] = '\0';
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "CvLoggerTask_GenVideoLogName failed(0x%x)", RetVal, 0U);
    }

    *pFileName = &NewFileName[0];

    AmbaMisra_TouchUnused(&CvFlowChan);
}

static UINT32 CvLoggerTask_FindVideoRecIdx(UINT32 CvFlowChan, UINT32 *pRecIdx)
{
    UINT32 i, j;
    UINT32 Found = 0U;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 StrmId = pResCfg->CvFlow[CvFlowChan].InputCfg.Input[0].StrmId;
    const SVC_REC_STRM_s *pRecStrm;

    if (StrmId < AMBA_DSP_MAX_VIEWZONE_NUM) {
        for (i = 0; i < pResCfg->RecNum; i++) {
            pRecStrm = &pResCfg->RecStrm[i];
            for (j = 0; j < pRecStrm->StrmCfg.NumChan; j++) {
                if (StrmId == pRecStrm->StrmCfg.ChanCfg[j].FovId) {
                    Found = 1U;
                    *pRecIdx = i;
                    break;
                }
            }
            if (Found == 1U) {
                break;
            }
        }
    }

    return Found;
}

static void CvLoggerTask_VideoRecCallback(UINT32 Event, const void *pInfo)
{
    UINT32 RetVal;
    const AMBA_RDT_FSTATUS_INFO_s *pStatus;
    SVC_CV_LOGGER_VIDEO_STATUS_s VidSt;
    UINT32 i, CtrlType;

    if (Event == SVC_RCM_EVT_FSTATUS) {
        AmbaMisra_TypeCast(&pStatus, &pInfo);

        AmbaPrint_PrintUInt5("[CV_LGR_TASK] VideoRecCallback: RecStrmId(%d), PTS(%lld), IsLast(%d), EventBits(0x%x)",
                  pStatus->RecStrmId, (UINT32)pStatus->VidCapPts, pStatus->IsLastFile, VidRecEventBits[pStatus->RecStrmId], 0U);

        if (pStatus->RecStrmId < (UINT32)CONFIG_ICAM_MAX_REC_STRM) {
            for (i = 0U; i < SVC_CV_FLOW_CHAN_MAX; i++) {
                if (CvLoggerEnable[i] == 1U) {
                    if ((VidRecEventBits[pStatus->RecStrmId] & ((UINT32) 1U << i)) > 0U) {
                        if (pStatus->IsLastFile == 1U) {
                            /* End of video recording. Use logger internal file name. */
                            CvLoggerTask_GetDefaultFileName(i, &VidSt.pFileName);
                        } else {
                            /* Use new video file name. */
                            CvLoggerTask_GenVideoLogName(i, pStatus->FileName, &VidSt.pFileName);
                        }

                        VidSt.IsLastFile = pStatus->IsLastFile;
                        VidSt.VidCapPts  = pStatus->VidCapPts;

                        if (pStatus->FileStatus == AMBA_RDT_FSTATUS_FOPEN) {
                            CtrlType = SVC_CV_LOGGER_CTRL_VIDEO_START;
                        } else {
                            CtrlType = SVC_CV_LOGGER_CTRL_VIDEO_CLOSE;
                        }
                        RetVal = SvcCvLogger_Ctrl(&CvLoggerHdlr[i], CtrlType, &VidSt);
                        if (RetVal != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "SvcCvLogger_Ctrl(%d) failed(0x%x)", i, RetVal);
                        }
                    }
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_LOGGER_TASK, "VideoRecCallback recv invalid RecStrmId = %d", pStatus->RecStrmId, 0U);
        }
    }
}
