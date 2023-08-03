/**
 *  @file SvcStereoACTask.c
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
 *  @details svc cv stereo auto calibration task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaPrint.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaSvcWrap.h"
#include "AmbaSensor.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcWrap.h"
#include "SvcIK.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_StereoAC.h"
#include "SvcCvCamCtrl.h"
#include "SvcCalibMgr.h"
#include "SvcCalibStereo.h"

/* svc-app */
#include "SvcCvAppDef.h"
#include "SvcStereoACTask.h"
#include "SvcBufMap.h"
#include "SvcWarp.h"

#include "cvapi_flexidag_ambamvac_cv2.h"
#include "AmbaSTU_StereoBarCalibInfo.h"


#define SVC_LOG_STEREO_AC_TASK         "StACTask"

#define SVC_STAC_APPLY_WARP_RESULT

#define SVC_STAC_CALIB_DATA_NUM         (2U)    /* left and right*/
#define SVC_STAC_META_DATA_SIZE         (2792)

#define YUV_STREAM_BIT                  (0x80000000U)

typedef struct {
    UINT32  Used     :1;
    UINT32  Reserved :31;
    UINT8   ID;
    UINT32  CvFlowChan;
    UINT32  CvFlowCbRegID;
    ULONG   CalibDataBufBase[SVC_STAC_CALIB_DATA_NUM];
    UINT32  CalibDataBufSize;
} SVC_STEREO_AC_TASK_HDLR_s;

static UINT8                      StACInit = 0;
static UINT8                      StACCvfEnable = 0;
static SVC_STEREO_AC_TASK_HDLR_s  StACHdlr[SVC_STEREO_AC_TASK_MAX_HDLR];
static UINT8                      StACDebugEnable = 0;
static UINT32                     StACHdlrBufSize = 0U;

static UINT32 StACTask_GreateHdlr(SVC_STEREO_AC_TASK_HDLR_s **pHdlr);
static UINT32 StACTask_CvfChan2Hdlr(UINT32 CvFlowChan, SVC_STEREO_AC_TASK_HDLR_s **pHdlr);
static void   StACTask_CvFlowOutputHandler(UINT32 CvFlowChan, UINT32 CvType, void *pOutput);

static UINT32 StACTask_GetMetaDataFromSD(const SVC_STEREO_AC_TASK_HDLR_s *pHdlr, SVC_CV_META_DATA_s *pMetaDataList);
static UINT32 StACTask_GetMetaDataFromCalib(const SVC_STEREO_AC_TASK_HDLR_s *pHdlr, SVC_CV_META_DATA_s *pMetaDataList);
static UINT32 StACTask_GetCalibMetaData(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, void **pMetaData, UINT32 *pSize);
static UINT32 StACTask_GetCalibData(UINT32 StrmId, SVC_CALIB_TBL_INFO_s *pTbl);

static void SVC_STEREO_AC_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (StACDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void SVC_STEREO_AC_DBG_INFO(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (StACDebugEnable > 1U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static UINT32 StACTask_GreateHdlr(SVC_STEREO_AC_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_STEREO_AC_TASK_MAX_HDLR; i++) {
            if (0U == StACHdlr[i].Used) {
                StACHdlr[i].Used = 1U;
                StACHdlr[i].ID = (UINT8)i;
                *pHdlr = &StACHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 StACTask_CvfChan2Hdlr(UINT32 CvFlowChan, SVC_STEREO_AC_TASK_HDLR_s **pHdlr)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    AmbaMisra_TouchUnused(pHdlr);

    if (NULL != pHdlr) {
        for (i = 0; i < SVC_STEREO_AC_TASK_MAX_HDLR; i++) {
            if ((1U == StACHdlr[i].Used) && (CvFlowChan == StACHdlr[i].CvFlowChan)) {
                *pHdlr = &StACHdlr[i];
                RetVal = SVC_OK;
                break;
            }
        }
    }
    return RetVal;
}

static UINT32 StACTask_ReadFile(const char *pFileName, void *pBuf, UINT32 Size)
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

static void StACTask_SaveBin(const char *pFileName, void *pBuf, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *pFile;
    UINT32 OpRes;

    Ret = AmbaFS_FileOpen(pFileName, "wb", &pFile);
    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "AmbaFS_FileOpen failed", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileWrite(pBuf, 1U, Size, pFile, &OpRes);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "AmbaFS_FileWrite failed", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(pFile);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "AmbaFS_FileClose failed", 0U, 0U);
        }
    }
}

static UINT32 StACTask_GetMetaDataFromSD(const SVC_STEREO_AC_TASK_HDLR_s *pHdlr, SVC_CV_META_DATA_s *pMetaDataList)
{
    #define FILE_PATH_LEFT "c:/201120_mvac_input/metadata/fov0_only_meta.bin"
    #define FILE_PATH_RIGHT "c:/201120_mvac_input/metadata/fov1_only_meta.bin"
    UINT32 RetVal = SVC_OK;
    void *pBufBase;
    UINT32 DataSize;

    DataSize = SVC_STAC_META_DATA_SIZE;
    AmbaMisra_TypeCast(&pBufBase, &pHdlr->CalibDataBufBase[0]);
    if (0U == StACTask_ReadFile(FILE_PATH_LEFT, pBufBase, DataSize)) {
        pMetaDataList[0].Type = 0U;
        pMetaDataList[0].pAddr = pBufBase;
        pMetaDataList[0].Size = DataSize;
        pMetaDataList[0].BufSize = pHdlr->CalibDataBufSize;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "Read meta data for left", 0U, 0U);
        RetVal = SVC_NG;
    }


    AmbaMisra_TypeCast(&pBufBase, &pHdlr->CalibDataBufBase[1]);
    if (0U == StACTask_ReadFile(FILE_PATH_RIGHT, pBufBase, DataSize)) {
        pMetaDataList[1].Type = 0U;
        pMetaDataList[1].pAddr = pBufBase;
        pMetaDataList[1].Size = DataSize;
        pMetaDataList[1].BufSize = pHdlr->CalibDataBufSize;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "Read meta data for right", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 StACTask_GetCalibMetaData(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, void **pMetaData, UINT32 *pSize)
{
    UINT32 RetVal = SVC_OK;
    const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfoMisraC = pStereoCamWarpInfo;
    ULONG DataStartAddr, MetaDataStartAddr;
    const UINT32 *pMetaDataSize;

    if ((pStereoCamWarpInfo == NULL) || (pMetaData == NULL)) {
        RetVal = SVC_NG;
    } else {
        if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
            (pStereoCamWarpInfo->Method.Version[1] == 1U) &&
            (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
            (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            (void) AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfoMisraC, sizeof(void *));
            MetaDataStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2100.HBase.ExifOffset;
        } else if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[1] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
                (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            (void) AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfoMisraC, sizeof(void *));
            MetaDataStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2200.HBase.ExifOffset;
        } else if ((pStereoCamWarpInfo->Method.Version[0] == 2U) &&
                (pStereoCamWarpInfo->Method.Version[1] == 3U) &&
                (pStereoCamWarpInfo->Method.Version[2] == 0U) &&
                (pStereoCamWarpInfo->Method.Version[3] == 0U)) {
            (void) AmbaWrap_memcpy(&DataStartAddr, &pStereoCamWarpInfoMisraC, sizeof(void *));
            MetaDataStartAddr = DataStartAddr + pStereoCamWarpInfo->Method.V2300.HBase.ExifOffset;
        } else {
            RetVal = SVC_NG;
        }

        if (RetVal == SVC_OK) {
            AmbaMisra_TypeCast(&pMetaDataSize, &MetaDataStartAddr);

            /* workaround: add 4 byte padding */
            MetaDataStartAddr = MetaDataStartAddr - 4U;
            *pSize = (*pMetaDataSize) + 4U /*padding*/ + 4U /*size*/;

            (void) AmbaWrap_memcpy(pMetaData, &MetaDataStartAddr, sizeof(void *));

        }
    }

    return RetVal;
}

static UINT32 StACTask_GetCalibData(UINT32 StrmId, SVC_CALIB_TBL_INFO_s *pTbl)
{
    UINT32 RetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 VinID = 0U, SensorIdx = 0U, SensorID = 0U, TblID = 0U;

    if ((StrmId & YUV_STREAM_BIT) == 0U) {
        /* FOV stream */
        if ((pResCfg->FovCfg[StrmId].CalUpdBits & (0x1UL << SVC_CALIB_STEREO_ID)) > 0U) {
            if (SVC_OK == SvcResCfg_GetSensorIdxOfFovIdx(StrmId, &VinID, &SensorIdx)) {
                AmbaPrint_PrintUInt5("SvcResCfg_GetSensorIdxOfFovIdx 0x%x 0x%x", VinID, SensorIdx, 0U, 0U, 0U);

                RetVal = SvcResCfg_GetSensorIDOfSensorIdx(VinID, SensorIdx, &SensorID);
                AmbaPrint_PrintUInt5("SvcResCfg_GetSensorIDOfSensorIdx RetVal(0x%x) 0x%x", RetVal, SensorID, 0U, 0U, 0U);

                RetVal |= SvcCalib_StereoGetTblIDInVinIDSensorID(VinID, SensorID, &TblID);
                AmbaPrint_PrintUInt5("SvcCalib_StereoGetTblIDInVinIDSensorID RetVal(0x%x) 0x%x", RetVal, TblID, 0U, 0U, 0U);
            } else {
                RetVal = SVC_NG;
            }

            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "GetCalibMetaData: invalid StrmId(0x%x)", StrmId, 0U);
            } else {
                SVC_STEREO_AC_DBG_INFO("VinID: %d, SensorID: 0x%x, TblID: %d", VinID, SensorID, TblID, 0U, 0U);

                RetVal = SvcCalib_ItemTableGet(SVC_CALIB_STEREO_ID, TblID, pTbl);
                if (RetVal != SVC_OK) {
                    SvcLog_DBG(SVC_LOG_STEREO_AC_TASK, "SvcCalib_ItemTableGet failed", 0U, 0U);
                }
            }
        } else {
            RetVal = SVC_NG;
        }
    } else {
        /* YUV stream */
        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "GetCalibMetaData: Unsupported stream type", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 StACTask_GetMetaDataFromCalib(const SVC_STEREO_AC_TASK_HDLR_s *pHdlr, SVC_CV_META_DATA_s *pMetaDataList)
{
    UINT32 RetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 CvFlowChan = pHdlr->CvFlowChan;
    UINT32 i, NumValid = 0U;
    SVC_CALIB_TBL_INFO_s CalibTbl;
    const SVC_CALIB_STEREO_INFO_s *pStCalInfo;

    /* Get calib data from calib partition */
    for (i = 0; (i < pCvFlow[CvFlowChan].InputCfg.InputNum) && (i < SVC_STAC_CALIB_DATA_NUM); i++) {
        CalibTbl.BufSize = pHdlr->CalibDataBufSize;
        AmbaMisra_TypeCast(&(CalibTbl.pBuf), &(pHdlr->CalibDataBufBase[i]));

        if (SVC_OK == StACTask_GetCalibData(pCvFlow->InputCfg.Input[i].StrmId, &CalibTbl)) {
            AmbaMisra_TypeCast(&(pStCalInfo), &(pHdlr->CalibDataBufBase[i]));

            AmbaPrint_PrintStr5("StACTask: Got calib data SerialNumber[%s]", pStCalInfo->SerialNumber, NULL, NULL, NULL, NULL);
            AmbaPrint_PrintUInt5("Version: %x %x %x %x",
                pStCalInfo->CamWarpInfo.Method.Version[0],
                pStCalInfo->CamWarpInfo.Method.Version[1],
                pStCalInfo->CamWarpInfo.Method.Version[2],
                pStCalInfo->CamWarpInfo.Method.Version[3],
                0U);

            if (SVC_OK == StACTask_GetCalibMetaData(&pStCalInfo->CamWarpInfo, &pMetaDataList[i].pAddr, &pMetaDataList[i].Size)) {
                pMetaDataList[i].Type = 0U;
                pMetaDataList[i].BufSize = pMetaDataList[i].Size;
                NumValid++;
            }
        } else {
            SvcLog_DBG(SVC_LOG_STEREO_AC_TASK, "Stereo calib data[%d] doesn't exist", i, 0U);
            break;
        }
    }

    if (NumValid == SVC_STAC_CALIB_DATA_NUM) {
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;;
    }

    return RetVal;
}
#define AMBA_CV_MVAC_WARP_TABLE_MAXSIZE (128 * 96 * 2)

static void StACTask_CvFlowOutputHandler(UINT32 CvFlowChan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal;
    const SVC_CV_DATA_OUTPUT_s *pCvOut;
    const AMBA_CV_MVAC_DATA_s *pACResult;
    SVC_STEREO_AC_TASK_HDLR_s *pHdlr;
    const UINT32 *pMsgCode;

    //SvcLog_DBG(SVC_LOG_STEREO_AC_TASK, "StACTask_CvFlowOutputHandler", 0U, 0U);

    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pOutput);

    AmbaMisra_TypeCast(&pCvOut, &pOutput);
    AmbaMisra_TypeCast(&pACResult, &pCvOut->Data[0U].pBuf);
    AmbaMisra_TypeCast(&pMsgCode, &pOutput);


    if (*pMsgCode == SVC_CV_STEREO_AUTO_CALIB) {
        if (SVC_OK == StACTask_CvfChan2Hdlr(CvFlowChan, &pHdlr)) {
#if defined SVC_STAC_APPLY_WARP_RESULT
            const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
            const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
            static AMBA_IK_GRID_POINT_s ACOutWarp[AMBA_CV_MVAC_WARP_TABLE_MAXSIZE];

            SVC_STEREO_AC_DBG(SVC_LOG_STEREO_AC_TASK, "Apply warp to StrmId(0x%x)", pCvFlow->InputCfg.Input[0].StrmId, 0U);

            /* Apply warp to left channel. Suppose Input[0] is left channel. */
            if ((pCvFlow->InputCfg.Input[0].StrmId & YUV_STREAM_BIT) == 0U) {
                RetVal = AmbaWrap_memcpy(ACOutWarp , pACResult->pWarpInfo->CalibrationUpdates[0].WarpTable, sizeof(pACResult->pWarpInfo->CalibrationUpdates[0].WarpTable));

                if (RetVal == SVC_OK) {
                    if (SVC_OK != Svc_ApplyWarpTable(pCvFlow->InputCfg.Input[0].StrmId, /* FovID */
                                                     1U,      /* Enable */
                                                     0U, 0U,  /* Width, Height */
                                                     0U, 0U,  /* HGdNum, VGdNum */
                                                     0U, 0U,  /* TileWExp, TileHExp */
                                                     ACOutWarp)) {
                        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "OutputHandler: ApplyWarpTable failed", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "AmbaWrap_memcpy failed", 0U, 0U);
                }
                StACTask_SaveBin("c:\\stereo_ac_out\\warp_left_app.bin", ACOutWarp,
                    sizeof(pACResult->pWarpInfo->CalibrationUpdates[0].WarpTable));
            } else {
                SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "OutputHandler: Unsupported stream type", 0U, 0U);
            }
#endif
        } else {
            SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "OutputHandler: Invalid CvflowChan(%u)", CvFlowChan, 0U);
        }
    } else {
        /*do nothing*/
    }
    AmbaMisra_TouchUnused(&RetVal);
}

/**
 *  Init the Stereo Auto Calibration task
 *  @return error code
 */
UINT32 SvcStereoACTask_Init(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(StACHdlr, 0, sizeof(StACHdlr));
    StACInit = 1U;

    return RetVal;
}

/**
 *  Query memory size
 *  @param[out] pMemSize Memory size
 *  @return error code
 */
UINT32 SvcStereoACTask_QueryBufSize(UINT32 *pMemSize)
{
    /* meta data buffer for left and right */
    StACHdlrBufSize = (sizeof(SVC_CALIB_STEREO_INFO_s) * SVC_STAC_CALIB_DATA_NUM);
    *pMemSize = StACHdlrBufSize * SVC_STEREO_AC_TASK_MAX_HDLR;

    SvcLog_DBG(SVC_LOG_STEREO_AC_TASK, "SvcStereoACTask_QueryBufSize = %d", *pMemSize, 0U);

    return SVC_OK;
}

/**
 *  Configure the Stereo Auto Calibration task
 *  @param[in] pConfig task configs
 *  @return error code
 */
UINT32 SvcStereoACTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    SVC_STEREO_AC_TASK_HDLR_s *pHdlr = NULL;
    SVC_CV_META_DATA_INFO_s MetaDataInfo;
    SVC_CV_META_DATA_s MetaDataList[SVC_STAC_CALIB_DATA_NUM];
    ULONG StACBufBase;
    UINT32 StACBufSize;

    SvcLog_DBG(SVC_LOG_STEREO_AC_TASK, "SvcStereoACTask_Config", 0U, 0U);

    if (StACInit == 1U) {
        /* Request buffer for meta data */
        RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED,
                                   SMEM_PF0_ID_STEREO_AC,
                                   &StACBufBase,
                                   &StACBufSize);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "fail to SvcBuffer_Request", 0U, 0U);
        }

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_STEREO_AC) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL)) {

                    RetVal = StACTask_GreateHdlr(&pHdlr);
                    if ((SVC_OK == RetVal) && (pHdlr != NULL)) {
                        pHdlr->CvFlowChan = i;
                        pHdlr->CalibDataBufSize = (StACHdlrBufSize >> 1U);
                        pHdlr->CalibDataBufBase[0] = StACBufBase + (StACHdlrBufSize * pHdlr->ID);
                        pHdlr->CalibDataBufBase[1] = StACBufBase + pHdlr->CalibDataBufSize;

                        SvcLog_DBG(SVC_LOG_STEREO_AC_TASK, "CvAppFlag = 0x%x", pCvFlow[i].CvAppFlag, 0U);

                        if ((pCvFlow[i].CvAppFlag & SVC_CV_APP_LOAD_TEST_DATA) > 0U) {
                            /* Get meta data from SD card */
                            RetVal = StACTask_GetMetaDataFromSD(pHdlr, MetaDataList);
                        } else {
                            /* Get meta data from Calib partition */
                            RetVal = StACTask_GetMetaDataFromCalib(pHdlr, MetaDataList);
                        }

                        /* Set calib meta data */
                        if (SVC_OK == RetVal) {
                            MetaDataInfo.NumData = SVC_STAC_CALIB_DATA_NUM;
                            MetaDataInfo.pList = MetaDataList;
                            RetVal = SvcCvFlow_Control(i, SVC_CV_CTRL_SET_META_DATA, &MetaDataInfo);
                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "SET_META_DATA(%u) failed", i, 0U);
                            }
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "StACTask_GreateHdlr(%u) error", i, 0U);
                    }

                    StACCvfEnable = 1U;
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "StAC task has not initialized", 0U, 0U);
    }

    return RetVal;
}

/**
 *  Start the Stereo Auto Calibration task
 *  @param[in] pStart task start configs
 *  @return error code
 */
UINT32 SvcStereoACTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    SVC_STEREO_AC_TASK_HDLR_s *pHdlr;

    SvcLog_DBG(SVC_LOG_STEREO_AC_TASK, "SvcStereoACTask_Start", 0U, 0U);

    if (StACCvfEnable == 1U) {
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_STEREO_AC) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL)) {
                    RetVal = StACTask_CvfChan2Hdlr(i, &pHdlr);
                    if (SVC_OK == RetVal) {
                        RetVal = SvcCvFlow_Register(i, StACTask_CvFlowOutputHandler, &pHdlr->CvFlowCbRegID);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "SvcCvFlow_Register(%u) error", i, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "SvcStereoACTask_Start: Invalid CvflowChan(%u)", i, 0U);
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 *  Stop the Stereo Auto Calibration task
 *  @param[in] pStop task stop configs
 *  @return error code
 */
UINT32 SvcStereoACTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;

    for (i = 0; i < SVC_STEREO_AC_TASK_MAX_HDLR; i++) {
        if (1U == StACHdlr[i].Used) {
            RetVal = SvcCvFlow_Unregister(StACHdlr[i].CvFlowChan, StACHdlr[i].CvFlowCbRegID);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_STEREO_AC_TASK, "SvcCvFlow_UnRegister(%u) error", i, 0U);
            }
            break;
        }
    }

    RetVal = AmbaWrap_memset(StACHdlr, 0, sizeof(StACHdlr));
    StACCvfEnable = 0U;

    return SVC_OK;
}

