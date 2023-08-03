/**
 *  @file SvcStixelTask.c
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
 *  @details svc cv stixel task
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
#include "SvcCvFlow_Stereo.h"
#include "SvcCvFlow_Stixel.h"
#include "SvcCvCamCtrl.h"

#if defined(CONFIG_SOC_CV2)
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_flexidag_ambastereoerr_cv2.h"
#endif
#if defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#include "cvapi_flexidag_ambaspufusion_cv2fs.h"
#include "cvapi_flexidag_ambastereoerr_cv2fs.h"
#include "cvapi_flexidag_ambaoffex_cv2fs.h"
#endif
#include "cvapi_flexidag_amba_spu_fex_wrap.h"
#include "cvapi_flexidag_amba_spu_fusion_wrap.h"
#include "cvapi_svccvalgo_stixel.h"

/* svc-app */
#include "SvcCvAppDef.h"
#include "SvcStixelTask.h"
#include "SvcBufMap.h"
#include "SvcUserPref.h"
#include "SvcStereoTask.h"

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
#include "SvcVoutFrmCtrlTask.h"
#endif

#define SVC_LOG_STIXEL_TASK         "StxTask"

#define SVC_STIXEL_OSD_LEVEL        (0x3)
#define SVC_STIXEL_OUT_BUF_NUM      (2U)
#define SVC_STIXEL_OUT_BUF_SIZE     (sizeof(SVC_CV_FLOW_STIXEL_OUTPUT_s))
#define SVC_STIXEL_OSD_SIZE_FHD     (1U)

#define SVC_STIXEL_OSD_SHOW         (0x01U)
#define SVC_STIXEL_OSD_UPDATE       (0x02U)
#ifdef CONFIG_ICAM_32BITS_OSD_USED
#define OSD_COLOR                  (0x8098E7E7U)
#else
#define OSD_COLOR                  (32U)
#endif

#define CONFIDENCE_THRESHOLD        (30000U)

#if defined(CONFIG_ICAM_CV_LINUX_STEREO_APP)
#define STIXEL_CROP_Y_OFFSET        (0U)
#else
#define STIXEL_CROP_Y_OFFSET        (96U)
#endif

#define STIXEL_CROP_HEIGHT          (768U)


typedef struct {
    UINT32  Used     :1;
    UINT32  DispOn   :1;
    UINT32  Reserved :30;
    UINT8   ID;
    UINT32  CvFlowChan;
    UINT32  CvFlowCbRegID;

    UINT8   OutBufIdx;
    UINT8   OutBuf[SVC_STIXEL_OUT_BUF_NUM][SVC_STIXEL_OUT_BUF_SIZE];
} SVC_STIXEL_TASK_HDLR_s;

static UINT8                   StixelInit = 0;
static UINT8                   StixelCvfEnable = 0;
static SVC_STIXEL_TASK_HDLR_s  StixelHdlr[SVC_STIXEL_TASK_MAX_HDLR];
static UINT8                   StixelDebugEnable = 0;

/* Definition for OSD display */
typedef struct {
    UINT8   VoutID;
    UINT8   DispStrmIdx;
    UINT8   DispStrmChanIdx;

    UINT8   GuiLevel;
    REGION_s OsdReg;
} SVC_STIXEL_DISP_INFO_s;

static SVC_STIXEL_DISP_INFO_s  StixelDispInfo[SVC_STIXEL_TASK_MAX_HDLR];
static UINT8                   StixelOsdUpdate[SVC_STIXEL_TASK_MAX_HDLR] = {0};

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
static UINT32 g_FrameSync;
#endif

/* Definition for File input */
typedef struct {
    ULONG   BufBase;
    UINT32  BufSize;
    ULONG   BufCurAddr;
} SVC_STIXEL_TASK_FILE_IN_CTRL_s;
static SVC_STIXEL_TASK_FILE_IN_CTRL_s StixelFileInCtrl;

static UINT32 StixelTask_GreateHdlr(SVC_STIXEL_TASK_HDLR_s **pHdlr);
static UINT32 StixelTask_CvfChan2Hdlr(UINT32 CvFlowChan, SVC_STIXEL_TASK_HDLR_s **pHdlr);
static void StixelTask_CvFlowOutputHandler(UINT32 CvFlowChan, UINT32 CvType, void *pOutput);

static UINT32 StixelTask_GuiLevel2Hdlr(UINT32 GuiLevel, SVC_STIXEL_TASK_HDLR_s **pHdlr);
static UINT32 StixelTask_UpdateDispInfo(UINT32 CvflowChan, SVC_STIXEL_DISP_INFO_s *pDispInfo);
static void StixelTask_OsdDraw(UINT32 VoutIdx, UINT32 Level);
static void StixelTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void StixelTask_OsdDrawStixel(const SVC_STIXEL_TASK_HDLR_s *pHdlr, UINT32 *pFrameNum);
static UINT32 StixelTask_SetRoadEstimate(UINT32 CvflowChan);

static UINT32 counter;
static UINT32 g_osd_enable = SVC_STIXEL_OSD_DISABLE;


static AMBA_KAL_MUTEX_t StixelMutex;
static AMBA_KAL_MUTEX_t OSDMutex;

static void Stixel_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&StixelMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "Stixel_MutexTake: timeout", 0U, 0U);
    }
}

static void Stixel_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&StixelMutex)) {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "Stixel_MutexGive: error", 0U, 0U);
    }
}

#if 1
static void StixelOSD_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&OSDMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "Stixel_MutexTake: timeout", 0U, 0U);
    }
}

static void StixelOSD_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&OSDMutex)) {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "Stixel_MutexGive: error", 0U, 0U);
    }
}
#endif

void SvcStixelTask_SetOsdEnable(UINT32 Enable) {

    Stixel_MutexTake();

    g_osd_enable = Enable;

    Stixel_MutexGive();
}


static void SVC_STIXEL_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (StixelDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void SVC_STIXEL_DBG_INFO(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (StixelDebugEnable > 1U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static UINT32 StixelTask_GreateHdlr(SVC_STIXEL_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_STIXEL_TASK_MAX_HDLR; i++) {
            if (0U == StixelHdlr[i].Used) {
                StixelHdlr[i].Used = 1U;
                StixelHdlr[i].ID = (UINT8)i;
                *pHdlr = &StixelHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }
    return RetVal;
}

static UINT32 StixelTask_CvfChan2Hdlr(UINT32 CvFlowChan, SVC_STIXEL_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_STIXEL_TASK_MAX_HDLR; i++) {
            if ((1U == StixelHdlr[i].Used) && (CvFlowChan == StixelHdlr[i].CvFlowChan)) {
                *pHdlr = &StixelHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }
    return RetVal;
}

static UINT32 StixelTask_AllocBuf(SVC_STIXEL_TASK_FILE_IN_CTRL_s *pMgr, UINT32 ReqSize, void **pBufAddr, UINT32 *pBufSize)
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

static void StixelTask_ResetBuf(SVC_STIXEL_TASK_FILE_IN_CTRL_s *pMgr)
{
    pMgr->BufCurAddr = pMgr->BufBase;
}

static UINT32 StixelTask_ReadFile(const char *pFileName, void *pBuf, UINT32 Size)
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

static void StixelTask_GetFileSize(const char* path, UINT32* Size)
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

static UINT32 StixelTask_FileWrite(const char *pFileName, void *pBuf, UINT32 Size)
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

static UINT32 StixelTask_GetFileSize_SD(const char *pFileName, UINT32 *pSize)
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
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "GetFileSize_SD: error(%d)", RetVal, 0U);
        AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

static void StixelTask_CvFlowOutputHandler(UINT32 CvFlowChan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal;
    const SVC_CV_FLOW_STIXEL_OUTPUT_s *pSvcStixelOut;
    SVC_STIXEL_TASK_HDLR_s *pHdlr;
    UINT32 OutBufIdx;
    const UINT32 *pMsgCode;

    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pOutput);
    AmbaMisra_TypeCast(&pMsgCode, &pOutput);

    if (*pMsgCode == SVC_CV_STIXEL_OUTPUT) {
        AmbaMisra_TypeCast(&pSvcStixelOut, &pOutput);
        {
            UINT32 i;
            SVC_STIXEL_DBG_INFO("StixelOut: StixelNum %u, W %u", pSvcStixelOut->StixelOut.StixelNum, pSvcStixelOut->StixelOut.Width, 0U, 0U, 0U);
            for (i = 0; i < 10U; i++) {
                SVC_STIXEL_DBG_INFO("Stixel: T %u B %u Avg %u Conf %u",
                    pSvcStixelOut->StixelOut.StixelList[i].Top,
                    pSvcStixelOut->StixelOut.StixelList[i].Bottom,
                    pSvcStixelOut->StixelOut.StixelList[i].AvgDisparity,
                    pSvcStixelOut->StixelOut.StixelList[i].Confidence, 0U);
            }
        }

        if (SVC_OK == StixelTask_CvfChan2Hdlr(CvFlowChan, &pHdlr)) {
           if (pHdlr->DispOn == 1U) {
               /* Get new buffer index */
               OutBufIdx = (UINT32)pHdlr->OutBufIdx;
                OutBufIdx++;
                if (OutBufIdx >= SVC_STIXEL_OUT_BUF_NUM) {
                    OutBufIdx = 0U;
                }

               RetVal = AmbaWrap_memcpy(&pHdlr->OutBuf[OutBufIdx][0], pSvcStixelOut, sizeof(SVC_CV_FLOW_STIXEL_OUTPUT_s));

               /* Update buffer index */
               pHdlr->OutBufIdx = (UINT8)OutBufIdx;
               StixelOsdUpdate[pHdlr->ID] = (UINT8)(SVC_STIXEL_OSD_SHOW | SVC_STIXEL_OSD_UPDATE);

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
               StixelOSD_MutexTake();
                g_FrameSync = pSvcStixelOut->FrameNum;
                StixelOSD_MutexGive();
#endif
            } else {
                /* *Do nothing */
            }
        } else {
            SvcLog_NG(SVC_LOG_STIXEL_TASK, "CvFlowOutputHandler: Invalid CvflowChan(%u)", CvFlowChan, 0U);
        }
    } else {
        //do nothing
    }
    AmbaMisra_TouchUnused(&RetVal);
}

/**
 *  Init the Stixel task
 *  @return error code
 */
UINT32 SvcStixelTask_Init(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&StixelHdlr, 0, sizeof(StixelHdlr));
    RetVal |= AmbaWrap_memset(&StixelFileInCtrl, 0, sizeof(StixelFileInCtrl));
    RetVal |= AmbaWrap_memset(&StixelDispInfo, 0, sizeof(StixelDispInfo));

    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&StixelMutex, NULL)) {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "MutexCreate error", 0U, 0U);
        RetVal |= SVC_NG;
    }

    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&OSDMutex, NULL)) {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "MutexCreate error", 0U, 0U);
        RetVal |= SVC_NG;
    }

    StixelInit = 1U;
    counter = 0U;

    return RetVal;
}

/**
 *  Configure the Stixel task
 *  @param[in] pConfig Stixel task configs
 *  @return error code
 */
UINT32 SvcStixelTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 FileInOn = 0U;

    SvcLog_DBG(SVC_LOG_STIXEL_TASK, "SvcStixelTask_Config", 0U, 0U);

    if (StixelInit == 1U) {
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_STIXEL) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL)) {
                    StixelCvfEnable = 1U;
                    RetVal = StixelTask_SetRoadEstimate(i);

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
                                       &StixelFileInCtrl.BufBase,
                                       &StixelFileInCtrl.BufSize);
            StixelTask_ResetBuf(&StixelFileInCtrl);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_STIXEL_TASK, "fail to SvcBuffer_Request", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "Stixel task has not initialized", 0U, 0U);
    }

    return RetVal;
}

/**
 *  Start the Stixel task
 *  @param[in] pStart Stixel task start configs
 *  @return error code
 */
UINT32 SvcStixelTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    SVC_STIXEL_TASK_HDLR_s *pHdlr;

    SvcLog_DBG(SVC_LOG_STIXEL_TASK, "SvcStixelTask_Start)", 0U, 0U);

    if (StixelCvfEnable == 1U) {
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_STIXEL) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL)) {
                    RetVal = StixelTask_GreateHdlr(&pHdlr);
                    if (SVC_OK == RetVal) {
                        pHdlr->CvFlowChan = i;
                        if (1U == StixelTask_UpdateDispInfo(i, &StixelDispInfo[pHdlr->ID])) {
                            pHdlr->DispOn = 1U;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_STIXEL_TASK, "StixelTask_GreateHdlr(%u) error", i, 0U);
                    }

                    RetVal = SvcCvFlow_Register(i, StixelTask_CvFlowOutputHandler, &pHdlr->CvFlowCbRegID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_STIXEL_TASK, "SvcCvFlow_Register(%u) error", i, 0U);
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 *  Stop the Stixel task
 *  @param[in] pStop Stixel task stop configs
 *  @return error code
 */
UINT32 SvcStixelTask_Stop(void)
{
    return SVC_OK;
}

/**
 *  Send File input to Stixel task
 *  @param[in] pInCfg CV file input configs
 *  @return error code
 */
UINT32 SvcStixelTask_SendDsiInput(const SVC_STIXEL_TASK_DSI_IN_s *pInCfg)
{
    UINT32 RetVal = SVC_OK;
    const SVC_CV_STEREO_OUTPUT_s *pStereoOut;
    const AMBA_CV_SPU_BUF_s *pOutBuf;
    const AMBA_CV_SPU_DATA_s *pOutData;
    ULONG DataAddr;
    UINT32 i = 0U;
    SVC_CV_FLOW_STIXEL_DSI_INPUT_s StxielIn;

    AmbaMisra_TypeCast(&pStereoOut, &pInCfg->pDsiInfo);
    if (pStereoOut->Type == STEREO_OUT_TYPE_FUSION) {
        AmbaMisra_TypeCast(&pOutBuf, &pStereoOut->DataAddr);
        AmbaMisra_TypeCast(&pOutData, &pOutBuf->pBuffer);

        for (i = 0; i < MAX_HALF_OCTAVES; i++) {
            if ((pOutData->Scales[i].Status == 0) && (pOutData->Scales[i].BufSize > 0U)) {
                AmbaMisra_TypeCast(&DataAddr, &pOutData);
                DataAddr += pOutData->Scales[i].DisparityMapOffset;

                /* Send to CvFlow */
                DataAddr += (ULONG)(STIXEL_CROP_Y_OFFSET * pOutData->Scales[i].DisparityPitch);
                AmbaMisra_TypeCast(&StxielIn.pAddr, &DataAddr);
                StxielIn.Width  = pOutData->Scales[i].DisparityWidth;
                StxielIn.Height = STIXEL_CROP_HEIGHT;//pOutData->Scales[i].DisparityHeight;
                StxielIn.Pitch  = pOutData->Scales[i].DisparityPitch;
                StxielIn.FrameNum = pStereoOut->FrameNum;

                SVC_STIXEL_DBG(SVC_LOG_STIXEL_TASK, "Send DSI to CvFlowChan(%u)", pInCfg->CvFlowChan, 0U);
                SVC_STIXEL_DBG_INFO("DSI Addr 0x%x, Width %u, Height %u, Pitch %u",
                                          DataAddr, StxielIn.Width, StxielIn.Height, StxielIn.Pitch, 0U);

                RetVal = SvcCvFlow_Control(pInCfg->CvFlowChan, SVC_CV_CTRL_SEND_DSI_INPUT, &StxielIn);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STIXEL_TASK, "[%u]Send DSI input error", pInCfg->CvFlowChan, 0U);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "[%u]Recv invalid output type(%u)", pInCfg->CvFlowChan, pStereoOut->Type);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Send File input to Stixel task
 *  @param[in] pInCfg CV file input configs
 *  @return error code
 */
UINT32 SvcStixelTask_SendFileInput(const SVC_STIXEL_TASK_FILE_IN_s *pInCfg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize, BufSize;
    void  *pBufAddr = NULL;
    ULONG  BufAddr = 0U;

    SvcLog_DBG(SVC_LOG_STIXEL_TASK, "SvcStixelTask_SendFileInput CvFlowChan(%u)", pInCfg->CvFlowChan, 0U);

    if (NULL != pInCfg) {
        /* Load file to buffer */
        RetVal = StixelTask_GetFileSize_SD(pInCfg->pFileName, &DataSize);
        if (SVC_OK == RetVal) {
            StixelTask_ResetBuf(&StixelFileInCtrl);
            RetVal |= StixelTask_AllocBuf(&StixelFileInCtrl, DataSize, &pBufAddr, &BufSize);
            if (SVC_OK == RetVal) {
                RetVal = StixelTask_ReadFile(pInCfg->pFileName, pBufAddr, DataSize);
                if (SVC_OK == RetVal) {
                    AmbaMisra_TypeCast(&BufAddr, &pBufAddr);
                    RetVal = SvcPlat_CacheClean(BufAddr, BufSize);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_STIXEL_TASK, "DataClean failed", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STIXEL_TASK, "ReadFile failed", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STIXEL_TASK, "AllocBuf failed", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_STIXEL_TASK, "GetFileSize failed", 0U, 0U);
        }

        /* Send to CvFlow */
        if (SVC_OK == RetVal) {
            SVC_CV_FLOW_STIXEL_DSI_INPUT_s StxielIn;

            StxielIn.pAddr  = pBufAddr;
            StxielIn.Width  = pInCfg->Width;
            StxielIn.Height = pInCfg->Height;
            StxielIn.Pitch  = pInCfg->Pitch;

            SVC_STIXEL_DBG(SVC_LOG_STIXEL_TASK, "Send DSI to CvFlowChan(%u)", pInCfg->CvFlowChan, 0U);
            SVC_STIXEL_DBG_INFO("DSI Addr 0x%x, Width %u, Height %u, Pitch %u",
                                BufAddr, StxielIn.Width, StxielIn.Height, StxielIn.Pitch, 0U);

            RetVal = SvcCvFlow_Control(pInCfg->CvFlowChan, SVC_CV_CTRL_SEND_DSI_INPUT, &StxielIn);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_STIXEL_TASK, "[%u]Send DSI input error", pInCfg->CvFlowChan, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "Invalid pInCfg", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 StixelTask_GuiLevel2Hdlr(UINT32 GuiLevel, SVC_STIXEL_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_STIXEL_TASK_MAX_HDLR; i++) {
            if ((1U == StixelHdlr[i].Used) && (GuiLevel == StixelDispInfo[i].GuiLevel)) {
                *pHdlr = &StixelHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }
    return RetVal;
}

static UINT32 StixelTask_UpdateDispInfo(UINT32 CvflowChan, SVC_STIXEL_DISP_INFO_s *pDispInfo)
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
                        AmbaPrint_PrintUInt5("StixelTask: Found FOV%d DispStrm[%u].StrmCfg.ChanCfg[%u]", StrmId, i, j, 0U, 0U);
                        break;
                    }
                }
            }
        }
    }

    pDispInfo->VoutID = (UINT8)VOUT_IDX_B;
    pDispInfo->DispStrmIdx = (UINT8)0U;
    pDispInfo->DispStrmChanIdx = (UINT8)0U;
    Found = 1U;

    //if (Found == 1U) {
        pDispInfo->OsdReg.Index = pDispInfo->VoutID;
#if (SVC_STIXEL_OSD_SIZE_FHD == 1U)
        pDispInfo->OsdReg.X = 0;
        pDispInfo->OsdReg.Y = 60;
        pDispInfo->OsdReg.W = 1920;
        pDispInfo->OsdReg.H = 960;
#else
        pDispInfo->OsdReg.X = 0;
        pDispInfo->OsdReg.Y = 30;
        pDispInfo->OsdReg.W = 960;
        pDispInfo->OsdReg.H = 480;
#endif

        pDispInfo->GuiLevel = SVC_STIXEL_OSD_LEVEL;
        SvcGui_Register(pDispInfo->VoutID,
                        SVC_STIXEL_OSD_LEVEL,
                        "Stixel",
                        StixelTask_OsdDraw,
                        StixelTask_OsdUpdate);
    //}

    return Found;
}

static void StixelTask_OsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    SVC_STIXEL_TASK_HDLR_s *pHdlr;
    UINT32 FrameNum;
#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
    UINT64 FrameSync;
#endif

    AmbaMisra_TouchUnused(&VoutIdx);

    if (SVC_OK == StixelTask_GuiLevel2Hdlr(Level, &pHdlr)) {
        if (((StixelOsdUpdate[pHdlr->ID] & SVC_STIXEL_OSD_SHOW) > 0U) && (g_osd_enable == SVC_STEREO_OSD_ENABLE)) {
            StixelTask_OsdDrawStixel(pHdlr, &FrameNum);
            AmbaMisra_TouchUnused(&FrameNum);

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
            FrameSync = (UINT64)FrameNum;
            if (SVC_OK != SvcVoutFrmCtrlTask_Ctrl(SVC_VFC_TASK_CMD_CAP_SEQ, &FrameSync)) {
                SvcLog_DBG(SVC_LOG_STIXEL_TASK, "StereoTask_OsdDraw sync failed %d", 0U, 0U);
            }
            AmbaMisra_TouchUnused(&FrameSync);
#endif
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "OsdDraw: Invalid GuiLevel(%u)", Level, 0U);
    }
}

static void StixelTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    SVC_STIXEL_TASK_HDLR_s *pHdlr;

    AmbaMisra_TouchUnused(&VoutIdx);

    if (SVC_OK == StixelTask_GuiLevel2Hdlr(Level, &pHdlr)) {
        if ((StixelOsdUpdate[pHdlr->ID] & SVC_STIXEL_OSD_UPDATE) > 0U) {
            StixelOsdUpdate[pHdlr->ID] &= ~(SVC_STIXEL_OSD_UPDATE);
            *pUpdate = 1U;
        } else {
            *pUpdate = 0U;
        }
    } else {
        *pUpdate = 0U;
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "OsdUpdate: Invalid GuiLevel(%u)", Level, 0U);
    }
}

static void StixelTask_OsdDrawStixel(const SVC_STIXEL_TASK_HDLR_s *pHdlr, UINT32 *pFrameNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const UINT8 *pOutBuf;
    const SVC_CV_FLOW_STIXEL_OUTPUT_s *pSvcStixelOut;
    const AMBA_CV_STIXEL_DET_OUT_s *pStixelOut;
    UINT32 VoutID, StixelNum, StixelStripW;
    UINT32 Xstart = 0U, XEnd  = 0U , Ystart  = 0U , YEnd = 0U;

    StixelOSD_MutexTake();
    pOutBuf = &pHdlr->OutBuf[pHdlr->OutBufIdx][0];
    AmbaMisra_TypeCast(&pSvcStixelOut, &pOutBuf);
    pStixelOut = &pSvcStixelOut->StixelOut;

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
    *pFrameNum = g_FrameSync;
#else
    *pFrameNum = 0U;
#endif
    StixelOSD_MutexGive();

    StixelNum = pStixelOut->StixelNum;
    VoutID = StixelDispInfo[pHdlr->ID].VoutID;
    StixelStripW = pStixelOut->Width;

    for (i = 0; i < StixelNum; i++) {
        Xstart = i * StixelStripW;
        XEnd = Xstart + StixelStripW - 1U;
        Ystart = pStixelOut->StixelList[i].Top;
        YEnd = pStixelOut->StixelList[i].Bottom;

#if (SVC_STIXEL_OSD_SIZE_FHD == 1U)
        Xstart = Xstart + StixelDispInfo[pHdlr->ID].OsdReg.X;
        XEnd   = XEnd + StixelDispInfo[pHdlr->ID].OsdReg.X;
        Ystart = Ystart + StixelDispInfo[pHdlr->ID].OsdReg.Y;
        YEnd   = YEnd + StixelDispInfo[pHdlr->ID].OsdReg.Y;
#else
        Xstart = (Xstart >> 1U) + StixelDispInfo[pHdlr->ID].OsdReg.X;
        XEnd   = (XEnd >> 1U) + StixelDispInfo[pHdlr->ID].OsdReg.X;
        Ystart = (Ystart >> 1U) + StixelDispInfo[pHdlr->ID].OsdReg.Y;
        YEnd   = (YEnd >> 1U) + StixelDispInfo[pHdlr->ID].OsdReg.Y;
#endif

        if (pStixelOut->StixelList[i].Confidence >= CONFIDENCE_THRESHOLD) {
            if((counter % 3000U) == 0U) {
                AmbaPrint_PrintUInt5("OsdDrawStixel: (%u %u) (%u %u) Confidence %d", Xstart, Ystart, XEnd, YEnd, pStixelOut->StixelList[i].Confidence);
            }
            //SvcOsd_ColorIdxToColorSetting(VoutID, 43, &Color);

#if (SVC_STIXEL_OSD_SIZE_FHD == 1U)
            Ystart += STIXEL_CROP_Y_OFFSET;
            YEnd += STIXEL_CROP_Y_OFFSET;
#else
            Ystart += (STIXEL_CROP_Y_OFFSET >> 1U);
            YEnd += (STIXEL_CROP_Y_OFFSET >> 1U);
#endif
            RetVal |= SvcOsd_DrawSolidRect(VoutID,
                                           Xstart,
                                           Ystart,
                                           XEnd,
                                           YEnd,
                                           OSD_COLOR);
        }
    }

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_STIXEL_TASK, "DrawStixel failed", 0U, 0U);
    }

    counter++;
}

/**
 *  Save Stixel Config
 *  @param[in] CvFlow Channel
 *  @param[in] EstimateMode
 *  @return error code
 */

UINT32 StixelTask_SaveRoadEstimate(UINT32 CvflowChan, UINT32 EstimateMode)
{
    UINT32 Rval = SVC_OK;
    SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s cfg;
    SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s *pcfg = &cfg;
    void *pBuf;
    char FileName[32];
    SVC_USER_PREF_s *pSvcUserPref;

    Rval = AmbaWrap_memset(&cfg, 0, sizeof(SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s));

    Rval = SvcCvFlow_Control(CvflowChan, SVC_CV_CTRL_GET_ROAD_ESTIMATE, pcfg);

    pcfg->Used = 1U;
    pcfg->RoadEstiMode = (UINT16)EstimateMode; /* STIXEL_ROAD_ESTI_MODE_AUTO : 0 or STIXEL_ROAD_ESTI_MODE_MANUAL : 1 */

    AmbaMisra_TypeCast(&pBuf, &pcfg);


    Rval = SvcUserPref_Get(&pSvcUserPref);
    if (Rval == SVC_OK) {
        if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL0) {
            FileName[0] = 'c';
            FileName[1] = '\0';
        } else if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
            FileName[0] = 'd';
            FileName[1] = '\0';
        } else {
            SvcLog_NG(SVC_LOG_STIXEL_TASK, "Unsupported storage", 0U, 0U);
            Rval = SVC_NG;
        }

        AmbaUtility_StringAppend(FileName, sizeof(FileName), ":\\stixelcfg.bin");

        Rval = StixelTask_FileWrite(FileName, pBuf, sizeof(SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s));
        if (SVC_OK == Rval) {
            SvcLog_OK(SVC_LOG_STIXEL_TASK, "StixelTask_FileWrite() success!!", 0U, 0U);
        } else {
            SvcLog_NG(SVC_LOG_STIXEL_TASK, "StixelTask_FileWrite() failed!!", 0U, 0U);
        }
    }

    return Rval;
}

/**
 *  Set Stixel Config
 *  @param[in] CvFlow Channel
 *  @return error code
 */

static UINT32 StixelTask_SetRoadEstimate(UINT32 CvflowChan)
{
    UINT32 Rval = SVC_OK;
    UINT32 Size;
    SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s cfg;
    SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s *pcfg = &cfg;
    static UINT8 pBinBuf[128];
    char EstiCamHeightStrBuff[50U];
    char EstiCamPitchStrBuff[50U];
    char FileName[32];
    SVC_USER_PREF_s *pSvcUserPref;

    Rval = SvcUserPref_Get(&pSvcUserPref);
    if (Rval == SVC_OK) {
        if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL0) {
            FileName[0] = 'c';
            FileName[1] = '\0';
        } else if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
            FileName[0] = 'd';
            FileName[1] = '\0';
        } else {
            SvcLog_NG(SVC_LOG_STIXEL_TASK, "Unsupported storage", 0U, 0U);
            Rval = SVC_NG;
        }

        AmbaUtility_StringAppend(FileName, sizeof(FileName), ":\\stixelcfg.bin");

        StixelTask_GetFileSize(FileName, &Size);
        if (StixelTask_ReadFile(FileName, pBinBuf, Size) != SVC_OK) {
            SvcLog_NG(SVC_LOG_STIXEL_TASK, "Stixel_ReadFile() failed!!", 0U, 0U);
        } else {
            Rval = AmbaWrap_memcpy(pcfg, pBinBuf, sizeof(SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s));
            if (Rval == SVC_OK) {
                SvcLog_OK(SVC_LOG_STIXEL_TASK, "Read file success", 0U, 0U);
                SvcLog_OK(SVC_LOG_STIXEL_TASK, "Used %d RoadEstiMode %d", pcfg->Used, (UINT32)pcfg->RoadEstiMode);

                Rval |= SvcCvFlow_Control(CvflowChan, SVC_CV_CTRL_SET_ROAD_ESTIMATE, pcfg);

                (void)AmbaUtility_DoubleToStr(&EstiCamHeightStrBuff[0U], sizeof(EstiCamHeightStrBuff), pcfg->EstiCamHeight, 5U);
                (void)AmbaUtility_DoubleToStr(&EstiCamPitchStrBuff[0U], sizeof(EstiCamPitchStrBuff), pcfg->EstiCamPitch, 5U);
                AmbaPrint_PrintStr5("[Stixel_Set_RoadEstimate] EstiCamHeight = %s, EstiCamPitch = %s\n", EstiCamHeightStrBuff, EstiCamPitchStrBuff, NULL, NULL, NULL);
            } else {
                SvcLog_OK(SVC_LOG_STIXEL_TASK, "AmbaWrap_memcpy fail", 0U, 0U);
            }
        }
    }
    return Rval;
}

