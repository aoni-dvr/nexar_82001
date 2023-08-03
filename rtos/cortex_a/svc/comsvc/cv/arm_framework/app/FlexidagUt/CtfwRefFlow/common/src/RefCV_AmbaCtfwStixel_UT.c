/**
 *  @file RefCV_AmbaCtfwStixel_UT.c
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
 *  @details SVC COMSVC CV
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_stixel.h"
#include "AmbaCtfw_StixelFlow.h"
#include "AmbaMisraFix.h"

#if defined(CONFIG_QNX)
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "/sd0"
#elif defined(CONFIG_LINUX)
#define SD_ROOT_PATH "/tmp/SD0"
#else // threadX and the others
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "c:"
#endif

#define UT_NAME "AmbaStixel_UT"

// Stixel configuration
//---------------------------------------------
#define UT_CFG_FOCAL_LENGTH_U              (1308.845298)
#define UT_CFG_FOCAL_LENGTH_V              (1308.845298)
#define UT_CFG_OPTICAL_CENTER_X            (998.2284065)
#define UT_CFG_OPTICAL_CENTER_Y            (479.614282)
#define UT_CFG_BASELINE                    (0.3003)

#define UT_CFG_DEFAULT_CAMERA_HEIGH        (1.545852)
#define UT_CFG_DEFAULT_CAMERA_PITCH        (0.155035)

#define UT_CFG_ROAD_SEARCH_PITCH_STRIDE    (0.25)
#define UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE   (0.02)
#define UT_CFG_ROAD_SEARCH_PITCH_NUM       (64U)
#define UT_CFG_ROAD_SEARCH_HEIGHT_NUM      (10U)

#define UT_CFG_VDISP_X_THR                   (0.2)
#define UT_CFG_VDISP_Y_THR                   (0.15)

#define UT_CFG_DET_OBJECT_HEIGHT           (1.0)
#define UT_CFG_MIN_DISPARITY               (6U)
//---------------------------------------------

#define UT_STIXEL_OUT_PATH SD_ROOT_PATH"/CTFW_UT/out/stixel"

static UINT32 ReadInputFile(const char* path, flexidag_memblk_t* pMemBlk)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;

    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret != 0U) {
        ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileTell fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(pMemBlk->pBuffer, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileRead fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileClose fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaCV_UtilityCmaMemClean(pMemBlk);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaCV_UtilityCmaMemClean fail ", 0U, 0U);
        }
    }
    return Ret;
}

static void GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileOpen fail ", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileSeek fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileTell fail ", 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileClose fail ", 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 LoadFlexiDagBin(const char* path, flexidag_memblk_t* pMemBlk)
{
    UINT32 BinSize;
    UINT32 RetVal = 0U;

    GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        RetVal = AmbaCV_UtilityCmaMemAlloc(BinSize, 1, pMemBlk);

        if (RetVal != 0U) {
            ArmLog_ERR(UT_NAME, "RefCV_MemblkAlloc: OOM", 0U, 0U);
        }

        RetVal = AmbaCV_UtilityFileLoad(path, pMemBlk);
        if (RetVal != 0U) {
            ArmLog_ERR(UT_NAME, "LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", RetVal, 0U);
        }
    } else {
        ArmLog_ERR(UT_NAME, "LoadFlexiDagBin: Open flexibin failed", 0U, 0U);
        RetVal = 1U;
    }
    return RetVal;
}

static void CtfwStixel_UT_ResultCbFxn(const CtfwFrameMetadata_t *pMetadata, const void *pOutData)
{
    const char *pOutFileFormat = UT_STIXEL_OUT_PATH "/STIXEL_OUT_%06d.bin";
    char OutFilePath[128U];
    CTFW_STIXEL_TASK_OUT_t *pStixelTskOut;
    UINT32 RetVal = ARM_OK;

    if ( pMetadata->ErrorState != 0U ) {
        ArmLog_STR(UT_NAME, "## %s, get error", __func__, 0U);
    } else {

        AmbaMisra_TypeCast(&pStixelTskOut, &pOutData);

        {
            UINT32 StixelNumM1;
            const AMBA_CV_STIXEL_s *pStixel;

            ArmLog_DBG(UT_NAME, "## Get Stixel result, FrameIdx(%06d)", pMetadata->FrameIndex, 0U);

            pStixel = &pStixelTskOut->StixelInfo.StixelList[0U];
            ArmLog_DBG(UT_NAME, "1st stixel (%d, %d)", pStixel->Top, pStixel->Bottom);

            StixelNumM1 = pStixelTskOut->StixelInfo.StixelNum - 1U;
            pStixel = &pStixelTskOut->StixelInfo.StixelList[StixelNumM1];
            ArmLog_DBG(UT_NAME, "last stixel (%d, %d)", pStixel->Top, pStixel->Bottom);
        }

        {
            UINT32 WriteSize;
            UINT32 RetStrLen;

            RetStrLen = AmbaUtility_StringPrintUInt32(OutFilePath, (UINT32)sizeof(OutFilePath),
                        pOutFileFormat, 1U, &pMetadata->FrameIndex);
            RetVal |= ArmFIO_Save(&pStixelTskOut->StixelInfo, (UINT32)sizeof(AMBA_CV_STIXEL_DET_OUT_s),
                                  OutFilePath, &WriteSize);

            (void) RetStrLen;
        }
    }

    (void) RetVal;

    return;
}

static void SetupSpuData(const flexidag_memblk_t *pSpuDataBuff, const flexidag_memblk_t *pDisparityBuff,
                         UINT32 DispWidth, UINT32 DispPitch, UINT32 DispHeight)
{
    AMBA_CV_SPU_DATA_s *pSpuData;
    ULONG SpuDataAddr;
    ULONG DispDataAddr;
    ULONG DisparityMapOffset;
    UINT32 RetVal = ARM_OK;

    UINT32 SetupScaleIdx;

    AmbaMisra_TypeCast(&pSpuData, &pSpuDataBuff->pBuffer);
    AmbaMisra_TypeCast(&SpuDataAddr, &pSpuDataBuff->pBuffer);
    AmbaMisra_TypeCast(&DispDataAddr, &pDisparityBuff->pBuffer);
    DisparityMapOffset = DispDataAddr - SpuDataAddr;

    // Setup header
    {
        AmbaMisra_TypeCast(&pSpuData->Reserved_0[0], &pSpuData);
        pSpuData->Reserved_0[0] = MAX_HALF_OCTAVES;
        pSpuData->Reserved_0[1] = DispHeight;
        pSpuData->Reserved_0[2] = DispWidth;
        pSpuData->Reserved_0[3] = 0U;
    }

    // Zero out all scales
    {
        UINT32 ScaleIdx;
        for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
            RetVal |= AmbaWrap_memset(&(pSpuData->Scales[ScaleIdx]), 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s));
            pSpuData->Scales[ScaleIdx].Status = (INT32)0xFFFFFFFEUL; // Not valid
        }
    }

    SetupScaleIdx = 0U;
    // Setup scale
    {
        pSpuData->Scales[SetupScaleIdx].Status = 0;
        pSpuData->Scales[SetupScaleIdx].BufSize = (DispPitch*DispHeight);
        pSpuData->Scales[SetupScaleIdx].DisparityHeight = DispHeight;
        pSpuData->Scales[SetupScaleIdx].DisparityWidth = DispWidth;
        pSpuData->Scales[SetupScaleIdx].DisparityPitch = DispPitch;
        pSpuData->Scales[SetupScaleIdx].DisparityBpp = 16U;
        pSpuData->Scales[SetupScaleIdx].DisparityQm = 8U;
        pSpuData->Scales[SetupScaleIdx].DisparityQf = 4U;
        pSpuData->Scales[SetupScaleIdx].Reserved_0 = 0U;
        pSpuData->Scales[SetupScaleIdx].RoiStartRow = 0U;
        pSpuData->Scales[SetupScaleIdx].RoiStartCol = 0U;
        pSpuData->Scales[SetupScaleIdx].Reserved_1 = 1U;
        pSpuData->Scales[SetupScaleIdx].RoiAbsoluteStartCol = 0U;
        pSpuData->Scales[SetupScaleIdx].RoiAbsoluteStartRow = 0U;
        pSpuData->Scales[SetupScaleIdx].InvalidDisparities = 0U; //No use
        pSpuData->Scales[SetupScaleIdx].DisparityMapOffset = (relative_ptr_t)DisparityMapOffset;
        pSpuData->Scales[SetupScaleIdx].Reserved_2 = 0U;
        pSpuData->Scales[SetupScaleIdx].Reserved_3 = 0U;
    }

    (void) RetVal;

    return;
}

static UINT32 RefCV_SetupStixelTaskConfig(CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
        UINT32 RoadDetMode, flexidag_memblk_t StixelFxBinBuf[])
{
    UINT32 Rval = ARM_OK;
    AMBA_CV_STIXEL_CFG_V2_s *pStixelConfig;
    pStixelConfig = &pStixelTaskConfig->StixelCfg;

    pStixelTaskConfig->InDispScaleId = 0U;

    // Setup stixel configuration
    {
        pStixelConfig->AlgCfg.RoadEstiMode = (UINT16)RoadDetMode;
        pStixelConfig->AlgCfg.IntParam.Fu = UT_CFG_FOCAL_LENGTH_U;
        pStixelConfig->AlgCfg.IntParam.Fv = UT_CFG_FOCAL_LENGTH_V;
        pStixelConfig->AlgCfg.IntParam.U0 = UT_CFG_OPTICAL_CENTER_X;
        pStixelConfig->AlgCfg.IntParam.V0 = UT_CFG_OPTICAL_CENTER_Y;
        pStixelConfig->AlgCfg.IntParam.Baseline = UT_CFG_BASELINE;

        pStixelConfig->AlgCfg.ManualDetCfg.PitchStride = UT_CFG_ROAD_SEARCH_PITCH_STRIDE;
        pStixelConfig->AlgCfg.ManualDetCfg.HeightStride = UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE;
        pStixelConfig->AlgCfg.ManualDetCfg.PitchSetNum = UT_CFG_ROAD_SEARCH_PITCH_NUM;
        pStixelConfig->AlgCfg.ManualDetCfg.HeightSetNum = UT_CFG_ROAD_SEARCH_HEIGHT_NUM;
        if ( pStixelConfig->AlgCfg.RoadEstiMode == STIXEL_ROAD_ESTI_MODE_MANUAL ) {
            pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Height = UT_CFG_DEFAULT_CAMERA_HEIGH;
            pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Pitch = UT_CFG_DEFAULT_CAMERA_PITCH;
        }
        pStixelConfig->AlgCfg.ManualDetCfg.VDispParam.XRatioThr = UT_CFG_VDISP_X_THR;
        pStixelConfig->AlgCfg.ManualDetCfg.VDispParam.YRatioThr = UT_CFG_VDISP_Y_THR;

        pStixelConfig->AlgCfg.MinDisparity = UT_CFG_MIN_DISPARITY;
        pStixelConfig->AlgCfg.DetObjectHeight = UT_CFG_DET_OBJECT_HEIGHT;

        pStixelConfig->RoiCfg.Width = STIXEL_IN_DISPARITY_WIDTH;
        pStixelConfig->RoiCfg.Height = STIXEL_IN_DISPARITY_HEIGHT;
        pStixelConfig->RoiCfg.StartX = 0U;
        pStixelConfig->RoiCfg.StartY = 0U;
    }

    {
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_v_disparity_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_VDISP]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_gen_esti_road_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_preprocess_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_PREPROCESS]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_free_space_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_FREE_SPACE]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_height_seg_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_HEIGHT_SEG]);
        if (Rval != ARM_OK) {
            ArmLog_ERR(UT_NAME, "## LoadFlexiDagBin error", 0U, 0U);
        }
    }

    return Rval;
}

void RefCV_CtfwStixel_UT(const char* pInputPath,
                         UINT32 StartFrameIndex, UINT32 EndFrameIndex, UINT32 RoadDetMode)
{
    UINT32 Rval = ARM_OK;

    UINT32 FrameCnt = 0U;

    UINT32 DispWidth = 1920U;
    UINT32 DispPitch = 3840U;
    UINT32 DispHeight = 960U;
    UINT32 AllocDisparitySize = DispPitch*DispHeight;
    UINT32 AllocSpuDataSize = (UINT32)sizeof(AMBA_CV_SPU_DATA_s);
    UINT32 AlignedSingleFrameSize;
    UINT32 AllocTotalSize;

    static flexidag_memblk_t TotalBuf;
    static UINT32 MemPoolId;
    static CtfwFrame_t InFramePool[CTFW_MAX_BUFFER_DEPTH];
    static flexidag_memblk_t SpuDataBuff[CTFW_MAX_BUFFER_DEPTH];
    static flexidag_memblk_t DisparityBuff[CTFW_MAX_BUFFER_DEPTH];
    static CTFW_STIXEL_TASK_CONFIG_t StixelTaskConfig;
    static flexidag_memblk_t FxBinBuf[STIXEL_FX_IDX_NUM];
    static UINT32 InitStat = 0U;

    AlignedSingleFrameSize = 0U;
    AlignedSingleFrameSize += CTFW_CV_MEM_ALIGN(AllocSpuDataSize);
    AlignedSingleFrameSize += CTFW_CV_MEM_ALIGN(AllocDisparitySize);

    AllocTotalSize = CTFW_MAX_BUFFER_DEPTH*AlignedSingleFrameSize;

    if ( InitStat == 0U ) {

        // Allocate memory
        {
            Rval = AmbaCV_UtilityCmaMemAlloc(AllocTotalSize, 1, &TotalBuf);
            if (Rval != ARM_OK) {
                ArmLog_ERR(UT_NAME, "## Can't allocate %u from CmaMem", AllocTotalSize, 0U);
            }
        }

        // Partitioning allocated memory
        // Setup Spu header
        if (Rval == ARM_OK) {
            UINT32 FrameBuffIdx;
            Rval |= ArmMemPool_Create(&TotalBuf, &MemPoolId);
            for (FrameBuffIdx = 0U; FrameBuffIdx < CTFW_MAX_BUFFER_DEPTH; FrameBuffIdx++) {
                Rval |= Cftw_MemPool_Allocate(MemPoolId, AllocSpuDataSize, &SpuDataBuff[FrameBuffIdx]);
                Rval |= Cftw_MemPool_Allocate(MemPoolId, AllocDisparitySize, &DisparityBuff[FrameBuffIdx]);
                SetupSpuData(&SpuDataBuff[FrameBuffIdx], &DisparityBuff[FrameBuffIdx], DispWidth, DispPitch, DispHeight);
            }
            if (Rval != ARM_OK) {
                ArmLog_ERR(UT_NAME, "## ArmMemPool error", 0U, 0U);
            }
        }

        Rval |= RefCV_SetupStixelTaskConfig(&StixelTaskConfig, RoadDetMode, FxBinBuf);

        if ( Rval == ARM_OK ) {
            CtfwGetProcResult_t GetStixelResultCbFxn = CtfwStixel_UT_ResultCbFxn;
            Rval |= AmbaCtfw_StixelInit(&StixelTaskConfig, GetStixelResultCbFxn, FxBinBuf, CTFW_MAX_BUFFER_DEPTH, InFramePool);
        }

        if ( Rval == ARM_OK ) {
            InitStat = 1U;
        }
    }

    if ( InitStat == 1U ) {
        UINT32 TargetFrames = (EndFrameIndex - StartFrameIndex) + 1U;

        for (FrameCnt = 0U; FrameCnt < TargetFrames; FrameCnt++) {
            UINT32 FrameBuffIdx;
            CtfwFrame_t *pFrame;

            // Load disparity file
            {
                char DispPath[128U];
                UINT32 FrameIdx;
                UINT32 RetStrLen;

                FrameIdx = StartFrameIndex + FrameCnt;
                FrameBuffIdx = FrameCnt % CTFW_MAX_BUFFER_DEPTH;

                RetStrLen = AmbaUtility_StringPrintUInt32(DispPath, (UINT32)sizeof(DispPath), pInputPath, 1U, &FrameIdx);
                Rval |= ReadInputFile(DispPath, &DisparityBuff[FrameBuffIdx]);
                pFrame = &InFramePool[FrameBuffIdx];

                (void) RetStrLen;
            }

            if (Rval == ARM_OK) {
                Rval = AmbaCtfw_StixelProduceFrame(&SpuDataBuff[FrameBuffIdx], FrameCnt, pFrame);
            }

            if (Rval == ARM_OK) {
                Rval = AmbaCtfw_StixelFeedOneFrame(pFrame);
            }
            Rval |= AmbaKAL_TaskSleep(100U);
        }
    }

    AmbaCtfw_StixelProfilerReport();
}

#if defined(CONFIG_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "schdr_api.h"
#include "cvtask_api.h"
#include "ambint.h"
#include "cvapi_ambacv_flexidag.h"
//#include "rtos/AmbaRTOSWrapper.h"

static void RefCV_SignalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(0);
}

int main(int argc, char **argv)
{
    static UINT32 init = 0;
    UINT32 ret = 0U;
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

    if ( argc < 5 ) {
        printf("Usage: %s [Input Folder] [StartFrame Index] [EndFrame Index] [Road detection mode]\n", argv[0U]);
        printf("Ex: %s \n", argv[0U]);
        exit(-1);
    }

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, RefCV_SignalHandlerShutdown);
    signal(SIGHUP, RefCV_SignalHandlerShutdown);
    signal(SIGUSR1, RefCV_SignalHandlerShutdown);
    signal(SIGQUIT, RefCV_SignalHandlerShutdown);
    signal(SIGINT, RefCV_SignalHandlerShutdown);
    signal(SIGKILL, RefCV_SignalHandlerShutdown);

    /* init scheduler */
    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    {
        const char* pInputPath = argv[1U];
        UINT32 StartFrameIdx = atoi(argv[2U]);
        UINT32 EndFrameIdx = atoi(argv[3U]);
        UINT32 RoadDetMode = atoi(argv[4U]);

        RefCV_CtfwStixel_UT(pInputPath, StartFrameIdx, EndFrameIdx, RoadDetMode);
    }

    return 0;
}
#endif

