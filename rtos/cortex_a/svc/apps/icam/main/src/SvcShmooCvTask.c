/**
 *  @file SvcShmooCvTask.c
 *
 * Copyright (c) [2021] Ambarella International LP
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
 *  @details svc cv file input
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaSvcWrap.h"
#include "AmbaPrint.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcWrap.h"
#include "SvcTaskList.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcTask.h"

/* svc-app */
#include "SvcCvAppDef.h"
#include "SvcShmooCvTask.h"
#include "SvcBufMap.h"

#include "AmbaNVM_Partition.h"
#include "SvcNvm.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "SvcUserPref.h"
#include "SvcDataCmp.h"

#define SVC_LOG_SHMOO_CV_TASK     "ShmooCvTask"

#define DATA_FMT_Y                              (0U)
#define DATA_FMT_UV420                          (1U)
#define DATA_FMT_BIN                            (2U)

#define CV_FLOW_DONE                            (0x1U)
#define CV_BOOT_DONE                            (0x10U)

#define SHMOO_WRITE_FILE                        (0U)

#define SVC_SHMOO_CV_TASK_PRI                (70U)
#define SVC_SHMOO_CV_TASK_CPU_BITS           (0x01U)
#define SVC_SHMOO_CV_BITSCMP_TASK_PRI        (65U)
#define SVC_SHMOO_CV_BITSCMP_TASK_CPU_BITS   (0x01U)


typedef struct {
    ULONG                        YAddr;
    ULONG                        UVAddr;
} SVC_SHMOO_CV_YUV_BUF_s;

typedef struct {
    ULONG                      BinAddr;
    UINT32                     DataSize;
} SVC_SHMOO_CV_BIN_INFO_s;

typedef struct {
    SVC_SHMOO_CV_YUV_BUF_s     YuvBuf;
} SVC_SHMOO_CV_DATA_BUF_s;

typedef struct {
    UINT32                       InputIdx;
    UINT8                        NumData;
    SVC_SHMOO_CV_DATA_INFO_s     DataInfo[SVC_SHMOO_CV_MAX_DATA_PER_INPUT];
    UINT8                        FileValid[SVC_SHMOO_CV_MAX_DATA_PER_INPUT];
    char                         FileName[SVC_SHMOO_CV_MAX_DATA_PER_INPUT][SVC_SHMOO_CV_MAX_FILE_PATH];
} SVC_SHMOO_CV_CFG_s;

typedef struct {
    SVC_SHMOO_CV_CONFIG_s      Config;
    SVC_SHMOO_CV_CFG_s   InputCfg[SVC_SHMOO_CV_MAX_INPUT_PER_CHAN];

    /* Buffer pool */
    ULONG                        BufBase;
    UINT32                       BufSize;
    ULONG                        BufCurAddr;

    /* Buffer info for specific data */
    SVC_SHMOO_CV_DATA_BUF_s    DataBuf[SVC_SHMOO_CV_MAX_INPUT_PER_CHAN][SVC_SHMOO_CV_MAX_DATA_PER_INPUT];
    SVC_SHMOO_CV_BIN_INFO_s    GoldenCvData[SVC_CV_MAX_DATA_BUF];
    SVC_DATA_CMP_HDLR_s        BitsCmpHdlr[SVC_CV_FLOW_CHAN_MAX];
    UINT32                     LoopRun;
    UINT32                     CvNum;
} SVC_SHMOO_CV_CTRL_s;




static SVC_SHMOO_CV_CTRL_s gShmooCtrl GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t gSvcShmooCvEventFlag GNU_SECTION_NOZEROINIT;
static UINT32 SvcShmooCvTask_RawOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput);

static void SvcShmooCvTask_InitBuf(void)
{
    UINT32 RetVal;

    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CV_FILE_IN, &gShmooCtrl.BufBase, &gShmooCtrl.BufSize);
    if (SVC_OK == RetVal) {
        gShmooCtrl.BufCurAddr = gShmooCtrl.BufBase;
        SvcLog_DBG(SVC_LOG_SHMOO_CV_TASK, "Init Buf: 0x%x, size %u", gShmooCtrl.BufBase, gShmooCtrl.BufSize);
    } else {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "fail to SvcBuffer_Request", 0U, 0U);
    }

}

static inline ULONG SvcShmooCvTask_GetAlignedValULONG   (ULONG Val, ULONG AlignBase) {return (((Val) + (AlignBase - 1U)) & ~(AlignBase - 1U));}
static UINT32 SvcShmooCvTask_AllocBuf(UINT32 ReqSize, ULONG *pBufAddr)
{
    #define BUF_ALIGN_VALUE  128
    UINT32 RetVal = SVC_OK;
    ULONG  BufLimit = gShmooCtrl.BufBase + gShmooCtrl.BufSize - 1U;
    ULONG  BufAddrAlign;
    ULONG  ReqSizeAlign;


    BufAddrAlign = SvcShmooCvTask_GetAlignedValULONG(gShmooCtrl.BufCurAddr, BUF_ALIGN_VALUE);
    ReqSizeAlign = SvcShmooCvTask_GetAlignedValULONG(ReqSize, BUF_ALIGN_VALUE);

    if ((BufAddrAlign + ReqSizeAlign) <= BufLimit) {
        *pBufAddr = BufAddrAlign; 
        gShmooCtrl.BufCurAddr += ReqSizeAlign;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

#if SHMOO_WRITE_FILE

static UINT32 SvcShmooCvTask_FileWrite(const char *pFileName, void *pBuf, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFile = NULL;
    UINT32 DataSize;

    RetVal = AmbaFS_FileOpen(pFileName, "w", &pFile);
    if ((RetVal == 0U) && (pFile != NULL)) {
        if (0U == AmbaFS_FileWrite(pBuf, 1, Size, pFile, &DataSize)) {
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

#endif

static UINT32 SvcShmooCvTask_GetBufferSize(UINT32 DataFmt, UINT32 Pitch, UINT32 Height)
{
    UINT32 Size = Pitch * Height;

    if (DataFmt == DATA_FMT_Y) {
        ;
    } else if (DataFmt == DATA_FMT_UV420) {
        Size = Size >> 1U;
    } else {
        /* Pitch * Height */
    }

    return Size;
}
static void SvcShmooCvTask_CvFlowOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    const UINT32 *pMsgCode;

    AmbaMisra_TouchUnused(&CvType);

    if (pOutput != NULL) {
        AmbaMisra_TypeCast(&pMsgCode, &pOutput);

        switch (*pMsgCode) {
            case SVC_CV_RAW_OUTPUT:
                RetVal = SvcShmooCvTask_RawOutputHandler(Chan, CvType, pOutput);
                break;
            default:
                /* Do nothing */
                break;
        }

        if (RetVal == SVC_OK) {
            if (SVC_OK != AmbaKAL_EventFlagSet(&gSvcShmooCvEventFlag, CV_FLOW_DONE << Chan )) {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "OutputHandler set flag failed", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "OutputHandler Done with error", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "OutputHandler Done with invalid info", 0U, 0U);
    }
}

static UINT32 SvcShmooCvTask_SendInput(UINT32 CvFlowChan, UINT32 NumInput)
{
    UINT32 RetVal;
    const SVC_SHMOO_CV_CFG_s *pInputCfg;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 i, j;
    SVC_CV_INPUT_IMG_INFO_s ImgSend;
    AMBA_DSP_PYMD_DATA_RDY_s YuvInfo[SVC_SHMOO_CV_MAX_INPUT_PER_CHAN];
    const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo[SVC_SHMOO_CV_MAX_INPUT_PER_CHAN] = {NULL, NULL};
    UINT32 ScaleIdx, HierBit;

    RetVal = AmbaWrap_memset(&ImgSend, 0, sizeof(SVC_CV_INPUT_IMG_INFO_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "memset ImgSend failed", 0U, 0U);
    }
    ImgSend.NumInfo = NumInput;


    if (gShmooCtrl.Config.Mode == SVC_SHMOO_CV_YUV_MODE) {
        for (i = 0; i < NumInput; i++) {
            pYuvInfo[i] = &YuvInfo[i];
            RetVal = AmbaWrap_memset(&YuvInfo[i], 0, sizeof(AMBA_DSP_PYMD_DATA_RDY_s));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "memset YuvInfo[%u] failed", i, 0U);
            }

            pInputCfg = &gShmooCtrl.InputCfg[i];

            YuvInfo[i].ViewZoneId = (UINT16) pCvFlow[CvFlowChan].InputCfg.Input[i].StrmId;
            for (j = 0; j < pInputCfg->NumData; j++) {
                ScaleIdx = pInputCfg->DataInfo[j].Img.ScaleIdx;
                HierBit = ((UINT32)1U << ScaleIdx);

                YuvInfo[i].Config.HierBit                 |= (UINT16) HierBit;
                YuvInfo[i].YuvBuf[ScaleIdx].DataFmt        = 0;
                YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrY      = gShmooCtrl.DataBuf[i][j].YuvBuf.YAddr;
                if (gShmooCtrl.DataBuf[i][j].YuvBuf.UVAddr == 0U) {
                    YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrUV = YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrY;
                } else {
                    YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrUV = gShmooCtrl.DataBuf[i][j].YuvBuf.UVAddr;
                }
                YuvInfo[i].YuvBuf[ScaleIdx].Pitch          = (UINT16) pInputCfg->DataInfo[j].Img.Pitch;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.OffsetX = 0;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.OffsetY = 0;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.Width   = (UINT16) pInputCfg->DataInfo[j].Img.Width;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.Height  = (UINT16) pInputCfg->DataInfo[j].Img.Height;
            }

            /*
            AmbaPrint_PrintUInt5("# Send YuvInfo: CvFlowChan(%d) ViewZoneId(%d)",
                                CvFlowChan,
                                YuvInfo[i].ViewZoneId,
                                0U,
                                0U,
                                0U);
            for (k = 0; k < AMBA_DSP_MAX_HIER_NUM; k++) {
                AmbaPrint_PrintUInt5("[%d] DataFmt %d Y: 0x%x UV: 0x%x",
                                    i,
                                    YuvInfo[i].YuvBuf[k].DataFmt,
                                    YuvInfo[i].YuvBuf[k].BaseAddrY,
                                    YuvInfo[i].YuvBuf[k].BaseAddrUV,
                                    0U);
                AmbaPrint_PrintUInt5("(%d %d %d %d)",
                                    YuvInfo[i].YuvBuf[k].Window.OffsetX,
                                    YuvInfo[i].YuvBuf[k].Window.OffsetY,
                                    YuvInfo[i].YuvBuf[k].Window.Width,
                                    YuvInfo[i].YuvBuf[k].Window.Height,
                                    0U);
            }
            */

            ImgSend.Info[i].Content.DataSrc = SVC_CV_DATA_SRC_PYRAMID;
            ImgSend.Info[i].Content.StrmId = (UINT16) pCvFlow[CvFlowChan].InputCfg.Input[i].StrmId;
            ImgSend.Info[i].ElementSize = sizeof(AMBA_DSP_PYMD_DATA_RDY_s);
            AmbaMisra_TypeCast(&ImgSend.Info[i].pBase, &pYuvInfo[i]);
        }
    }


    RetVal = SvcCvFlow_Control(CvFlowChan, SVC_CV_CTRL_SEND_EXT_YUV, &ImgSend);

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "SEND_EXT_YUV error", 0U, 0U);
    }

    return RetVal;
}
/**
 *  Configure the CV file input task
 *  @param[in] pConfig CV File-In configs
 *  @return error code
 */
static UINT32 SvcShmooCvTask_Config(const SVC_SHMOO_CV_CONFIG_s *pConfig)
{
    UINT32 RetVal;
    static char ShmooCvFlagName[32] = "SvcShmooCvFlag";
    static UINT8 Init = 0U;

    if (NULL != pConfig) {
        SvcLog_DBG(SVC_LOG_SHMOO_CV_TASK, "Config CvFlowChan(%u), Mode(%u)", pConfig->CvFlowChan, pConfig->Mode);

        RetVal = AmbaWrap_memset(&gShmooCtrl, 0, sizeof(SVC_SHMOO_CV_CTRL_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "memset gShmooCtrl failed", 0U, 0U);
        }

        RetVal = AmbaWrap_memcpy(&gShmooCtrl.Config, pConfig, sizeof(SVC_SHMOO_CV_CONFIG_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "memcpy Config failed", 0U, 0U);
        }

        if (Init == 0U) {
            Init = 1U;
            RetVal = AmbaKAL_EventFlagCreate(&gSvcShmooCvEventFlag, ShmooCvFlagName);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "ShmooCvFlagName isn't created", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "pConfig = NULL", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Set input information for the CV file input task
 *  @param[in] Type CV File-In type
 *  @param[in] pInfo CV File-In information
 *  @return error code
 */
static UINT32 SvcShmooCvTask_SetInfo(UINT32 Type, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    UINT32 InputIdx, DataIdx;
    SVC_SHMOO_CV_CFG_s *pInputCfg;
    const SVC_SHMOO_CV_INPUT_INFO_s *pInputInfo;
    const SVC_SHMOO_CV_FILE_INFO_s *pFileInfo;
    UINT32 StrLength;

    AmbaMisra_TouchUnused(pInfo);

    switch (Type) {
        case SVC_SHMOO_CV_SET_INPUT_INFO:
            AmbaMisra_TypeCast(&pInputInfo, &pInfo);
            InputIdx = pInputInfo->InputIdx;
            DataIdx  = pInputInfo->DataIdx;

            if ((InputIdx < SVC_SHMOO_CV_MAX_INPUT_PER_CHAN) && (DataIdx < SVC_SHMOO_CV_MAX_DATA_PER_INPUT)) {
                pInputCfg = &gShmooCtrl.InputCfg[InputIdx];
                pInputCfg->InputIdx = InputIdx;
                pInputCfg->NumData++;
                RetVal = AmbaWrap_memcpy(&pInputCfg->DataInfo[DataIdx], &pInputInfo->DataInfo, sizeof(SVC_SHMOO_CV_INPUT_INFO_s));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "memcpy DataInfo failed", 0U, 0U);
                }

                AmbaPrint_PrintUInt5("[InputInfo] InputIdx: %u, NumData: %u, DataIdx: %u",
                                      pInputCfg->InputIdx, pInputCfg->NumData, DataIdx, 0U, 0U);
                AmbaPrint_PrintUInt5("ScaleIdx: %u, W: %u, H: %u, P: %u",
                                      pInputCfg->DataInfo[DataIdx].Img.ScaleIdx,
                                      pInputCfg->DataInfo[DataIdx].Img.Width,
                                      pInputCfg->DataInfo[DataIdx].Img.Height,
                                      pInputCfg->DataInfo[DataIdx].Img.Pitch,
                                      0U);
            } else {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Invalid input info", 0U, 0U);
                RetVal = SVC_NG;
            }
            break;
        case SVC_SHMOO_CV_SET_FILE_INFO:
            AmbaMisra_TypeCast(&pFileInfo, &pInfo);
            InputIdx = pFileInfo->InputIdx;
            DataIdx  = pFileInfo->DataIdx;

            if ((InputIdx < SVC_SHMOO_CV_MAX_INPUT_PER_CHAN) && (DataIdx < SVC_SHMOO_CV_MAX_DATA_PER_INPUT)) {
                StrLength = SvcWrap_strlen(pFileInfo->FileName);
                /* Remain 11 char for renaming */
                if ((SVC_SHMOO_CV_MAX_FILE_PATH - StrLength) < 11U) {
                    SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "File name too long", 0U, 0U);
                    RetVal = SVC_NG;
                }

                if (RetVal == SVC_OK) {
                    pInputCfg = &gShmooCtrl.InputCfg[InputIdx];
                    pInputCfg->FileValid[DataIdx] = 1U;
                    SvcWrap_strcpy(&pInputCfg->FileName[DataIdx][0], SVC_SHMOO_CV_MAX_FILE_PATH, pFileInfo->FileName);

                    AmbaPrint_PrintUInt5("[FileInfo] InputIdx: %u, NumData: %u, DataIdx: %u",
                                          pInputCfg->InputIdx, pInputCfg->NumData, DataIdx, 0U, 0U);
                    AmbaPrint_PrintStr5("%s", pFileInfo->FileName, NULL, NULL, NULL, NULL);
                } else {
                    SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Set file info error", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Invalid file info", 0U, 0U);
                RetVal = SVC_NG;
            }
            break;
        default:
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Invalid info type", 0U, 0U);
            RetVal = SVC_NG;
            break;
    }

    return RetVal;
}

#if SHMOO_WRITE_FILE == 0U
static UINT32 SvcShmooCvTask_DataCompare(UINT32 Chan, UINT8 CmpId, const SVC_SHMOO_CV_BIN_INFO_s *DataInfo)
{
    UINT32 RetVal = SVC_OK, i;
    SVC_DATA_CMP_SEND_s  CmpInfo;

    CmpInfo.CmpId = CmpId;
    CmpInfo.NumData = 4U;

    for(i=0; i<CmpInfo.NumData; i++) {
        AmbaMisra_TypeCast(&(CmpInfo.Data[i].DataAddr), &(DataInfo[i].BinAddr));
        CmpInfo.Data[i].DataSize = DataInfo[i].DataSize;
        CmpInfo.Data[i].BufBase = DataInfo[i].BinAddr;
        CmpInfo.Data[i].BufSize = DataInfo[i].DataSize;
    }

    RetVal = SvcDataCmp_DataSend(&(gShmooCtrl.BitsCmpHdlr[Chan]), &CmpInfo);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "SvcDataCmp_DataSend failed %u", RetVal, 0U);
    }

    return RetVal;
}
#endif

static UINT32 SvcShmooCvTask_RawOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK, i;
    const SVC_CV_DATA_OUTPUT_s *pRawCvData;
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pOutput);
    AmbaMisra_TypeCast(&pRawCvData, &pOutput);

#if SHMOO_WRITE_FILE

    {
        char OutputFileName[32U] = "C:\\Golden0.bin\0";
        for(i=0; i<pRawCvData->NumData;i++) {
            OutputFileName[9] = i + '0' ;
            RetVal = SvcShmooCvTask_FileWrite(OutputFileName, pRawCvData->Data[i].pBuf, pRawCvData->Data[i].BufSize);
            if (RetVal == SVC_OK) {
                AmbaPrint_PrintStr5("[FinTask] Write %s done", OutputFileName, NULL, NULL, NULL, NULL);
            } else{
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Fail to write files",0U, 0U);
            }
        }
        gShmooCtrl.LoopRun = 0U;

    }

#else
    {
        SVC_SHMOO_CV_BIN_INFO_s DataInfo[SVC_CV_MAX_DATA_BUF];
        const SVC_SHMOO_CV_BIN_INFO_s *pDataInfo;
        /*Send Golden Data*/
        for(i=0; i<pRawCvData->NumData;i++)
        {
            DataInfo[i].BinAddr = gShmooCtrl.GoldenCvData[i].BinAddr;
            DataInfo[i].DataSize = gShmooCtrl.GoldenCvData[i].DataSize;
        }
        pDataInfo = DataInfo;
        RetVal = SvcShmooCvTask_DataCompare(Chan, 0U, pDataInfo);
        /*Send Output Data*/
        for(i=0; i<pRawCvData->NumData;i++)
        {
            AmbaMisra_TypeCast(&(DataInfo[i].BinAddr), &(pRawCvData->Data[i].pBuf));
            DataInfo[i].DataSize = pRawCvData->Data[i].BufSize;
        }
        pDataInfo = DataInfo;
        RetVal = SvcShmooCvTask_DataCompare(Chan,1U, pDataInfo);

    }
#endif
    return RetVal;

}

static void SvcShmooCvTask_SysStatusCallback(UINT32 StatIdx, void *pInfo)
{
    const SVC_APP_STAT_CV_BOOT_s *pCvStatus = NULL;
    AmbaMisra_TouchUnused(pInfo);

    if (StatIdx == SVC_APP_STAT_CV_BOOT) {
        AmbaMisra_TypeCast(&pCvStatus, &pInfo);
        if (SVC_OK != AmbaKAL_EventFlagSet(&gSvcShmooCvEventFlag, CV_BOOT_DONE)) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "SysStatusCallback set flag failed", 0U, 0U);
        }
    }

}

static UINT32 SvcShmooCvTask_LoadRomData(const void *pDestBuff, UINT32 BufSize, const char *pPath)
{
    UINT32 Rval;
    UINT32 FileSize;
    UINT8 *pDataBuf = NULL;

    Rval = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, pPath, &FileSize); //FIXME, fileSize should be a parameter
    if ((Rval == OK) && (FileSize != 0U) && (BufSize >= FileSize)) {
        AmbaMisra_TypeCast(&pDataBuf, &pDestBuff);
        Rval = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA, pPath, 0U, FileSize, pDataBuf, 5000);
    } else {
        AmbaPrint_PrintUInt5("SvcShmooCvTask_LoadRomData(): BufSize=%u FileSize=%u", BufSize, FileSize, 0U, 0U, 0U);
        Rval = SVC_NG;
    }

    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Load %s from ROMFS error", pPath, NULL, NULL, NULL, NULL);
    }else{
        AmbaPrint_PrintUInt5("SvcShmooCvTask_LoadRomData() OK!!: BufSize=%u FileSize=%u", BufSize, FileSize, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 SvcShmooCvTask_LoadFromRom(UINT32 FrameWidth, UINT32 FrameHeight, UINT32 FrameType, ULONG *pDstAddr, const char *pPath)
{

    UINT32 RetVal;
    UINT32 DataSize, Width; 
    ULONG AlignedValue;
    const void *pDstBuf;

    AlignedValue = SvcShmooCvTask_GetAlignedValULONG(FrameWidth, 128U);
    Width = (UINT32)AlignedValue;
    DataSize = SvcShmooCvTask_GetBufferSize(FrameType, Width, FrameHeight);
    RetVal = SvcShmooCvTask_AllocBuf(DataSize, pDstAddr);

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "FrmaeType:[%u] Allocate buffer error", FrameType, 0U);
    }else{
        AmbaMisra_TypeCast(&pDstBuf, pDstAddr);
        RetVal = SvcShmooCvTask_LoadRomData(pDstBuf,DataSize,pPath);
    }

    return RetVal;

}

void SvcShmooCvTask_ShmooCompareReport(UINT32 *pErrorCheck, UINT32 PrintReport)
{
    UINT32 RetVal, i;
    SVC_DATA_CMP_REPORT_s Report;

    for(i=0; i<gShmooCtrl.CvNum; i++){
        RetVal = SvcDataCmp_ReportGet(&gShmooCtrl.BitsCmpHdlr[i], &Report);

        if (RetVal == SVC_OK) {
            if (pErrorCheck != NULL) {
                *pErrorCheck = (Report.ErrCount == 0U)? (*pErrorCheck | 0U) : (*pErrorCheck | 1U);
            }
        }

        if(PrintReport == 1U){
            AmbaPrint_PrintUInt5("CvChan:[%u], TotalCmp:[%u], ErrCount:[%u]", i, (UINT32) Report.CmpCount, (UINT32) Report.ErrCount, 0U, 0U);
        }
    }
}

void SvcShmooCvTask_DataCmpCheck(UINT32 *pHaveError, UINT32 IsStop)
{
    SvcShmooCvTask_ShmooCompareReport(pHaveError, 0U);
    if ((*pHaveError == 1U) || (IsStop == 1U)) {
        gShmooCtrl.LoopRun = 0U;
    }
}

static void * SvcShmooCvTask_TaskEntry(void * Arg)
{
#define FRAME_W 1280U
#define FRAME_H 640U

#define FRAME_Y_NAME  "frame.y"
#define FRAME_UV_NAME "frame.uv"
#define NUM_OF_GOLDEN_DATA 4U


    UINT32 RetVal = SVC_OK, RegisterID, i;
    SVC_SHMOO_CV_CONFIG_s Config = {0U, SVC_SHMOO_CV_YUV_MODE};
    SVC_SHMOO_CV_INPUT_INFO_s InputInfo = {0U, 0U, 0U, {{FRAME_W, FRAME_H, FRAME_W, 0U}}};
    SVC_SHMOO_CV_FILE_INFO_s  FileInfo  = {0U, 0U, "c:\\Frame.y"};
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 EventFlag, FileSize;
    SVC_DATA_CMP_TASK_CREATE_s  Create;
    SVC_APP_STAT_SHMOO_CV_s ShmooStatus;
    SVC_USER_PREF_s *pSvcUserPref;

    RetVal = SvcUserPref_Get(&pSvcUserPref);

    AmbaMisra_TouchUnused(Arg);

    /* Set Info */
    RetVal |= SvcShmooCvTask_Config(&Config);
    RetVal |= SvcShmooCvTask_SetInfo(SVC_SHMOO_CV_SET_INPUT_INFO, &InputInfo);
    RetVal |= SvcShmooCvTask_SetInfo(SVC_SHMOO_CV_SET_FILE_INFO,  &FileInfo);

    SvcLog_OK(SVC_LOG_SHMOO_CV_TASK, "Start to load ROM data", 0U, 0U);

    if (RetVal == SVC_OK) {
        gShmooCtrl.CvNum = pSvcUserPref->ShmooInfo.CvNum;

        for(i=0; i<gShmooCtrl.CvNum; i++){
            RetVal = SvcCvFlow_RegisterEx(i, SVC_CV_FLOW_OUTPUT_PORT1, SvcShmooCvTask_CvFlowOutputHandler, &RegisterID);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Register Cv flow Chan [%u] failed", i, 0U);
            }
        }
        RetVal = SvcSysStat_Register(SVC_APP_STAT_CV_BOOT, SvcShmooCvTask_SysStatusCallback, &RegisterID);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Register CV sys stat failed", 0U, 0U);
        }

        SvcShmooCvTask_InitBuf();

        gShmooCtrl.LoopRun = 1U;
        /* Load Frame Y and Frame UV*/
        RetVal = SvcShmooCvTask_LoadFromRom(FRAME_W, FRAME_H, DATA_FMT_Y, &(gShmooCtrl.DataBuf[0][0].YuvBuf.YAddr), FRAME_Y_NAME);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Load Y Frame from ROM failed", 0U, 0U);
        }

        RetVal = SvcShmooCvTask_LoadFromRom(FRAME_W, FRAME_H, DATA_FMT_UV420, &gShmooCtrl.DataBuf[0][0].YuvBuf.UVAddr, FRAME_UV_NAME);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Load UV Frame from ROM failed", 0U, 0U);
        }

        /*Load Golden Data*/
        for (i = 0U; i < NUM_OF_GOLDEN_DATA; i++)
        {
            char GoldenName[] = "Golden0.bin\0";
            GoldenName[6] = i + '0';

            RetVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, GoldenName, &FileSize);
            if (RetVal == SVC_OK) {
                gShmooCtrl.GoldenCvData[i].DataSize = FileSize;
                RetVal = SvcShmooCvTask_LoadFromRom(FileSize, 1U, DATA_FMT_BIN, &gShmooCtrl.GoldenCvData[i].BinAddr, GoldenName);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Load Golden Data:[%u] Failed", i, 0U);
                }
            } else {
                    SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Get Golden Data:[%u] Size Failed", i, 0U);
            }
        }

        for(i=0; i<gShmooCtrl.CvNum; i++){
            /*Create data comparison data*/
            Create.CmpNum       = (UINT8)2U;
            Create.DataNum      = NUM_OF_GOLDEN_DATA;
            Create.TaskPriority = SVC_SHMOO_CV_BITSCMP_TASK_PRI;
            Create.TaskCpuBits  = SVC_SHMOO_CV_BITSCMP_TASK_CPU_BITS;
            RetVal = SvcDataCmp_Create(&(gShmooCtrl.BitsCmpHdlr[i]), &Create);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "SvcDataCmp_Create failed %u", RetVal, 0U);
            }
        }

        if (RetVal == SVC_OK) {
            RetVal = AmbaKAL_EventFlagGet(  &gSvcShmooCvEventFlag,
                                            CV_BOOT_DONE,
                                            AMBA_KAL_FLAGS_ANY,
                                            AMBA_KAL_FLAGS_CLEAR_AUTO,
                                            &EventFlag,
                                            AMBA_KAL_WAIT_FOREVER);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_SHMOO_CV_TASK,"Wait CV_BOOT_DONE failed", 0U, 0U);
            }
        }

        ShmooStatus.Status = SVC_APP_STAT_SHMOO_CV_ON;
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_SHMOO_CV, &ShmooStatus);
        if (RetVal == SVC_NG) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "Issue STAT_SHMOO failed", 0U, 0U);
        }

        while (gShmooCtrl.LoopRun == 1U) {
            (void)AmbaKAL_TaskSleep(5U);

            for(i=0; i<gShmooCtrl.CvNum; i++){
                RetVal = SvcShmooCvTask_SendInput(i, pCvFlow->InputCfg.InputNum); //gShmooCtrl.Config.CvFlowChan

                if (RetVal == SVC_OK) {
                    RetVal = AmbaKAL_EventFlagGet(&gSvcShmooCvEventFlag,
                                                    CV_FLOW_DONE << i,
                                                    AMBA_KAL_FLAGS_ANY,
                                                    AMBA_KAL_FLAGS_CLEAR_AUTO,
                                                    &EventFlag,
                                                    5000);
                    if (RetVal != SVC_OK) {
                        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK,"Wait CV_FLOW_DONE failed", 0U, 0U);
                    }
                }
            }

        }

        for(i=0; i<gShmooCtrl.CvNum; i++){
            RetVal = SvcDataCmp_Delete(&gShmooCtrl.BitsCmpHdlr[i]);
        }
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "SvcDataCmp_Delete failed %u", RetVal, 0U);
        }

    }
    return NULL;
}

UINT32 SvcShmooCvTask_ShmooTask(void)
{

    UINT32 RetVal;
    static SVC_TASK_CTRL_s ShmooCvTaskCtrl GNU_SECTION_NOZEROINIT;
    #define SHMOOCVSTACKSIZE 0x3000U
    static UINT8 ShmooCvTaskStack[SHMOOCVSTACKSIZE] GNU_SECTION_NOZEROINIT;
    static char ShmooCvTaskName[16] = "ShmooCvTask";

    ShmooCvTaskCtrl.Priority   = SVC_SHMOO_CV_TASK_PRI;
    ShmooCvTaskCtrl.EntryFunc  = SvcShmooCvTask_TaskEntry;
    ShmooCvTaskCtrl.pStackBase = ShmooCvTaskStack;
    ShmooCvTaskCtrl.StackSize  = SHMOOCVSTACKSIZE;
    ShmooCvTaskCtrl.CpuBits    = SVC_SHMOO_CV_TASK_CPU_BITS;
    ShmooCvTaskCtrl.EntryArg   = (UINT32) 0U;

    RetVal = SvcTask_Create(ShmooCvTaskName, &ShmooCvTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SHMOO_CV_TASK, "SvcShmooCvTask create failed", 0U, 0U);
    }
    return RetVal;

}
