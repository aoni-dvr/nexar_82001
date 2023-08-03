/**
 *  @file AmbaAcRef_Flow.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Reference flow of auto calibration, use APIs provided by AmbaSTU.
 *
 */

#include "../../inc/ac/AmbaAcRef_Flow.h"
#include "AmbaMisraFix.h"

#include "stdio.h"
#include "stdlib.h"
#include "ArmErrCode.h"
#include "AmbaTypes.h"
#include "SvcCvAlgo.h"
#include "cvapi_flexidag_ambamvac_cv2.h"
#include "cvapi_flexidag.h"
#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaSTU_FexTask.h"
#include "AmbaSTU_FmaTask.h"
#include "AmbaSTU_VoTask.h"
#include "AmbaSTU_MvacTask.h"
#include "AmbaSTU_SourceTask.h"
#include "AmbaSTU_SinkTask.h"

#define TASKNAME "AppAc_Main"

#define DEFAULT_CVMEM_SIZE    (APPAC_SOURCE_MEM_SIZE + APPAC_SCA_FEX_MEM_SIZE + APPAC_SCA_FMA_MEM_SIZE + APPAC_VO_MEM_SIZE + APPAC_MVAC_MEM_SIZE + APPAC_SINK_MEM_SIZE)
#define ARM_LOG_CVFLOW_COMM    "CvflowComm"
static uint32_t gCvmemSize = DEFAULT_CVMEM_SIZE;
static flexidag_memblk_t  gTotalBuf = {0};
static UINT32 gMemPoolId;
AppAcTaskHandle_t gAmbaFileInTaskHandle;
AppAcTaskHandle_t gAmbaFileOutTaskHandle;
AppAcTaskHandle_t gAmbaFexTaskHandle;
AppAcTaskHandle_t gAmbaFmaTaskHandle;
AppAcTaskHandle_t gAmbaVoTaskHandle;
AppAcTaskHandle_t gAmbaMvacTaskHandle;
AppAcTaskHandle_t gAmbaSourceTaskHandle;
AppAcTaskHandle_t gAmbaSinkTaskHandle;

//char gInputFilePath[128] = "/tmp/nfs/ac/Input/";
//char gInputFileName[128] = "";
//char gOutputFilePath[128] = "/tmp/nfs/ac/out/mvac/";
//char gOutputFileName[128] = "mvac";
//UINT32 gTargetFrames = 1U;

//FIXME: don't use global variable to do this
UINT32 gAllFrameProcDone = 0U;

//void ParseArg(int argc, char **argv)
//{
//    ArmStdC_strcpy(gInputFilePath, ArmStdC_strlen(argv[1]), argv[1]);
//    ArmStdC_strcpy(gInputFileName, ArmStdC_strlen(argv[2]), argv[2]);
//    ArmStdC_strcpy(gOutputFilePath, ArmStdC_strlen(argv[3]), argv[3]);
//    ArmStdC_strcpy(gOutputFileName, ArmStdC_strlen(argv[4]), argv[4]);
//    gTargetFrames = strtol(argv[5], NULL, 10);
//
//    ArmLog_STR("AppAC_Main", "## gInputFile %s/%s", gInputFilePath,gInputFileName);
//    ArmLog_STR("AppAC_Main", "## gOutputFile %s/%s", gOutputFilePath,gOutputFileName);
//}

char AmbaFexFlexiBinFileName[128] = "/sdcard/AmbaFexFlexiDag.bin";

void RefCV_AmbaAc_ProfilerReport(void)
{
    (void) AppAc_ProfilerTimerReport(&gAmbaFileInTaskHandle, 0U);
    (void) AppAc_ProfilerTimerReport(&gAmbaFexTaskHandle, 0U);
    (void) AppAc_ProfilerTimerReport(&gAmbaFmaTaskHandle, 0U);
    (void) AppAc_ProfilerTimerReport(&gAmbaVoTaskHandle, 0U);
    (void) AppAc_ProfilerTimerReport(&gAmbaMvacTaskHandle, 0U);
    (void) AppAc_ProfilerTimerReport(&gAmbaMvacTaskHandle, 1U);
    (void) AppAc_ProfilerTimerReport(&gAmbaFileOutTaskHandle, 0U);

}



UINT32 RefCV_AmbaAcInit(AppAcMetadata_t *pAppAcMetadata, AmbaAcConfig_t *pAmbaAcConfig, AcGetResultCb_t AcGetResultCb)
{
    UINT32 Rval = ARM_OK;
    flexidag_memblk_t *pInitMetaLeftBuf = &(pAppAcMetadata->CalibInfo.Left);
    flexidag_memblk_t *pInitMetaRightBuf = &(pAppAcMetadata->CalibInfo.Right);
    AMBA_CV_MVAC_VIEW_INFO_s *pMvacViewInfo = &(pAmbaAcConfig->MvacViewInfo);
    AMBA_CV_VO_CFG_s *pVoCfg = &(pAmbaAcConfig->VoCfg);

    ArmLog_DBG("AppAC_Main", "## Hello RefCV_AmbaAcInit!!", 0U, 0U);

    if (Rval == ARM_OK) {
        Rval = AmbaCV_UtilityCmaMemAlloc(gCvmemSize, 1, &gTotalBuf);
        if (Rval != 0) {
            ArmLog_ERR(ARM_LOG_CVFLOW_COMM, "## Can't allocate %u from CmaMem", gCvmemSize, 0U);
        } else {
            Rval = ArmMemPool_Create(&gTotalBuf, &gMemPoolId);
        }
    }

    //clear task handles
    ArmStdC_memset(gTotalBuf.pBuffer, 0U, gTotalBuf.buffer_size);
    ArmStdC_memset(&gAmbaFileInTaskHandle, 0U, sizeof(AppAcTaskHandle_t));
    ArmStdC_memset(&gAmbaFileOutTaskHandle, 0U, sizeof(AppAcTaskHandle_t));
    ArmStdC_memset(&gAmbaFexTaskHandle, 0U, sizeof(AppAcTaskHandle_t));
    ArmStdC_memset(&gAmbaFmaTaskHandle, 0U, sizeof(AppAcTaskHandle_t));
    ArmStdC_memset(&gAmbaVoTaskHandle, 0U, sizeof(AppAcTaskHandle_t));
    ArmStdC_memset(&gAmbaMvacTaskHandle, 0U, sizeof(AppAcTaskHandle_t));

    //init
    if (Rval == ARM_OK) {
        Rval |= AppAc_SourceTaskInit(&gAmbaSourceTaskHandle, gMemPoolId);
        Rval |= AppAc_ScaAmbaFexInit(&gAmbaFexTaskHandle, gMemPoolId, &AmbaFexFlexiBinFileName[0]);
        Rval |= AppAc_ScaAmbaFmaInit(&gAmbaFmaTaskHandle, gMemPoolId, &AmbaFexFlexiBinFileName[0], pMvacViewInfo->ScaleId);
        Rval |= AppAc_AmbaVoInit(&gAmbaVoTaskHandle, gMemPoolId, pInitMetaRightBuf, pVoCfg);
        Rval |= AppAc_SinkTaskInit(&gAmbaSinkTaskHandle, gMemPoolId, AcGetResultCb);
        Rval |= AppAc_AmbaMvacInit(&gAmbaMvacTaskHandle, gMemPoolId, pInitMetaLeftBuf, pMvacViewInfo);
    }

    //run
    if (Rval == ARM_OK) {
        Rval |= AppAc_SourceTaskStart(&gAmbaSourceTaskHandle);
        Rval |= AppAc_ScaAmbaFexStart(&gAmbaFexTaskHandle);
        Rval |= AppAc_ScaAmbaFmaStart(&gAmbaFmaTaskHandle);
        Rval |= AppAc_AmbaVoStart(&gAmbaVoTaskHandle);
        Rval |= AppAc_AmbaMvacStart(&gAmbaMvacTaskHandle);
        Rval |= AppAc_SinkTaskStart(&gAmbaSinkTaskHandle);
    }

//    if (Rval == ARM_OK) {
//        ArmLog_STR("AppAC_Main", "## test MVAC restart start"); fflush(stdout);
//
//        ArmLog_STR("AppAC_Main", "## MVAC stop"); fflush(stdout);
//        Rval |= AppAc_AmbaMvacStop(&gAmbaMvacTaskHandle);
//        ArmLog_STR("AppAC_Main", "## MVAC deinit"); fflush(stdout);
//        Rval |= AppAc_AmbaMvacDeinit(&gAmbaMvacTaskHandle, gMemPoolId);
//        ArmLog_STR("AppAC_Main", "## MVAC init"); fflush(stdout);
//        Rval |= AppAc_AmbaMvacInit(&gAmbaMvacTaskHandle, gMemPoolId, pInitMetaLeftBuf);
//        ArmLog_STR("AppAC_Main", "## MVAC start"); fflush(stdout);
//        Rval |= AppAc_AmbaMvacStart(&gAmbaMvacTaskHandle);
//
//        ArmLog_STR("AppAC_Main", "## test MVAC restart finish");
////        while (1) {
////            usleep(1000000);
////        }
//    }

    //connect
    if (Rval == ARM_OK) {
        Rval |= AppAc_TaskConnect_TF(&gAmbaSourceTaskHandle, &gAmbaFexTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaFexTaskHandle,    &gAmbaFmaTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaFexTaskHandle,    &gAmbaVoTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaFmaTaskHandle,    &gAmbaVoTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaFexTaskHandle,    &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaFmaTaskHandle,    &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaVoTaskHandle,     &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaMvacTaskHandle,   &gAmbaSinkTaskHandle);
    }

    return Rval;

}


UINT32 RefCV_AmbaAcFeedOneFrame(UINT32 FrameNum, AppAcPicInfo_t *pAppAcPicInfo, AppAcMetadata_t *pMetadata)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(&FrameNum);

    Rval = AppAc_SourceTaskFeedOneFrame(&gAmbaSourceTaskHandle, pAppAcPicInfo, pMetadata);

    return Rval;
}


UINT32 RefCV_AmbaAc_MvacStop(void)
{
    UINT32 Rval = ARM_OK;

    if (Rval == ARM_OK) {
        Rval |= AppAc_TaskDisconnect_TF(&gAmbaFexTaskHandle, &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskDisconnect_TF(&gAmbaFmaTaskHandle, &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskDisconnect_TF(&gAmbaVoTaskHandle, &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskDisconnect_TF(&gAmbaMvacTaskHandle, &gAmbaSinkTaskHandle);

        Rval |= AppAc_AmbaMvacStop(&gAmbaMvacTaskHandle);
        Rval |= AppAc_AmbaMvacDeinit(&gAmbaMvacTaskHandle, gMemPoolId);
    }

    return Rval;
}

UINT32 RefCV_AmbaAc_MvacStart(AppAcMetadata_t *pAppAcMetadata, AMBA_CV_MVAC_VIEW_INFO_s *pMvacViewInfo)
{
    UINT32 Rval = ARM_OK;
    flexidag_memblk_t *pInitMetaLeftBuf = &(pAppAcMetadata->CalibInfo.Left);
    //flexidag_memblk_t *pInitMetaRightBuf = &(pAppAcMetadata->CalibInfo.Right);

    if (Rval == ARM_OK) {
        Rval |= AppAc_AmbaMvacInit(&gAmbaMvacTaskHandle, gMemPoolId, pInitMetaLeftBuf, pMvacViewInfo);
        Rval |= AppAc_AmbaMvacStart(&gAmbaMvacTaskHandle);

        Rval |= AppAc_TaskConnect_TF(&gAmbaFexTaskHandle, &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaFmaTaskHandle, &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaVoTaskHandle, &gAmbaMvacTaskHandle);
        Rval |= AppAc_TaskConnect_TF(&gAmbaMvacTaskHandle, &gAmbaSinkTaskHandle);
    }

    return Rval;
}


UINT32 RefCV_AmbaAc_MvacRestart(AppAcMetadata_t *pAppAcMetadata, AMBA_CV_MVAC_VIEW_INFO_s *pMvacViewInfo)
{
    UINT32 Rval = ARM_OK;
    //flexidag_memblk_t *pInitMetaLeftBuf = &(pAppAcMetadata->CalibInfo.Left);
    //flexidag_memblk_t *pInitMetaRightBuf = &(pAppAcMetadata->CalibInfo.Right);

    if (Rval == ARM_OK) {
        Rval |= RefCV_AmbaAc_MvacStop();
        Rval |= RefCV_AmbaAc_MvacStart(pAppAcMetadata, pMvacViewInfo);
    }

    return Rval;
}


