/**
 *  @file SvcFexTask.c
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
 *  @details svc cv feature extraction task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaSvcWrap.h"
#include "AmbaSD.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcWrap.h"
#include "SvcPlat.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_Fex.h"
#include "SvcCvCamCtrl.h"

#if defined (CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#else
#include "cvapi_flexidag_ambaspufex_cv2.h"
#endif

/* svc-app */
#include "SvcCvAppDef.h"
#include "SvcFexTask.h"
#include "SvcBufMap.h"
#include "SvcUserPref.h"
#include "SvcStereoTask.h"

#define SVC_LOG_FEX_TASK            "FexTask"

#define SVC_FEX_OSD_LEVEL        (0x4)
#define SVC_FEX_OUT_BUF_NUM      (2U)
#define SVC_FEX_OUT_BUF_SIZE     (sizeof(SVC_CV_FLOW_FEX_OUTPUT_s))

#define SVC_FEX_OSD_SHOW         (0x01U)
#define SVC_FEX_OSD_UPDATE       (0x02U)
#ifdef CONFIG_ICAM_32BITS_OSD_USED
#define OSD_COLOR                (0xFFFFD700U)
#else
#define OSD_COLOR                (32U)
#endif
#define OSD_LINE_WIDTH           (2U)
#define OSD_LINE_LEN             (2U)

typedef struct {
    UINT32  Used     :1;
    UINT32  DispOn   :1;
    UINT32  Reserved :30;
    UINT8   ID;
    UINT32  CvFlowChan;
    UINT32  CvFlowCbRegID;

    UINT8   OutBufIdx;
    UINT8   OutBuf[SVC_FEX_OUT_BUF_NUM][SVC_FEX_OUT_BUF_SIZE];
} SVC_FEX_TASK_HDLR_s;

static UINT8                   FexTaskInit = 0;
static UINT8                   FexCvfEnable = 0;
static SVC_FEX_TASK_HDLR_s     FexHdlr[SVC_FEX_TASK_MAX_HDLR];
static UINT8                   FexDebugEnable = 2U;

/* Definition for OSD display */
typedef struct {
    UINT8   VoutID;
    UINT8   DispStrmIdx;
    UINT8   DispStrmChanIdx;
    UINT32  SrcWidth;
    UINT32  SrcHeight;

    UINT8   GuiLevel;
    REGION_s OsdReg;
} SVC_FEX_DISP_INFO_s;

static SVC_FEX_DISP_INFO_s     FexDispInfo[SVC_FEX_TASK_MAX_HDLR];
static UINT8                   FexOsdUpdate[SVC_FEX_TASK_MAX_HDLR] = {0};

/* Definition for File input */
typedef struct {
    ULONG   BufBase;
    UINT32  BufSize;
    ULONG   BufCurAddr;
} SVC_FEX_TASK_FILE_IN_CTRL_s;
static SVC_FEX_TASK_FILE_IN_CTRL_s FexFileInCtrl;

static UINT32 FexTask_GreateHdlr(SVC_FEX_TASK_HDLR_s **pHdlr);
static UINT32 FexTask_CvfChan2Hdlr(UINT32 CvFlowChan, SVC_FEX_TASK_HDLR_s **pHdlr);
static void   FexTask_CvFlowOutputHandler(UINT32 CvFlowChan, UINT32 CvType, void *pOutput);

static UINT32 FexTask_GuiLevel2Hdlr(UINT32 GuiLevel, SVC_FEX_TASK_HDLR_s **pHdlr);
static void   FexTask_CalcOSDRegion(UINT32 DispStrmIdx, UINT32 DispStrmChanIdx, REGION_s *pOsdReg);
static UINT32 FexTask_UpdateDispInfo(UINT32 CvflowChan, SVC_FEX_DISP_INFO_s *pDispInfo);
static void   FexTask_OsdDraw(UINT32 VoutIdx, UINT32 Level);
static void   FexTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void   FexTask_OsdDrawFex(const SVC_FEX_TASK_HDLR_s *pHdlr);

static AMBA_KAL_MUTEX_t FexMutex;

static void Fex_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&FexMutex, 5000)) {
        SvcLog_NG(SVC_LOG_FEX_TASK, "Fex_MutexTake: timeout", 0U, 0U);
    }
}

static void Fex_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&FexMutex)) {
        SvcLog_NG(SVC_LOG_FEX_TASK, "Fex_MutexGive: error", 0U, 0U);
    }
}

static void SVC_FEX_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (FexDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void SVC_FEX_DBG_INFO(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (FexDebugEnable > 1U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static UINT32 FexTask_GreateHdlr(SVC_FEX_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_FEX_TASK_MAX_HDLR; i++) {
            if (0U == FexHdlr[i].Used) {
                FexHdlr[i].Used = 1U;
                FexHdlr[i].ID = (UINT8)i;
                *pHdlr = &FexHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }
    return RetVal;
}

static UINT32 FexTask_CvfChan2Hdlr(UINT32 CvFlowChan, SVC_FEX_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_FEX_TASK_MAX_HDLR; i++) {
            if ((1U == FexHdlr[i].Used) && (CvFlowChan == FexHdlr[i].CvFlowChan)) {
                *pHdlr = &FexHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }
    return RetVal;
}

#if 0
static UINT32 FexTask_AllocBuf(SVC_FEX_TASK_FILE_IN_CTRL_s *pMgr, UINT32 ReqSize, void **pBufAddr, UINT32 *pBufSize)
{
    #define BUF_ALIGN_VALUE  128
    UINT32 RetVal = SVC_OK;
    ULONG  BufLimit = pMgr->BufBase + pMgr->BufSize - 1U;
    ULONG  BufAddrAlign;
    UINT32 ReqSizeAlign;

    BufAddrAlign = AmbaSvcWrap_GetAlignedAddr(pMgr->BufCurAddr, BUF_ALIGN_VALUE);
    ReqSizeAlign = GetAlignedValU32(ReqSize, BUF_ALIGN_VALUE);

    if ((BufAddrAlign + ReqSizeAlign) <= BufLimit) {
        AmbaMisra_TypeCast(pBufAddr, &BufAddrAlign);
        *pBufSize = ReqSizeAlign;
        pMgr->BufCurAddr += ReqSizeAlign;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void FexTask_ResetBuf(SVC_FEX_TASK_FILE_IN_CTRL_s *pMgr)
{
    pMgr->BufCurAddr = pMgr->BufBase;
}

static UINT32 FexTask_ReadFile(const char *pFileName, void *pBuf, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFile = NULL;
    UINT32 DataSize;

    RetVal = AmbaFS_FileOpen(pFileName, "rb", &pFile);
    if ((RetVal == 0U) && (pFile != NULL)) {
        if (0U == AmbaFS_FileRead(pBuf, 1, Size, pFile, &DataSize)) {
            if (DataSize != Size) {
                RetVal = 3U;
            }
        } else {
            RetVal = 2U;
        }

        if (0U != AmbaFS_FileClose(pFile)) {
            RetVal |= 0x10U;
        }
    } else {
        RetVal = 1U;
    }

    return RetVal;
}

static void FexTask_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 FexTask_FileWrite(const char *pFileName, void *pBuf, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFile = NULL;
    UINT32 DataSize;

    RetVal = AmbaFS_FileOpen(pFileName, "w", &pFile);
    if ((RetVal == 0U) && (pFile != NULL)) {
        RetVal = AmbaFS_FileWrite(pBuf, 1, Size, pFile, &DataSize);
        if ((RetVal == 0U) && (DataSize == Size)) {
            RetVal = AmbaFS_FileClose(pFile);
            if (RetVal == 0U) {
                RetVal = SVC_OK;
            } else {
                RetVal = 3U;
            }
        } else {
            (void) AmbaFS_FileClose(pFile);
            RetVal = 2U;
        }
    } else {
        RetVal = 1U;
    }

    return RetVal;
}

static UINT32 FexTask_GetFileSize_SD(const char *pFileName, UINT32 *pSize)
{
    UINT32 RetVal;
    UINT32 FileSize = 0U;
    UINT64 FilePos = 0U;
    AMBA_FS_FILE *pFile;

    if ((NULL != pFileName) && (NULL != pSize)) {
        RetVal = AmbaFS_FileOpen(pFileName, "rb", &pFile);
        if (RetVal == 0U) {
            RetVal = AmbaFS_FileSeek(pFile, 0LL, AMBA_FS_SEEK_END);
            if(RetVal == 0U) {
                RetVal = AmbaFS_FileTell(pFile, &FilePos);
                if(RetVal == 0U) {
                    FileSize = (UINT32)(FilePos & 0xFFFFFFFFU);
                    RetVal = AmbaFS_FileClose(pFile);
                    if(RetVal == 0U) {
                        *pSize = FileSize;
                        RetVal = SVC_OK;
                    } else {
                        RetVal = 5U;
                    }
                } else {
                    RetVal = 4U;
                }
            } else {
                RetVal = 3U;
            }
        } else {
            RetVal = 2U;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_FEX_TASK, "GetFileSize_SD: error(%d)", RetVal, 0U);
        AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}
#endif

static void FexTask_CvFlowOutputHandler(UINT32 CvFlowChan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal;
    const SVC_CV_FLOW_FEX_OUTPUT_s *pCvOut;
    SVC_FEX_TASK_HDLR_s *pHdlr;
    UINT32 OutBufIdx;

    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pOutput);

    AmbaMisra_TypeCast(&pCvOut, &pOutput);

    if (SVC_OK == FexTask_CvfChan2Hdlr(CvFlowChan, &pHdlr)) {
        if (pHdlr->DispOn == 1U) {
            /* Get new buffer index */
            OutBufIdx = (UINT32)pHdlr->OutBufIdx;
            OutBufIdx++;
            if (OutBufIdx >= SVC_FEX_OUT_BUF_NUM) {
                OutBufIdx = 0U;
            }

            RetVal = AmbaWrap_memcpy(&pHdlr->OutBuf[OutBufIdx][0], pCvOut, sizeof(SVC_CV_FLOW_FEX_OUTPUT_s));

            /* Update buffer index */
            Fex_MutexTake();
            pHdlr->OutBufIdx = (UINT8)OutBufIdx;
            FexOsdUpdate[pHdlr->ID] = (UINT8)(SVC_FEX_OSD_SHOW | SVC_FEX_OSD_UPDATE);
            Fex_MutexGive();
        } else {
            /* *Do nothing */
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX_TASK, "CvFlowOutputHandler: Invalid CvflowChan(%u)", CvFlowChan, 0U);
    }

    AmbaMisra_TouchUnused(&RetVal);
}

/**
 *  Init the Fex task
 *  @return error code
 */
UINT32 SvcFexTask_Init(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&FexHdlr, 0, sizeof(FexHdlr));
    RetVal |= AmbaWrap_memset(&FexFileInCtrl, 0, sizeof(FexFileInCtrl));
    RetVal |= AmbaWrap_memset(&FexDispInfo, 0, sizeof(FexDispInfo));

    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&FexMutex, NULL)) {
        SvcLog_NG(SVC_LOG_FEX_TASK, "MutexCreate error", 0U, 0U);
        RetVal = SVC_NG;
    }

    FexTaskInit = 1U;

    return RetVal;
}

/**
 *  Configure the Fex task
 *  @param[in] pConfig Fex task configs
 *  @return error code
 */
UINT32 SvcFexTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 FileInOn = 0U;

    SvcLog_DBG(SVC_LOG_FEX_TASK, "SvcFexTask_Config", 0U, 0U);

    if (FexTaskInit == 1U) {
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if (pCvFlow[i].CvFlowType == SVC_CV_FLOW_FEX) {
                    FexCvfEnable = 1U;

                    if ((pCvFlow[i].CvAppFlag & SVC_CV_APP_FILE_INPUT_TEST) == 1U) {
                        FileInOn = 1U;
                    }
                }
            }
        }

        if (FileInOn == 1U) {
            /* Request buffer for file input */
            RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED,
                                       FMEM_ID_CV_FILE_IN,
                                       &FexFileInCtrl.BufBase,
                                       &FexFileInCtrl.BufSize);
            //FexTask_ResetBuf(&FexFileInCtrl);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_FEX_TASK, "fail to SvcBuffer_Request", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX_TASK, "Fex task has not initialized", 0U, 0U);
    }

    return RetVal;
}

/**
 *  Start the Fex task
 *  @param[in] pStart Fex task start configs
 *  @return error code
 */
UINT32 SvcFexTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    SVC_FEX_TASK_HDLR_s *pHdlr;

    SvcLog_DBG(SVC_LOG_FEX_TASK, "SvcFexTask_Start", 0U, 0U);

    if (FexCvfEnable == 1U) {
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if (pCvFlow[i].CvFlowType == SVC_CV_FLOW_FEX) {
                    RetVal = FexTask_GreateHdlr(&pHdlr);
                    if (SVC_OK == RetVal) {
                        pHdlr->CvFlowChan = i;
                        if (1U == FexTask_UpdateDispInfo(i, &FexDispInfo[pHdlr->ID])) {
                            pHdlr->DispOn = 1U;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_FEX_TASK, "FexTask_GreateHdlr(%u) error", i, 0U);
                    }

                    RetVal = SvcCvFlow_Register(i, FexTask_CvFlowOutputHandler, &pHdlr->CvFlowCbRegID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_FEX_TASK, "SvcCvFlow_Register(%u) error", i, 0U);
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 *  Stop the Fex task
 *  @param[in] pStop Fex task stop configs
 *  @return error code
 */
UINT32 SvcFexTask_Stop(void)
{
    return SVC_OK;
}

static UINT32 FexTask_GuiLevel2Hdlr(UINT32 GuiLevel, SVC_FEX_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_FEX_TASK_MAX_HDLR; i++) {
            if ((1U == FexHdlr[i].Used) && (GuiLevel == FexDispInfo[i].GuiLevel)) {
                *pHdlr = &FexHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }
    return RetVal;
}

static void FexTask_CalcOSDRegion(UINT32 DispStrmIdx, UINT32 DispStrmChanIdx, REGION_s *pOsdReg)
{
    UINT32 RetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDispStrm = &pResCfg->DispStrm[DispStrmIdx];
    const SVC_CHAN_CFG_s  *pChanCfg = &pDispStrm->StrmCfg.ChanCfg[DispStrmChanIdx];
    UINT32 Value, MixerWidth, MixerHeight, OsdWidth, OsdHeight;

    /* Calculate OSD region with resepect to OSD buffer */
    /* Suppose max OSD window is the same as Mixer window */
    RetVal = SvcOsd_GetOsdBufSize(pDispStrm->VoutID, &OsdWidth, &OsdHeight);
    if (SVC_OK == RetVal) {
        MixerWidth  = pDispStrm->StrmCfg.MaxWin.Width;
        MixerHeight = pDispStrm->StrmCfg.MaxWin.Height;

        if (pDispStrm->VideoRotateFlip == (UINT8) AMBA_DSP_ROTATE_0_HORZ_FLIP) {
            Value = (UINT32) pDispStrm->StrmCfg.Win.Width - ((UINT32) pChanCfg->DstWin.OffsetX + (UINT32) pChanCfg->DstWin.Width);
        } else {
            Value = pChanCfg->DstWin.OffsetX;
        }
        pOsdReg->Index = pDispStrm->VoutID;
        pOsdReg->X     = ((Value + ((MixerWidth - pDispStrm->StrmCfg.Win.Width) >> 1)) * OsdWidth) / MixerWidth;
        pOsdReg->Y     = ((pChanCfg->DstWin.OffsetY + ((MixerHeight - pDispStrm->StrmCfg.Win.Height) >> 1)) * OsdHeight) / MixerHeight;
        pOsdReg->W     = (pChanCfg->DstWin.Width * OsdWidth) / MixerWidth;
        pOsdReg->H     = (pChanCfg->DstWin.Height * OsdHeight) / MixerHeight;
        pOsdReg->RotateFlip = pDispStrm->VideoRotateFlip;
    } else {
        SvcLog_NG(SVC_LOG_FEX_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
    }
}


static UINT32 FexTask_UpdateDispInfo(UINT32 CvflowChan, SVC_FEX_DISP_INFO_s *pDispInfo)
{
    #define MAX_VOUT_CHAN_NUM   2U
    UINT32 Found = 0;
    UINT32 i, j, k;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 StrmId = pResCfg->CvFlow[CvflowChan].InputCfg.Input[0].StrmId;
    UINT32 VoutPri[MAX_VOUT_CHAN_NUM];

    /* Search and decide the CvFlowChan to be displayed */
    VoutPri[0] = VOUT_IDX_B;
    VoutPri[1] = VOUT_IDX_A;
    for (k = 0; (k < MAX_VOUT_CHAN_NUM) && (0U == Found); k++) {
        /* Search corresponding VOUT */
        for (i = 0; (i < pResCfg->DispNum) && (0U  == Found); i++) {
            if (pResCfg->DispStrm[i].VoutID == VoutPri[k]) {
                /* Search corresponding FOV */
                for (j = 0; j < pResCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                    if (StrmId == pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId) {
                        pDispInfo->VoutID = (UINT8)VoutPri[k];
                        pDispInfo->DispStrmIdx = (UINT8)i;
                        pDispInfo->DispStrmChanIdx = (UINT8)j;
                        Found = 1U;
                        AmbaPrint_PrintUInt5("FexTask: Found FOV%d DispStrm[%u].StrmCfg.ChanCfg[%u]", StrmId, i, j, 0U, 0U);
                        break;
                    }
                }
            }
        }
    }

    if (Found == 1U) {
        pDispInfo->SrcWidth = pResCfg->CvFlow[CvflowChan].InputCfg.Input[0].FrameWidth;
        pDispInfo->SrcHeight = pResCfg->CvFlow[CvflowChan].InputCfg.Input[0].FrameHeight;

        FexTask_CalcOSDRegion(pDispInfo->DispStrmIdx, pDispInfo->DispStrmChanIdx, &pDispInfo->OsdReg);

        SVC_FEX_DBG(SVC_LOG_FEX_TASK, "FexTask_UpdateDispInfo: SrcWidth(%d) SrcHeight(%d)", pDispInfo->SrcWidth, pDispInfo->SrcHeight);
        AmbaPrint_PrintUInt5("OsdRegion: VOUT(%u) X %d Y %d W %d H %d",
            pDispInfo->OsdReg.Index, pDispInfo->OsdReg.X, pDispInfo->OsdReg.Y, pDispInfo->OsdReg.W, pDispInfo->OsdReg.H);

        pDispInfo->GuiLevel = SVC_FEX_OSD_LEVEL;
        SvcGui_Register(pDispInfo->VoutID,
                        SVC_FEX_OSD_LEVEL,
                        "Fex",
                        FexTask_OsdDraw,
                        FexTask_OsdUpdate);
    }

    return Found;
}

static void FexTask_OsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    SVC_FEX_TASK_HDLR_s *pHdlr;

    AmbaMisra_TouchUnused(&VoutIdx);

    if (SVC_OK == FexTask_GuiLevel2Hdlr(Level, &pHdlr)) {
        if (((FexOsdUpdate[pHdlr->ID] & SVC_FEX_OSD_SHOW) > 0U) && (pHdlr->DispOn == 1U)) {
            FexTask_OsdDrawFex(pHdlr);
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX_TASK, "OsdDraw: Invalid GuiLevel(%u)", Level, 0U);
    }
}

static void FexTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    SVC_FEX_TASK_HDLR_s *pHdlr;

    AmbaMisra_TouchUnused(&VoutIdx);

    if (SVC_OK == FexTask_GuiLevel2Hdlr(Level, &pHdlr)) {
        if ((FexOsdUpdate[pHdlr->ID] & SVC_FEX_OSD_UPDATE) > 0U) {
            FexOsdUpdate[pHdlr->ID] &= ~(SVC_FEX_OSD_UPDATE);
            *pUpdate = 1U;
        } else {
            *pUpdate = 0U;
        }
    } else {
        *pUpdate = 0U;
        SvcLog_NG(SVC_LOG_FEX_TASK, "OsdUpdate: Invalid GuiLevel(%u)", Level, 0U);
    }
}

static void FexTask_OsdDrawFex(const SVC_FEX_TASK_HDLR_s *pHdlr)
{
    #define BUCKETS_MAX_DISP_POINTS     10U
    UINT32 RetVal = SVC_OK;
    const  UINT8 *pOutBuf;
    const  SVC_CV_FLOW_FEX_OUTPUT_s *pCvOut;
    const  AMBA_CV_FEX_DATA_s *pFex;
    UINT32 ScaleId = 0U, Found = 0U;
    ULONG  ULFex, KeyPointCntAddr, KeyPointAddr;
    const UINT8 *pKeyPointCnt;
    const AMBA_CV_FEX_KEYPOINT_s *pKeyPoints;
    UINT32 Idex, j, DispPointCnt;
    const SVC_FEX_DISP_INFO_s *pDispInfo = &FexDispInfo[pHdlr->ID];
    UINT32 OsdWidth, OsdHeight;
    UINT32 Xstart = 0U, XEnd = 0U, Ystart = 0U, YEnd = 0U;

    Fex_MutexTake();
    pOutBuf = &pHdlr->OutBuf[pHdlr->OutBufIdx][0];
    Fex_MutexGive();

    AmbaMisra_TypeCast(&pCvOut, &pOutBuf);
    pFex = pCvOut->pFexOut;
    AmbaMisra_TypeCast(&ULFex, &pFex);

    for (ScaleId = 0; ScaleId < MAX_HALF_OCTAVES; ScaleId++) {
        if (pFex->PrimaryList[ScaleId].Enable == 1U) {
            Found = 1U;
            break;
        }
    }

    if ((Found == 1U) && (ScaleId < MAX_HALF_OCTAVES)) {
        KeyPointCntAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsOffset;
        AmbaMisra_TypeCast(&pKeyPointCnt, &KeyPointCntAddr);
        AmbaMisra_TypeCast(&pKeyPoints, &KeyPointAddr);

        for (Idex = 0U; Idex < CV_FEX_MAX_BUCKETS; Idex++) {
            DispPointCnt = pKeyPointCnt[Idex];
            //SVC_FEX_DBG(SVC_LOG_FEX_TASK, "PrimaryList Bucket[%d]: Cnt(%d)", Idex, DispPointCnt);

            if (DispPointCnt > BUCKETS_MAX_DISP_POINTS) {
                DispPointCnt = BUCKETS_MAX_DISP_POINTS;
            }
            for (j = 0U; j < DispPointCnt; j++) {
                /* Discard the friction parts of 14.2 format */
                Xstart = (UINT32)pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].X >> 2U;
                Ystart = (UINT32)pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].Y >> 2U;

                /* Map to OSD coordinate */
                Xstart = (Xstart * pDispInfo->OsdReg.W) / pDispInfo->SrcWidth;
                Ystart = (Ystart * pDispInfo->OsdReg.H) / pDispInfo->SrcHeight;

                /* Rebase to origin of OSD coordinate */
                Xstart = Xstart + pDispInfo->OsdReg.X;
                Ystart = Ystart + pDispInfo->OsdReg.Y;

                XEnd = Xstart + OSD_LINE_LEN;
                YEnd = Ystart;

                RetVal = SvcOsd_GetOsdBufSize(pDispInfo->OsdReg.Index, &OsdWidth, &OsdHeight);
                if (SVC_OK == RetVal) {
                    if (((Xstart + OSD_LINE_LEN) >= OsdWidth) ||
                        ((XEnd + OSD_LINE_LEN) >= OsdWidth) ||
                        ((Ystart + OSD_LINE_WIDTH) >= OsdHeight) ||
                        ((YEnd + OSD_LINE_WIDTH) >= OsdHeight)) {
                        continue;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_FEX_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
                }

                RetVal |= SvcOsd_DrawLine(pDispInfo->OsdReg.Index,
                                          Xstart,
                                          Ystart,
                                          XEnd,
                                          YEnd,
                                          OSD_LINE_WIDTH,
                                          OSD_COLOR);
                if (SVC_OK != RetVal) {
                    SvcLog_DBG(SVC_LOG_FEX_TASK, "SvcOsd_DrawLine failed %d", RetVal, 0U);
                    SVC_FEX_DBG_INFO("FexTask: KeyPoint[%d]: (%d, %d) OSD: (%d, %d)",
                        j,
                        ((UINT32)pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].X >> 2U),
                        ((UINT32)pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].Y >> 2U),
                        Xstart,
                        Ystart);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX_TASK, "No FEX is enabled.", 0U, 0U);
    }

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_FEX_TASK, "DrawFex failed", 0U, 0U);
    }
}
