/*
* Copyright (c) 2020 Ambarella International LP
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
*/


#include "stdio.h"
#include "stdlib.h"
#include "ArmErrCode.h"
#include "AmbaTypes.h"
#include "SvcCvAlgo.h"
#include "cvapi_flexidag_ambamvac_cv2.h"
#include "cvapi_flexidag.h"
#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaAcRef_Flow.h"
#include "AmbaMisraFix.h"

#define TASKNAME "AmbaAc_UT"

//#define FILE_PATH "/tmp/nfs/ac/Input/"
//#define FILE_PATH "/sd0/outdoor1/pushcar/2323/"
//#define FILE_PATH "/sd0/outdoor1/pushcar/2929/"
//#define FILE_PATH "/sd0/scale2_image/8_1/"
#define FILE_PATH "/sd0/201120_mvac_input/"

#define MaxFileNameLen 128U
#define FILENUM 500U
#define DEFAULT_CVMEM_SIZE    (6U << 20U)
#define SCALE_ID (2U)
static uint32_t gCvmemSize = DEFAULT_CVMEM_SIZE;
static flexidag_memblk_t  gTotalBuf = {0};
static UINT32 gMemPoolId;
static UINT32 gTargetFrames = FILENUM;


void ParseArg(int argc, char **argv)
{

}

static UINT32 SetFrameNum(AppAcPicInfo_t *pPicInfo, UINT32 FrameNum)
{
    UINT32 Rval = ARM_OK;
    memio_source_recv_picinfo_t *pInfo;

    ArmStdC_memcpy(&pInfo, &(pPicInfo->PicInfo.pBuffer), sizeof(void*));
    pInfo->pic_info.frame_num = FrameNum;


    return Rval;
}


void ConfigPicInfo(memio_source_recv_picinfo_t *pPicInfo, char *pLumaLeftAddr, char *pLumaRightAddr, UINT32 ScaleId)
{
    UINT32 i;

    roi_t Roi;
    UINT32 RoiStartX = 0U;
    UINT32 RoiStartY = 0U;
    UINT32 RoiWidth = 1920U;
    UINT32 RoiPitch = 1920U;
    UINT32 RoiHeight = 960U;
    UINT32 YuvWidth = 3840U;
    //UINT32 YuvPitch = 3840U;
    UINT32 YuvHeight = 1920U;

    UINT32 LumaLeftOffset =  (UINT32)((UINT64)pLumaLeftAddr - (UINT64)pPicInfo);
    UINT32 LumaRightOffset =  (UINT32)((UINT64)pLumaRightAddr - (UINT64)pPicInfo);

    ArmLog_U64(TASKNAME, "## pLumaLeftAddr=%llx, pPicInfo=%llx", (UINT64)pLumaLeftAddr, (UINT64)pPicInfo);
    ArmLog_U64(TASKNAME, "## pLumaRightAddr=%llx, pPicInfo=%llx", (UINT64)pLumaRightAddr, (UINT64)pPicInfo);

    ArmStdC_memset(pPicInfo, 0U, sizeof(memio_source_recv_picinfo_t));

    Roi.m_start_col = (UINT16) RoiStartX;
    Roi.m_start_row = (UINT16) RoiStartY;
    Roi.m_width_m1 = (UINT16) RoiWidth - 1U;
    Roi.m_height_m1 = (UINT16) RoiHeight - 1U;

    pPicInfo->pic_info.pyramid.image_width_m1 = (UINT16) (YuvWidth - 1U);
    pPicInfo->pic_info.pyramid.image_height_m1 = (UINT16) (YuvHeight - 1U);
    pPicInfo->pic_info.pyramid.image_pitch_m1 = (YuvWidth - 1U);
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.mode = 1U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.octave_mode = 0U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.roi_pitch = (UINT16) RoiPitch;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_start_row = (INT16) Roi.m_start_col;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_start_col = (INT16) Roi.m_start_row;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_height_m1 = Roi.m_height_m1;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_width_m1 = Roi.m_width_m1;


    for (i = 0U; i < MAX_HALF_OCTAVES; i++) {
        if (i == ScaleId) {
            pPicInfo->pic_info.pyramid.half_octave[i].ctrl.disable = 0U;
        } else {
            pPicInfo->pic_info.pyramid.half_octave[i].ctrl.disable = 1U;
        }
    }

    pPicInfo->pic_info.rpLumaLeft[ScaleId] = LumaLeftOffset; //assign to scale 0
    pPicInfo->pic_info.rpLumaRight[ScaleId] = LumaRightOffset;


    ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.image_width_m1  = %d", (UINT32)pPicInfo->pic_info.pyramid.image_width_m1, 0U);
    ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.image_height_m1 = %d", (UINT32)pPicInfo->pic_info.pyramid.image_height_m1, 0U);
    ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.image_pitch_m1  = %d", (UINT32)pPicInfo->pic_info.pyramid.image_pitch_m1, 0U);
    for (i = 0U; i < MAX_HALF_OCTAVES; i++) {

        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].ctrl.disable     = %d", i, pPicInfo->pic_info.pyramid.half_octave[i].ctrl.disable    );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].ctrl.mode        = %d", i, pPicInfo->pic_info.pyramid.half_octave[i].ctrl.mode       );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].ctrl.octave_mode = %d", i, pPicInfo->pic_info.pyramid.half_octave[i].ctrl.octave_mode);
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].ctrl.roi_pitch   = %d", i, pPicInfo->pic_info.pyramid.half_octave[i].ctrl.roi_pitch  );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].roi_start_row    = %d", i, (UINT32)pPicInfo->pic_info.pyramid.half_octave[i].roi_start_row   );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].roi_start_col    = %d", i, (UINT32)pPicInfo->pic_info.pyramid.half_octave[i].roi_start_col   );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].roi_height_m1    = %d", i, (UINT32)pPicInfo->pic_info.pyramid.half_octave[i].roi_height_m1   );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.pyramid.half_octave[%d].roi_width_m1     = %d", i, (UINT32)pPicInfo->pic_info.pyramid.half_octave[i].roi_width_m1    );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.rpLumaLeft         [%d]                  = %d", i, pPicInfo->pic_info.rpLumaLeft [i]                         );
        ArmLog_DBG(TASKNAME, "## pPicInfo->pic_info.rpLumaRight        [%d]                  = %d", i, pPicInfo->pic_info.rpLumaRight[i]                         );

    }

}


void RefCV_GetResult(void *pResult)
{
    UINT32 Rval = ARM_OK;
    AMBA_CV_MVAC_DATA_s *pAcResult;

    ArmLog_U64(TASKNAME, "## pAcResult=0x%llx", (UINT64)pResult, (UINT64)0U); //fflush(stdout);

    ArmStdC_memcpy(&pAcResult, &(pResult), sizeof(void*));


    switch (pAcResult->State) {
    case AMBA_CV_MVAC_STATE_RESET:
        ArmLog_DBG(TASKNAME, "## state: RESET", 0U, 0U);
        break;
    case AMBA_CV_MVAC_STATE_WAIT:
        ArmLog_DBG(TASKNAME, "## state: WAIT", 0U, 0U);
        break;
    case AMBA_CV_MVAC_STATE_ACCUMULATION:
        ArmLog_DBG(TASKNAME, "## state: ACCUMULATION", 0U, 0U);
        break;
    case AMBA_CV_MVAC_STATE_SOLVE:
        ArmLog_DBG(TASKNAME, "## state: SOLVE", 0U, 0U);
        if (pAcResult->pWarpInfo) {
            UINT32 WriteSize;
            if ((pAcResult->pWarpInfo)->CalibrationUpdates[0].WarpTableValid == (UINT16)1U) {
                ArmLog_DBG(TASKNAME, "[ut] got warp table", 0U, 0U);
                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                   sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                   FILE_PATH"/out/warp_left.bin", &WriteSize);
                RefCV_AmbaAc_ProfilerReport();
            }
            if ((pAcResult->pWarpInfo)->CalibrationUpdates[0].MetaCalibValid == (UINT16)1U) {
                ArmLog_DBG(TASKNAME, "[ut] got calib info", 0U, 0U);
                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                   sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                   FILE_PATH"/out/calib_left.bin", &WriteSize);
            }


        }
        break;
    case AMBA_CV_MVAC_STATE_END:
        ArmLog_DBG(TASKNAME, "## state: END", 0U, 0U);
        if (pAcResult->pWarpInfo) {
            UINT32 WriteSize;
            if ((pAcResult->pWarpInfo)->CalibrationUpdates[0].WarpTableValid == (UINT16)1U) {
                ArmLog_DBG(TASKNAME, "[ut] got warp table", 0U, 0U);
                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                   sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                   FILE_PATH"/out/warp_left.bin", &WriteSize);
                RefCV_AmbaAc_ProfilerReport();
            }
            if ((pAcResult->pWarpInfo)->CalibrationUpdates[0].MetaCalibValid == (UINT16)1U) {
                ArmLog_DBG(TASKNAME, "[ut] got calib info", 0U, 0U);
                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                   sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                   FILE_PATH"/out/calib_left.bin", &WriteSize);
            }


        }

        break;

    case AMBA_CV_MVAC_STATE_SKIP:
        ArmLog_DBG(TASKNAME, "## state: SKIP", 0U, 0U);

        if (pAcResult->pWarpInfo) {
            UINT32 WriteSize;
            if ((pAcResult->pWarpInfo)->CalibrationUpdates[0].WarpTableValid == (UINT16)1U) {
                ArmLog_DBG(TASKNAME, "[ut] got warp table", 0U, 0U);
                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                   sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                   FILE_PATH"/out/warp_left.bin", &WriteSize);
                RefCV_AmbaAc_ProfilerReport();
            }
            if ((pAcResult->pWarpInfo)->CalibrationUpdates[0].MetaCalibValid == (UINT16)1U) {
                ArmLog_DBG(TASKNAME, "[ut] got calib info", 0U, 0U);
                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                   sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                   FILE_PATH"/out/calib_left.bin", &WriteSize);
            }


        }


        break;
    default:
        ArmLog_ERR(TASKNAME, "## callback at state: ERROR", 0U, 0U);
    }


//    switch (pAcResult->State) {
//        case MVAC_STATE_RESET:
//            printf("[ut]state: RESET\n");
//            break;
//        case MVAC_STATE_WAIT:
//            printf("[ut]state: WAIT\n");
//            break;
//        case MVAC_STATE_ACCUMULATION:
//            printf("[ut]state: ACCUMULATION\n");
//            if(pAcResult->WarpInfoValid == 1){
//                printf("[ut] generate warp table at MVAC_STATE_ACCUMULATION\n");
//            }
//            break;
//        case MVAC_STATE_SOLVE:
//            printf("[ut]state: SOLVE\n");
//            if(pAcResult->WarpInfoValid == 1){
//                UINT32 WriteSize;
//
//                printf("[ut] generate warp table at MVAC_STATE_SOLVE\n");
//                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                        sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                        "/tmp/nfs/ac/out/warp_left.bin",
//                        &WriteSize);
//            }
//            break;
//        case MVAC_STATE_END:
//            printf("[ut]state: END\n");
//            if(pAcResult->WarpInfoValid == 1){
//                UINT32 WriteSize;
//
//                printf("[ut] generate warp table at MVAC_STATE_END\n");
//                Rval = ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                        sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                        "/tmp/nfs/ac/out/warp_left.bin",
//                        &WriteSize);
//            }
//
//            break;
//        case MVAC_STATE_SKIP:
//            printf("[ut]state: SKIP\n");
//            if(pAcResult->WarpInfoValid == 1){
//                printf("[ut] generate warp table at MVAC_STATE_SKIP\n");
//            }
//            break;
//        default:
//            printf("[ut]state: ERROR\n");
//    }


    AmbaMisra_TouchUnused(&Rval);
    return;
}

UINT32 RefCV_PrepareInitData(
    AppAcMetadata_t *pMetadata,
    AmbaAcConfig_t *pAmbaAcConfig,
    UINT32 ScaleId)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen] = FILE_PATH;
    //char FileIndex[MaxFileNameLen] = {0};
    UINT32 Length, ActualLoadSize;
    //flexidag_memblk_t *pOutBufDesc;

    // read metadata
    if (Rval == ARM_OK) {
        ArmStdC_strcpy(Filename, MaxFileNameLen, FILE_PATH);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov0.bin");
        ArmLog_STR(TASKNAME, "## filename: %s", Filename, NULL);
        Rval = ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pMetadata->CalibInfo.Left.pBuffer, Length, Filename, &ActualLoadSize);
        ArmLog_U64(TASKNAME, "## metadata left, pBuffer=%llx", (UINT64)pMetadata->CalibInfo.Left.pBuffer, (UINT64)0U);
    }

    if (Rval == ARM_OK) {
        ArmStdC_strcpy(Filename, MaxFileNameLen, FILE_PATH);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov1.bin");
        ArmLog_STR(TASKNAME, "## filename: %s", Filename, NULL);
        Rval = ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pMetadata->CalibInfo.Right.pBuffer, Length, Filename, &ActualLoadSize);
        ArmLog_U64(TASKNAME, "## metadata right, pBuffer=%llx", (UINT64)pMetadata->CalibInfo.Right.pBuffer, (UINT64)0U);
    }


    if (Rval == ARM_OK) {
        ArmStdC_strcpy(Filename, MaxFileNameLen, FILE_PATH);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/warp_info.bin");
        ArmLog_STR(TASKNAME, "## filename: %s", Filename, NULL);
        Rval = ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(&(pAmbaAcConfig->MvacViewInfo.WarpTableInfo), Length, Filename, &ActualLoadSize);
        ArmLog_U64(TASKNAME, "## warp info pBuffer=%llx", (UINT64)&(pAmbaAcConfig->MvacViewInfo.WarpTableInfo), (UINT64)0U);
    }
    if (Rval == ARM_OK) {
        pAmbaAcConfig->MvacViewInfo.ScaleId = ScaleId;
    }

    if (Rval == ARM_OK) {
        AMBA_CV_VO_CFG_s *pVoConfig = &(pAmbaAcConfig->VoCfg);
        pVoConfig->View.RoiX = 0;
        pVoConfig->View.RoiY = 0;
        pVoConfig->View.RoiWidth = 1920;
        pVoConfig->View.RoiHeight = 960;
        pVoConfig->View.Scale = 2;
        //pVoConfig->pInMetaRight
        pVoConfig->MaxMinimizationIters = 8;
        pVoConfig->ViewMaxMinimizationFeaturesStereo = 400;
    }

    return Rval;
}


UINT32 RefCV_PrepareOneFrame(
    UINT32 FrameNum,
    AppAcPicInfo_t *pAppAcPicInfo,
    flexidag_memblk_t *pLumaLeft,
    flexidag_memblk_t *pLumaRight,
    AppAcMetadata_t *pMetadata)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen] = FILE_PATH;
    char FileIndex[MaxFileNameLen] = {0};
    UINT32 Length, ActualLoadSize;
    flexidag_memblk_t *pOutBufDesc;
    //char metaleftpath[MaxFileNameLen];// = "/tmp/nfs/ac/Input/metadata/fov0_only_meta.bin";
    //char metarightpath[MaxFileNameLen];// = "/tmp/nfs/ac/Input/metadata/fov1_only_meta.bin";

    // read input image
    //load left image
    //generate filename
    if (Rval == ARM_OK) {
        ArmStdC_strcpy(Filename, MaxFileNameLen, FILE_PATH);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/left/frames_");
        sprintf(FileIndex, "%d", FrameNum);
        ArmStdC_strcat(Filename, MaxFileNameLen, FileIndex);
        ArmStdC_strcat(Filename, MaxFileNameLen, ".y");
        ArmLog_STR(TASKNAME, "## filename: %s", Filename, NULL);

        //load to outbuf
        Rval = ArmFIO_GetSize(Filename, &Length);
    }

    pOutBufDesc = pLumaLeft;
    if ((Rval == ARM_OK) && (Length <= pOutBufDesc->buffer_size)) {
        Rval = ArmFIO_Load(pOutBufDesc->pBuffer, Length, Filename, &ActualLoadSize);
        Rval |= AmbaCV_UtilityCmaMemClean(pOutBufDesc);
    } else {
        ArmLog_ERR(TASKNAME, "## Proc2Uprp, ArmFIO_Load failed, Rval=%d, Length=%d", Rval, Length);
    }

    if(Rval != ARM_OK) {
        ArmLog_ERR(TASKNAME, "## Proc2Uprp, ArmFIO_Load failed", 0U, 0U);
    }

    //load right image
    //generate filename
    if (Rval == ARM_OK) {
        ArmStdC_strcpy(Filename, MaxFileNameLen, FILE_PATH);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/right/frames_");
        sprintf(FileIndex, "%d", FrameNum);
        ArmStdC_strcat(Filename, MaxFileNameLen, FileIndex);
        ArmStdC_strcat(Filename, MaxFileNameLen, ".y");
        ArmLog_STR(TASKNAME, "## filename: %s", Filename, NULL);
        //load to outbuf
        Rval = ArmFIO_GetSize(Filename, &Length);
    }

    pOutBufDesc = pLumaRight;
    if ((Rval == ARM_OK) && (Length <= pOutBufDesc->buffer_size)) {
        Rval = ArmFIO_Load(pOutBufDesc->pBuffer, Length, Filename, &ActualLoadSize);
        Rval |= AmbaCV_UtilityCmaMemClean(pOutBufDesc);
    } else {
        ArmLog_ERR(TASKNAME, "## Proc2Uprp, ArmFIO_Load failed, Rval=%d, Length=%d", Rval, Length);
    }
    if(Rval != ARM_OK) {
        ArmLog_ERR(TASKNAME, "## Proc2Uprp, ArmFIO_Load failed", 0U, 0U);
    }

    // read metadata
    if (Rval == ARM_OK) {
        ArmStdC_strcpy(Filename, MaxFileNameLen, FILE_PATH);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov0.bin");
        ArmLog_STR(TASKNAME, "## filename: %s", Filename, NULL);
        Rval = ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pMetadata->CalibInfo.Left.pBuffer, Length, Filename, &ActualLoadSize);
        ArmLog_U64(TASKNAME, "## metadata left, pBuffer=%llx", (UINT64)pMetadata->CalibInfo.Left.pBuffer, (UINT64)0U);
    }

    if (Rval == ARM_OK) {
        ArmStdC_strcpy(Filename, MaxFileNameLen, FILE_PATH);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov1.bin");
        ArmLog_STR(TASKNAME, "## filename: %s", Filename, NULL);
        Rval = ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pMetadata->CalibInfo.Right.pBuffer, Length, Filename, &ActualLoadSize);
        ArmLog_U64(TASKNAME, "## metadata right, pBuffer=%llx", (UINT64)pMetadata->CalibInfo.Right.pBuffer, (UINT64)0U);
    }

    if (Rval == ARM_OK) {
        Rval = SetFrameNum(pAppAcPicInfo, FrameNum);
    }

    return Rval;
}


void RefCV_AmbaAc_UT(void)
{
    UINT32 Rval = ARM_OK;
    UINT32 FrameNum;
    AppAcPicInfo_t AppAcPicInfo;
    AppAcMetadata_t AppAcMetadata;
    flexidag_memblk_t LumaLeft;
    flexidag_memblk_t LumaRight;
    UINT32 ImageSize = ((1920U*960U) + 1420U);
    UINT32 MetaSize = 3000U;
    memio_source_recv_picinfo_t *pPicInfo;
    AmbaAcConfig_t AmbaAcConfig;

    // alloc memory
    if (Rval == ARM_OK) {
        Rval = AmbaCV_UtilityCmaMemAlloc(gCvmemSize, 1, &gTotalBuf);
        if (Rval != 0) {
            ArmLog_ERR(TASKNAME, "## Can't allocate %u from CmaMem", gCvmemSize, 0U);
        } else {
            //(void)ArmStdC_memset(gTotalBuf.pBuffer, 0U, gTotalBuf.buffer_size);
            Rval = ArmMemPool_Create(&gTotalBuf, &gMemPoolId);
        }
    }
    Rval = ArmMemPool_Allocate(gMemPoolId, sizeof(memio_source_recv_picinfo_t), &(AppAcPicInfo.PicInfo));
    Rval = ArmMemPool_Allocate(gMemPoolId, ImageSize, &(LumaLeft));
    Rval = ArmMemPool_Allocate(gMemPoolId, ImageSize, &(LumaRight));
    Rval = ArmMemPool_Allocate(gMemPoolId, MetaSize, &(AppAcMetadata.CalibInfo.Left));
    Rval = ArmMemPool_Allocate(gMemPoolId, MetaSize, &(AppAcMetadata.CalibInfo.Right));

    // config picinfo
    ArmStdC_memcpy(&pPicInfo, &(AppAcPicInfo.PicInfo.pBuffer), sizeof(void*));
    ConfigPicInfo(pPicInfo, LumaLeft.pBuffer, LumaRight.pBuffer, SCALE_ID);

    (void)RefCV_PrepareInitData(&AppAcMetadata, &AmbaAcConfig, SCALE_ID);
    (void)RefCV_AmbaAcInit(&AppAcMetadata, &AmbaAcConfig, &RefCV_GetResult);

    FrameNum = 0U;
//    while (1U) {
    for (FrameNum = 0U; FrameNum < gTargetFrames; FrameNum++) {
        if ((FrameNum % 1U) == 0U) {
            Rval = RefCV_PrepareOneFrame(FrameNum, &AppAcPicInfo, &LumaLeft, &LumaRight, &AppAcMetadata);
            if (Rval == ARM_OK) {
                Rval = RefCV_AmbaAcFeedOneFrame(FrameNum, &AppAcPicInfo, &AppAcMetadata);
            }
        }
        usleep(100000);
    }
//        if (Rval == ARM_OK) {
//            Rval |= RefCV_AmbaAc_MvacStop();
//            Rval |= RefCV_AmbaAc_MvacStart(&AppAcMetadata, &MvacViewInfo);
//        }
//    }
    RefCV_AmbaAc_ProfilerReport();
}

