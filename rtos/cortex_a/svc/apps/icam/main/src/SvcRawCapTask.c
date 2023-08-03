/**
 *  @file SvcRawCapTask.c
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
 *  @details svc Raw Capture task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaShell.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_Capability.h"

#include "AmbaImg_Proc.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaIQParamHandlerSample.h"

/* core */
#include "SvcErrCode.h"
#include "SvcMem.h"
#include "SvcRawCap.h"
#include "SvcBuffer.h"
#include "SvcImg.h"
#include "SvcCmd.h"
#include "SvcWrap.h"
#include "SvcLog.h"

/* icam */
#include "SvcBufMap.h"
#include "SvcTaskList.h"
#include "SvcRawCapTask.h"

#define SVC_LOG_RCP_TSK             "RCP_TSK"

typedef struct {
    UINT32                       CmdID;
    SVC_RAW_CAP_EXTRA_CAP_FUNC_f CmdHdlr;
} SVC_RAW_CAP_TASK_CMD_HDLR_s;

static UINT32 SvcRawCapTask_AaaInfoGet(UINT32 CmdID, void *pData);
static UINT32 SvcRawCapTask_AaaDataCap(UINT32 CmdID, void *pData);
static UINT32 SvcRawCapTask_CmdEntry(UINT32 CmdID, void *pData);
static void   SvcRawCapTask_ShellCmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcRawCapTask_ShellCmdInstall(void);

static SVC_RAW_CAP_TASK_CMD_HDLR_s RawCapTskCmdHdlrs[SVC_RAW_CAP_CMD_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 SvcRawCapTask_AaaInfoGet(UINT32 CmdID, void *pData)
{
    UINT32 RetVal = SVC_OK;
    SVC_RAW_CAP_AAA_DATA_INFO_s *pDataInfo = NULL;

    AmbaMisra_TouchUnused(&CmdID);
    AmbaMisra_TouchUnused(pData);
    AmbaMisra_TypeCast(&pDataInfo, &pData);

    if (pDataInfo == NULL) {
        RetVal = SVC_NG;
        SVC_WRAP_PRINT "Fail to get aaa info - output data info should not null"
            SVC_PRN_ARG_S SVC_LOG_RCP_TSK
            SVC_PRN_ARG_PROC SvcLog_NG
            SVC_PRN_ARG_E
    } else {
        pDataInfo->DataBufSize = (UINT32)sizeof(SVC_RAW_CAP_AAA_DATA_s);
    }

    return RetVal;
}

static UINT32 SvcRawCapTask_AaaDataCap(UINT32 CmdID, void *pData)
{
    UINT32 RetVal = SVC_OK;
    SVC_RAW_CAP_AAA_DATA_INFO_s *pDataInfo = NULL;

    AmbaMisra_TouchUnused(&CmdID);
    AmbaMisra_TouchUnused(pData);
    AmbaMisra_TypeCast(&pDataInfo, &pData);

    if (pDataInfo == NULL) {
        RetVal = SVC_NG;
        SVC_WRAP_PRINT "Fail to cap aaa data - output data info should not null"
            SVC_PRN_ARG_S SVC_LOG_RCP_TSK
            SVC_PRN_ARG_PROC SvcLog_NG
            SVC_PRN_ARG_E
    } else {
        SVC_RAW_CAP_AAA_DATA_s *pAaaData = NULL;

        AmbaMisra_TypeCast(&(pAaaData), &(pDataInfo->pDataBuf));

        if (pAaaData == NULL) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "Fail to cap aaa data - invalid data buffer base"
                SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                SVC_PRN_ARG_PROC SvcLog_NG
                SVC_PRN_ARG_E
        } else if (pDataInfo->DataBufSize < (UINT32)(sizeof(SVC_RAW_CAP_AAA_DATA_s))) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "Fail to cap aaa data - invalid data buffer size! 0x%x < 0x%x"
                SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                SVC_PRN_ARG_PROC SvcLog_NG
                SVC_PRN_ARG_UINT32 pDataInfo->DataBufSize                   SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 (UINT32)(sizeof(SVC_RAW_CAP_AAA_DATA_s)) SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
        } else {
            UINT32              Idx, ExpIdx;
            UINT32              ImgAlgoInfoNum = 0U;
            SVC_IMG_ALGO_INFO_s ImgAlgoInfo[AMBA_DSP_MAX_VIEWZONE_NUM];
            SVC_IMG_ALGO_INFO_s *pCurImgAlgoInfo = NULL;

            AmbaSvcWrap_MisraMemset(ImgAlgoInfo, 0, sizeof(ImgAlgoInfo));
            SvcImg_AlgoInfoGet(pDataInfo->VinID, pDataInfo->CapSeq, ImgAlgoInfo, &ImgAlgoInfoNum);

            for (Idx = 0U; Idx < ImgAlgoInfoNum; Idx ++) {
                if (pDataInfo->ChanID == ImgAlgoInfo[Idx].Ctx.Id) {
                    pCurImgAlgoInfo = &(ImgAlgoInfo[Idx]);
                    break;
                }
            }

            if (pCurImgAlgoInfo == NULL) {
                RetVal = SVC_NG;
                SVC_WRAP_PRINT "Fail to cap aaa data - invalid image algo channel id(%d)! VinID(%u) RawCapSeq(%u)"
                    SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                    SVC_PRN_ARG_PROC SvcLog_NG
                    SVC_PRN_ARG_UINT32 pDataInfo->ChanID SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pDataInfo->VinID  SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 pDataInfo->CapSeq SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            } else {
                if (pCurImgAlgoInfo->ExposureNum > SVC_RAW_CAP_AAA_MAX_EXP_NUM) {
                    RetVal = SVC_NG;
                    SVC_WRAP_PRINT "Fail to cap aaa data - invalid exposure number. %d > %d"
                        SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                        SVC_PRN_ARG_PROC SvcLog_NG
                        SVC_PRN_ARG_UINT32 pCurImgAlgoInfo->ExposureNum SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 SVC_RAW_CAP_AAA_MAX_EXP_NUM  SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                } else if (pCurImgAlgoInfo->Ctx.pAaaInfo == NULL) {
                    RetVal = SVC_NG;
                    SVC_WRAP_PRINT "Fail to cap aaa data - 3A data should not null."
                        SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                        SVC_PRN_ARG_PROC SvcLog_NG
                        SVC_PRN_ARG_E
                } else if (pCurImgAlgoInfo->Ctx.pAeInfo == NULL) {
                    RetVal = SVC_NG;
                    SVC_WRAP_PRINT "Fail to cap aaa data - Ae data should not null."
                        SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                        SVC_PRN_ARG_PROC SvcLog_NG
                        SVC_PRN_ARG_E
                } else {
                    AMBA_AE_INFO_s *pCurAeInfo = NULL;

                    AmbaSvcWrap_MisraMemset(pAaaData, 0, sizeof(SVC_RAW_CAP_AAA_DATA_s));
                    pAaaData->MagicCode = SVC_RAW_CAP_AAA_MAGIC_CODE;
                    pAaaData->ExposureNum = pCurImgAlgoInfo->ExposureNum;

                    AmbaSvcWrap_MisraMemcpy(&(pAaaData->AaaInfo), pCurImgAlgoInfo->Ctx.pAaaInfo, sizeof(AMBA_IMGPROC_OFFLINE_AAA_INFO_s));

                    AmbaMisra_TypeCast(&(pCurAeInfo), &(pCurImgAlgoInfo->Ctx.pAeInfo));

                    if (pCurAeInfo != NULL) {
                        for (ExpIdx = 0U; ExpIdx < pCurImgAlgoInfo->ExposureNum; ExpIdx ++) {
                            AmbaSvcWrap_MisraMemcpy(&(pAaaData->AeInfo[ExpIdx]), &(pCurAeInfo[ExpIdx]), sizeof(AMBA_AE_INFO_s));
                        }
                    }

                    AmbaMisra_TouchUnused(pCurAeInfo);
                }
            }

            AmbaMisra_TouchUnused(pCurImgAlgoInfo);
        }

        AmbaMisra_TouchUnused(pDataInfo);
    }

    return RetVal;
}

static UINT32 SvcRawCapTask_CmdEntry(UINT32 CmdID, void *pData)
{
    UINT32 RetVal = SVC_OK;

    if (CmdID >= SVC_RAW_CAP_CMD_NUM) {
        RetVal = SVC_NG;
        SVC_WRAP_PRINT "Fail to proc capture cmd - invalid command id(%u)"
            SVC_PRN_ARG_S SVC_LOG_RCP_TSK
            SVC_PRN_ARG_PROC SvcLog_NG
            SVC_PRN_ARG_UINT32 CmdID SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    }

    if (RetVal == SVC_OK) {
        UINT32 CmdIdx;

        for (CmdIdx = 0U; CmdIdx < SVC_RAW_CAP_CMD_NUM; CmdIdx ++) {
            if ((RawCapTskCmdHdlrs[CmdIdx].CmdID == CmdID) && (RawCapTskCmdHdlrs[CmdIdx].CmdHdlr != NULL)) {
                RetVal = (RawCapTskCmdHdlrs[CmdIdx].CmdHdlr)(CmdID, pData);
            }
        }
    }

    return RetVal;
}

static void SvcRawCapTask_ShellCmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    SvcRawCap_CmdAppEntry(ArgCount, pArgVector);
    AmbaMisra_TouchUnused(&PrintFunc);
}

static void SvcRawCapTask_ShellCmdInstall(void)
{
    AMBA_SHELL_COMMAND_s SvcRawCapCmdApp;

    SvcRawCapCmdApp.pName    = "svc_rawcap";
    SvcRawCapCmdApp.MainFunc = SvcRawCapTask_ShellCmdEntry;
    SvcRawCapCmdApp.pNext    = NULL;

    if (SHELL_ERR_SUCCESS != SvcCmd_CommandRegister(&SvcRawCapCmdApp)) {
        SVC_WRAP_PRINT "## fail to install svc raw cap command"
            SVC_PRN_ARG_S SVC_LOG_RCP_TSK
            SVC_PRN_ARG_PROC SvcLog_NG
            SVC_PRN_ARG_E
    }
}

/**
 * Initial raw capture task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcRawCapTask_Init(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 BufSize = 0U;
    ULONG  BufBase = 0U;
    UINT8 *pBuf = NULL;

    PRetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_TRANSIENT_BUF, &BufBase, &BufSize);
    if (PRetVal != SVC_OK) {
        RetVal = SVC_NG;
        SVC_WRAP_PRINT "## fail to request raw cap memory"
            SVC_PRN_ARG_S SVC_LOG_RCP_TSK
            SVC_PRN_ARG_PROC SvcLog_NG
            SVC_PRN_ARG_E
    }

    if (RetVal == SVC_OK) {
        AmbaSvcWrap_MisraMemset(&RawCapTskCmdHdlrs, 0, sizeof(RawCapTskCmdHdlrs));
        RawCapTskCmdHdlrs[0U] = (SVC_RAW_CAP_TASK_CMD_HDLR_s) { SVC_RAW_CAP_CMD_AAA_INFO_GET, SvcRawCapTask_AaaInfoGet };
        RawCapTskCmdHdlrs[1U] = (SVC_RAW_CAP_TASK_CMD_HDLR_s) { SVC_RAW_CAP_CMD_AAA_DATA_CAP, SvcRawCapTask_AaaDataCap };
    }

    if (RetVal == SVC_OK) {
        RetVal = SvcRawCap_Create(SVC_RAW_CAP_TASK_PRI, SVC_RAW_CAP_TASK_CPU_BITS);
    }

    if (RetVal == SVC_OK) {
        if (0U != SvcRawCap_CfgExtCapCb(SvcRawCapTask_CmdEntry)) {
            SVC_WRAP_PRINT "## fail to register svc_rawcap callback"
                SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                SVC_PRN_ARG_PROC SvcLog_NG
                SVC_PRN_ARG_E
        }
    }

    if (RetVal == SVC_OK) {
        AmbaMisra_TypeCast(&(pBuf), &(BufBase));
        RetVal = SvcRawCap_CfgMem(pBuf, BufSize);
    }

#if defined(CONFIG_ICAM_IMGITUNER_USED)
    if (RetVal == SVC_OK) {
        UINT32 ItnBufSize = 0U;
        ULONG  ItnBufBase = 0U;

        PRetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ITUNER, &ItnBufBase, &ItnBufSize);
        if (PRetVal != SVC_OK) {
            RetVal = SVC_NG;
            SVC_WRAP_PRINT "## fail to request ituner memory"
                SVC_PRN_ARG_S SVC_LOG_RCP_TSK
                SVC_PRN_ARG_PROC SvcLog_NG
                SVC_PRN_ARG_E
        } else {
            AmbaMisra_TypeCast(&(pBuf), &(ItnBufBase));
            RetVal = SvcRawCap_CfgItnMem(pBuf, ItnBufSize);
        }
    }
#endif

    SvcRawCapTask_ShellCmdInstall();
    AmbaMisra_TouchUnused(&RetVal);

    return SVC_OK;
}
