/**
 *  @file AmbaImgMainAe.c
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Amba Image Main Ae
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"
#include "AmbaWrap.h"

#include "AmbaKAL.h"

#include "AmbaDSP.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_Image3aStatistics.h"

#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AeAwbAdj_Control.h"
#include "Amba_ExifImg.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainAe.h"
#include "AmbaImgMainAe_Platform.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

#define AMBA_IMG_MAIN_AE_USER_DATA_NUM    16U
#define AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM    16U

typedef struct /*_AMBA_IMG_MAIN_AE_MEM_CTX_s_*/ {
    AMBA_IP_VIN_TONE_CURVE_s  *pVinTone[AMBA_IMG_MAIN_AE_USER_DATA_NUM];
    AMBA_IP_VIN_TONE_CURVE_s  *pSnapVinTone[AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM];
} AMBA_IMG_MAIN_AE_MEM_CTX_s;

typedef union /*_AE_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR          Data;
    void                            *pVoid;
    UINT8                           *pUint8;
    AMBA_IMG_MAIN_AE_USER_DATA_s    *pUser;
    const void                      *pCvoid;
    AMBA_IK_CFA_3A_DATA_s           *pCfa;
    AMBA_IK_PG_3A_DATA_s            *pRgb;
    AMBA_IMG_SENSOR_WB_s            *pWgc;
    AMBA_IP_GLOBAL_AAA_DATA_s       *pAaaData;
    AMBA_IP_VIN_TONE_CURVE_s        *pVinTone;
} AE_MEM_INFO_u;

typedef struct /*_AE_MEM_INFO_s_*/ {
    AE_MEM_INFO_u    Ctx;
} AE_MEM_INFO_s;

typedef union /*_AE_SNAP_MEM_INFO_CTX_s_*/ {
    AMBA_IMG_MAIN_MEM_ADDR               Data;
    void                                 *pVoid;
    UINT8                                *pUint8;
    AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s    *pUser;
    AMBA_IMG_SENSOR_WB_s                 *pWgc;
    EXIF_INFO_s                          *pExifInfo;
} AE_SNAP_MEM_INFO_u;

typedef struct /*_AE_MEM_INFO_s_*/ {
    AE_SNAP_MEM_INFO_u    Ctx;
} AE_SNAP_MEM_INFO_s;

typedef union /*_AE_TASK_PARAM_CTX_s_*/ {
    UINT32 Data;
    struct {
        UINT32 VinId:    16;
        UINT32 ChainId:  16;
    } Bits;
} AE_TASK_PARAM_u;

typedef struct /*_AE_TASK_PARAM_s_*/ {
    AE_TASK_PARAM_u    Ctx;
} AE_TASK_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_AE_TASK_INFO_s_*/ {
    char Name[32];
} AMBA_IMG_MAIN_AE_TASK_INFO_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainAeTaskId[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static void AmbaImgMainAe_Task(UINT32 Param);
static void *AmbaImgMainAe_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_Ready[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_Go[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_Ack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_LockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_UnlockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
#ifdef CONFIG_BUILD_IMGFRW_SMC
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_SmcAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
#endif
static UINT32 AmbaImgMainAe_EnableFlag[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static UINT32 AmbaImgMainAe_StableCheck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static AMBA_KAL_MUTEX_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_StableMutex[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAe_StableAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAe_Cmd[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

static UINT32 AmbaImgMainAe_DummyFlag[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAe_DebugCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
UINT32 AmbaImgMainAe_QueryCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAe_VinNum;

static AMBA_IMG_MAIN_AE_USER_DATA_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_UserData[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_USER_DATA_NUM];

static AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_SnapUserData[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM];
static UINT64 GNU_SECTION_NOZEROINIT AmbaImgMainAe_SnapUser[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM];
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAe_SnapAebCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static EXIF_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_SnapExif[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM];
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAe_SnapExifAebCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

static UINT32 AmbaImgMainAe_MemCtxFlag = 0U;
static AMBA_IMG_MAIN_AE_MEM_CTX_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_MemCtx[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

/**
 *  @private
 *  Amba image main ae create
 *  @param[in] VinId vin id
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_AE_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainAe_TaskStack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_AE_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_TaskInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    AE_TASK_PARAM_s ParamInfo;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task create (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                /* semaphore */
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAe_Ready[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAe_Go[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAe_Ack[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAe_LockAck[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAe_UnlockAck[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
#ifdef CONFIG_BUILD_IMGFRW_SMC
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAe_SmcAck[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
#endif
                FuncRetCode = AmbaKAL_MutexCreate(&AmbaImgMainAe_StableMutex[VinId][i], NULL);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAe_StableAck[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* ae flag reset */
                AmbaImgMainAe_EnableFlag[VinId][i] = 1U;
                AmbaImgMainAe_StableCheck[VinId][i] = 0U;
                /* task param */
                ParamInfo.Ctx.Bits.VinId = (UINT8) VinId;
                ParamInfo.Ctx.Bits.ChainId = (UINT8) i;
                /* task name */
                {
                    char str[11];
                    UINT32 VinStrNum = (AmbaImgMainAe_VinNum > 9U) ? (UINT32) 2U : (UINT32) 1U;
                    AmbaImgMainAe_TaskInfo[VinId][i].Name[0] = '\0';
                    (void) var_strcat(AmbaImgMainAe_TaskInfo[VinId][i].Name, "ImageVin");
                    var_utoa(VinId, str, 10U, VinStrNum, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAe_TaskInfo[VinId][i].Name, str);
                    (void) var_strcat(AmbaImgMainAe_TaskInfo[VinId][i].Name, "Ae");
                    var_utoa(i, str, 10U, 1U, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAe_TaskInfo[VinId][i].Name, str);
                }
                /* task */
                if (RetCode == OK) {
                    AmbaImgPrint2(PRINT_FLAG_MSG, "image create ae task", VinId, i);
                    {
                        AE_MEM_INFO_s MemInfo;
                        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) ParamInfo.Ctx.Data;
                        FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainAeTaskId[VinId][i],
                                                         AmbaImgMainAe_TaskInfo[VinId][i].Name,
                                                         Priority,
                                                         AmbaImgMainAe_TaskEx,
                                                         MemInfo.Ctx.pVoid,
                                                         AmbaImgMainAe_TaskStack[VinId][i],
                                                         AMBA_IMG_MAIN_AE_TASK_STACK_SIZE,
                                                         AMBA_KAL_DONT_START);
                    }
                    if (FuncRetCode == OK) {
                        UINT32 AeCoreInclusion = CoreInclusion;
                        if (AmbaImgMain_ImgAaaCore[VinId][i] > 0U) {
                            AeCoreInclusion = AmbaImgMain_ImgAaaCore[VinId][i];
                        }
                        FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainAeTaskId[VinId][i], AeCoreInclusion);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainAeTaskId[VinId][i]);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                    } else {
                        RetCode = NG_UL;
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae delete
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Delete(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task delete (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                /* task */
                FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainAeTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainAeTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* semaphore */
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAe_Ready[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAe_Go[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAe_Ack[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAe_LockAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAe_UnlockAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
#ifdef CONFIG_BUILD_IMGFRW_SMC
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAe_SmcAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
#endif
                FuncRetCode = AmbaKAL_MutexDelete(&AmbaImgMainAe_StableMutex[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAe_StableAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* ae flag reset */
                AmbaImgMainAe_EnableFlag[VinId][i] = 0U;
                AmbaImgMainAe_StableCheck[VinId][i] = 0U;
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae memory size query
 *  @return memory size
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_MemSizeQuery(void)
{
    UINT32 MemSize;
    UINT32 MemSizeAlign;

    UINT32 TotalMemSizeAlign;

    /* mem size get */
    MemSize = (UINT32) sizeof(AMBA_IP_VIN_TONE_CURVE_s);
    /* alignment */
    MemSizeAlign = MemSize + ((((UINT32) AMBA_CACHE_LINE_SIZE) - (MemSize % ((UINT32) AMBA_CACHE_LINE_SIZE))) % ((UINT32) AMBA_CACHE_LINE_SIZE));

    /* total mem size get */
    TotalMemSizeAlign = MemSizeAlign * (AMBA_IMG_NUM_VIN_CHANNEL * AMBA_IMG_NUM_VIN_ALGO * AMBA_IMG_MAIN_AE_USER_DATA_NUM);
    TotalMemSizeAlign += MemSizeAlign * (AMBA_IMG_NUM_VIN_CHANNEL * AMBA_IMG_NUM_VIN_ALGO * AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM);

    //AmbaImgPrint2(PRINT_FLAG_MSG, "image mem size query", MemSizeAlign, TotalMemSizeAlign);

    return TotalMemSizeAlign;
}

/**
 *  @private
 *  Amba image main ae memory init
 *  @param[in] pMem pointer to memory buffer
 *  @param[in] MemSizeInByte memory size in byte
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_MemInit(void *pMem, UINT32 MemSizeInByte)
{
    UINT32 RetCode = OK_UL;
    UINT32 i, j, k;

    AE_MEM_INFO_s MemInfo;
    AE_MEM_INFO_s MemInfo2;

    UINT32 MemSize;
    UINT32 MemSizeAlign;

    UINT32 Index;
    UINT32 Index2;

    /* mem get */
    MemInfo.Ctx.pVoid = pMem;
    /* mem size get */
    MemSize = (UINT32) sizeof(AMBA_IP_VIN_TONE_CURVE_s);
    /* alignment */
    MemSizeAlign = MemSize + ((((UINT32) AMBA_CACHE_LINE_SIZE) - (MemSize % ((UINT32) AMBA_CACHE_LINE_SIZE))) % ((UINT32) AMBA_CACHE_LINE_SIZE));

    /* video vin tone curve */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            for (k = 0U; k < AMBA_IMG_MAIN_AE_USER_DATA_NUM; k++) {
                Index = MemSizeAlign * ((i * AMBA_IMG_NUM_VIN_ALGO * AMBA_IMG_MAIN_AE_USER_DATA_NUM) + (j * AMBA_IMG_MAIN_AE_USER_DATA_NUM) + k);
                if (Index < MemSizeInByte) {
                    MemInfo2.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[Index]);
                    AmbaImgMainAe_MemCtx[i][j].pVinTone[k] = MemInfo2.Ctx.pVinTone;
                    //AmbaImgPrintEx(PRINT_FLAG_MSG, "image mem video", (UINT32) MemInfo2.Ctx.Data, 16U);
                } else {
                    /* index exceed */
                    RetCode = NG_UL;
                }
            }
        }
    }
    /* snap vin tone curve */
    Index2 = MemSizeAlign * (AMBA_IMG_NUM_VIN_CHANNEL * AMBA_IMG_NUM_VIN_ALGO * AMBA_IMG_MAIN_AE_USER_DATA_NUM);
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[Index2]);
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            for (k = 0U; k < AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM; k++) {
                Index2 = MemSizeAlign * ((i * AMBA_IMG_NUM_VIN_ALGO * AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM) + (j * AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM) + k);
                if ((Index + Index2) < MemSizeInByte) {
                    MemInfo2.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[Index2]);
                    AmbaImgMainAe_MemCtx[i][j].pSnapVinTone[k] = MemInfo2.Ctx.pVinTone;
                    //AmbaImgPrintEx(PRINT_FLAG_MSG, "image mem snap", (UINT32) MemInfo2.Ctx.Data, 16U);
                } else {
                    /* index exceed */
                    RetCode = NG_UL;
                }
            }
        }
    }

    if (RetCode == NG_UL) {
        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image mem ae init fail");
        AmbaImgPrint_Flush();
    } else {
        AmbaImgMainAe_MemCtxFlag = 1U;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Init(void)
{
    /* compatible for app */
    static AMBA_IP_VIN_TONE_CURVE_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_VideoVinToneCurve[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_USER_DATA_NUM];
    static AMBA_IP_VIN_TONE_CURVE_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_StillVinToneCurve[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM];

    UINT32 RetCode = OK_UL;
    UINT32 i, j, k;

    /* ae task dummy flag init */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgMainAe_DummyFlag[i][j] = 1U;
        }
    }
    /* ae debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
                AmbaImgMainAe_DebugCount[i][j] = 0xFFFFFFFFU;
                AmbaImgMainAe_QueryCount[i][j] = 0xFFFFFFFFU;
            }
        }
    }

    /* hook vin tone curve */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            /* video */
            for (k = 0U; k < AMBA_IMG_MAIN_AE_USER_DATA_NUM; k++) {
                if (AmbaImgMainAe_MemCtxFlag == 1U) {
                    /* allocated */
                    AmbaImgMainAe_UserData[i][j][k].HdrInfo.pVinToneCurve = AmbaImgMainAe_MemCtx[i][j].pVinTone[k];
                } else {
                    /* static (compatible for app) */
                    AmbaImgMainAe_UserData[i][j][k].HdrInfo.pVinToneCurve = &(AmbaImgMainAe_VideoVinToneCurve[i][j][k]);
                }
            }
            /* still */
            for (k = 0U; k < AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM; k++) {
                if (AmbaImgMainAe_MemCtxFlag == 1U) {
                    /* allocated */
                    AmbaImgMainAe_SnapUserData[i][j][k].HdrInfo.pVinToneCurve = AmbaImgMainAe_MemCtx[i][j].pSnapVinTone[k];
                } else {
                    /* static (compatible for app) */
                    AmbaImgMainAe_SnapUserData[i][j][k].HdrInfo.pVinToneCurve = &(AmbaImgMainAe_StillVinToneCurve[i][j][k]);
                }
            }
            /* update flag reset */
            AmbaImgMain_VinToneCurveUpd[i][j] = IMG_MAIN_VIN_TONE_CURVE_RUM_TIME;
        }
    }

    AmbaImgMainAe_VinNum = AMBA_IMG_NUM_VIN_CHANNEL;

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae active
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Active(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task active (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                if (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_STOP) {
                    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAe_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* cmd start */
                        AmbaImgMainAe_Cmd[VinId][i] = (UINT32) AE_TASK_CMD_START;
                        /* task go */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAe_Go[VinId][i]);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* ack take */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAe_Ack[VinId][i], AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                        } else {
                            RetCode = NG_UL;
                        }
                    } else {
                        RetCode = NG_UL;
                    }
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae inactive
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Inactive(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task inactive (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                if ((AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_START) ||
                    (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_LOCK_DOWN)) {
                    AmbaImgMainAe_Cmd[VinId][i] = (UINT32) AE_TASK_CMD_STOP;
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae idle wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_IdleWait(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task idlewait (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAe_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAe_Ready[VinId][i]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae lock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Lock(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task active (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                if (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_START) {
                    /* lock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_LockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* lock cmd */
                    AmbaImgMainAe_Cmd[VinId][i] = (UINT32) AE_TASK_CMD_LOCK;
                } else {
                    /* cmd fail */
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae lock wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_LockWait(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task active (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                if ((AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_LOCK) ||
                    (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_LOCK_DOWN)) {
                    /* lock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_LockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgPrint2(PRINT_FLAG_LOCK, "ae lock ack", VinId, (UINT32) ImageChanId.Ctx.Bits.AlgoId);
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    /* cmd fail */
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae unlock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Unlock(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task active (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                if ((AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_LOCK) ||
                    (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_LOCK_DOWN)) {
                    /* unlock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_UnlockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* unlock cmd */
                    AmbaImgMainAe_Cmd[VinId][i] = (UINT32) AE_TASK_CMD_UNLOCK;
                    /* unlock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_UnlockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                } else {
                    /* cmd fail */
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_SMC
/**
 *  @private
 *  Amba image main ae smc on
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_SmcOn(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task active (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                if (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_START) {
                    /* smc cmd on */
                    AmbaImgMainAe_Cmd[VinId][i] = (UINT32) AE_TASK_CMD_SMC_ON;
                    /* smc ack take */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_SmcAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae smc off
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_SmcOff(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* vr map get */
            pVrMap = &(pAmbaImgMainChannel[VinId][i].pCtx->VrMap);

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;
            /* vr id get */
            VrId = pVrMap->Id.Ctx.Bits.VrId;
            /* vr alt id get */
            VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

            /* task active (non-vr/vr-master) */
            if ((VrId == 0U/*non-vr*/) ||
                (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)/*vr master*/)) {
                if (AmbaImgMainAe_Cmd[VinId][i] == (UINT32) AE_TASK_CMD_START) {
                    /* smc cmd off */
                    AmbaImgMainAe_Cmd[VinId][i] = (UINT32) AE_TASK_CMD_SMC_OFF;
                    /* smc ack take */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_SmcAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}
#endif
/**
 *  @private
 *  Amba image main ae stable wait
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] Timeout timeout
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_StableWait(UINT32 VinId, UINT32 ChainId, UINT32 Timeout)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 mVinId = 0U;
    UINT32 mChainId = 0U;

    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        /* image channel get */
        ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][ChainId].pCtx->Id.Ctx.Data;
        /* vr map get */
        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
        /* vin id get */
        mVinId = ImageChanId.Ctx.Bits.VinId;
        /* chain id get */
        mChainId = ImageChanId.Ctx.Bits.ChainId;
        /* vr id get */
        VrId = pVrMap->Id.Ctx.Bits.VrId;
        /* vr alt id get */
        VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;

        /* vr slave check */
        if ((VrId > 0U) &&
            (((VrId & (((UINT32) 1U) << mVinId)) == 0U) || ((VrAltId & (((UINT32) 1U) << mChainId)) == 0U))) {
            /* master vin id */
            for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                if ((VrId & (((UINT32) 1U) << i)) > 0U) {
                    mVinId = i;
                    break;
                }
            }
            /* master chain id */
            for (i = 0U; i < AMBA_IMG_NUM_VIN_ALGO; i++) {
                if ((VrAltId & (((UINT32) 1U) << i)) > 0U) {
                    mChainId = i;
                    break;
                }
            }
        }
        if (AmbaImgMainAe_EnableFlag[mVinId][mChainId] > 0U) {
            /* stable wait */
            if (AmbaImgMainAe_Cmd[mVinId][mChainId] == (UINT32) AE_TASK_CMD_START) {
                /* mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(AmbaImgMainAe_StableMutex[mVinId][mChainId]), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* stable ack clear */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_StableAck[mVinId][mChainId]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* stable flag */
                    AmbaImgMainAe_StableCheck[mVinId][mChainId] = 1;
                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(AmbaImgMainAe_StableMutex[mVinId][mChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* stable ack take */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAe_StableAck[mVinId][mChainId]), Timeout);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                } else {
                    /* mutex fail */
                    RetCode = NG_UL;
                }
            } else {
                /* run fail */
                RetCode = NG_UL;
            }
        } else {
            /* enable fail */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image main ae framerate get
 *  @param[in] ImageChanId image channel id
 *  @param[out] pFramerate pointer to the framerate data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAe_FramerateGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 *pFramerate)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
    if (FuncRetCode == OK_UL) {
        *pFramerate = pInfo->Frame.Rate*10U/(((UINT32) 1U) << (pInfo->Op.Status.SsIndex & 0x1FU));
    } else {
        /* info invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image main ae zone to algorithm channel get
 *  @param[in] pCtx pointer to the image channel context data
 *  @param[out] pZone2AlgoTable pointer to the zone to algorithm channel table
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAe_Zone2AlgoGet(const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx, UINT32 *pZone2AlgoTable)
{
    UINT32 i, j, k;

    UINT32 VinId = pCtx->Id.Ctx.Bits.VinId;
    UINT32 ChainId = pCtx->Id.Ctx.Bits.ChainId;
    UINT32 StatZoneId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    if (pCtx->VrMap.Id.Ctx.Bits.VrId == 0U) {
        /* non-vr */
        AmbaImgPrintStr(PRINT_FLAG_DBG, "zone2algo:");
        /* zone id get */
        StatZoneId = pCtx->Id.Ctx.Bits.ZoneId;
        /* zone to algo */
        for (k = 0U; (k < AMBA_IMG_NUM_FOV_CHANNEL) && ((StatZoneId >> k) > 0U); k++) {
            if ((StatZoneId & (((UINT32) 1U) << k)) > 0U) {
                pZone2AlgoTable[k] =  pCtx->Id.Ctx.Bits.AlgoId;
                AmbaImgPrint2(PRINT_FLAG_DBG, "zone 2 algo", k,  pZone2AlgoTable[k]);
            }
        }
    } else {
        /* vr-master */
        AmbaImgPrintStr(PRINT_FLAG_DBG, "zone2algo (vr):");
        /* vin check */
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            if (pCtx->VrMap.Flag[i] > 0U) {
                /* algo check */
                for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
                    /* is families */
                    if ((pCtx->VrMap.Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                        /* image channel id get */
                        ImageChanId2.Ctx.Data = pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Data;
                        /* zone id get */
                        if ((i == VinId) && (j == ChainId)) {
                            /* vr master */
                            StatZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId2);
                        } else {
                            /* vr salve */
                            StatZoneId = ImageChanId2.Ctx.Bits.ZoneId;
                        }
                        /* zone to algo */
                        for (k = 0U; (k < AMBA_IMG_NUM_FOV_CHANNEL) && ((StatZoneId >> k) > 0U); k++) {
                            if ((StatZoneId & (((UINT32) 1U) << k)) > 0U) {
                                pZone2AlgoTable[k] = ImageChanId2.Ctx.Bits.AlgoId;
                                AmbaImgPrint2(PRINT_FLAG_DBG, "zone 2 algo", k,  pZone2AlgoTable[k]);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 *  Amba image main ae vr statistics processing
 *  @param[in] pPort pointer to the statistics port data
 *  @param[in] pZone2AlgoTable pointer to the zone to algorithm channel table
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAe_VrStatProc(const AMBA_IMG_STATISTICS_PORT_s *pPort, const UINT32 *pZone2AlgoTable)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 AlgoId = pPort->ImageChanId.Ctx.Bits.AlgoId;
    UINT32 StatZoneId = pPort->ZoneId.Req;

    UINT32 StatCnt = 0U;

    UINT32 TileRow;
    UINT32 TileCol;

    AE_MEM_INFO_s MemInfo;
    AMBA_IP_GLOBAL_AAA_DATA_s *pAaaData = NULL;

    INT32 Weighting[MAX_VIEW_NUM];
    UINT32 WeightingU;

    /* algo aaa buffer get */
    FuncRetCode = AmbaImgProc_GAAAGetStat(AlgoId, &pAaaData);
    if (FuncRetCode == OK) {
        /* algo aaa weighting get */
        FuncRetCode = AmbaImgProc_GAAAGetWeighting(AlgoId, Weighting);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* cfa histo reset */
        for (i = 0U; i < AMBA_IK_CFA_HISTO_COUNT; i++) {
            pAaaData->CfaHist.HisBinY[i] = 0U;
            pAaaData->CfaHist.HisBinR[i] = 0U;
            pAaaData->CfaHist.HisBinG[i] = 0U;
            pAaaData->CfaHist.HisBinB[i] = 0U;
        }

        /* rgb histo reset */
        for (i = 0U; i < AMBA_IK_PG_HISTO_COUNT; i++) {
            pAaaData->PgHist.HisBinY[i] = 0U;
            pAaaData->PgHist.HisBinR[i] = 0U;
            pAaaData->PgHist.HisBinG[i] = 0U;
            pAaaData->PgHist.HisBinB[i] = 0U;
        }

        for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((StatZoneId >> i) > 0U); i++) {
            if ((StatZoneId & (((UINT32) 1U) << i)) > 0U) {
                /* cfa get */
                MemInfo.Ctx.pCvoid = pPort->Data.pCfa[i];

                /* cfa header */
                if (StatCnt == 0U) {
                    FuncRetCode = AmbaWrap_memcpy(&(pAaaData->Header), &(MemInfo.Ctx.pCfa->Header), sizeof(AMBA_IK_3A_HEADER_s));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }

                /* cfa ae */
                TileRow = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumRow;
                TileCol = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AE.AeTileNumCol;

                FuncRetCode = AmbaWrap_memcpy(&(pAaaData->Ae[TileRow*TileCol*StatCnt]), MemInfo.Ctx.pCfa->Ae, sizeof(AMBA_IK_CFA_AE_s)*TileRow*TileCol);
                if (FuncRetCode != OK_UL) {
                    /* */
                }

                /* cfa awb */
                TileRow = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumRow;
                TileCol = MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumCol;

                FuncRetCode = AmbaWrap_memcpy(&(pAaaData->Awb[TileRow*TileCol*StatCnt]), MemInfo.Ctx.pCfa->Awb, sizeof(AMBA_IK_CFA_AWB_s)*TileRow*TileCol);
                if (FuncRetCode != OK_UL) {
                    /* */
                }

                /* weighting*/
                WeightingU = (Weighting[StatCnt] > 0) ? (UINT32) Weighting[StatCnt] : 0U;

                /* cfa histo */
                for (j = 0U; j < AMBA_IK_CFA_HISTO_COUNT; j++) {
                    pAaaData->CfaHist.HisBinY[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinY[j]*WeightingU);
                    pAaaData->CfaHist.HisBinR[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinR[j]*WeightingU);
                    pAaaData->CfaHist.HisBinG[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinG[j]*WeightingU);
                    pAaaData->CfaHist.HisBinB[j] += (MemInfo.Ctx.pCfa->Histogram.HisBinB[j]*WeightingU);
                }

                /* rgb get */
                MemInfo.Ctx.pCvoid = pPort->Data.pRgb[i];

                /* rgb histo */
                for (j = 0U; j < AMBA_IK_PG_HISTO_COUNT; j++) {
                    pAaaData->PgHist.HisBinY[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinY[j]*WeightingU);
                    pAaaData->PgHist.HisBinR[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinR[j]*WeightingU);
                    pAaaData->PgHist.HisBinG[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinG[j]*WeightingU);
                    pAaaData->PgHist.HisBinB[j] += (MemInfo.Ctx.pRgb->Histogram.HisBinB[j]*WeightingU);
                }

                /* stat algo id put */
                pAaaData->StatisticMap[StatCnt] = pZone2AlgoTable[i];

                /* count update */
                StatCnt++;
            }
        }

        /* stat count */
        pAaaData->StatCount = StatCnt;

        /* debug only */
        if (AmbaImgMainAe_DebugCount[pPort->ImageChanId.Ctx.Bits.VinId][pPort->ImageChanId.Ctx.Bits.ChainId] < AmbaImgMain_DebugCount) {
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "master algo id", pPort->ImageChanId.Ctx.Bits.VinId, AlgoId);
            MemInfo.Ctx.pAaaData = pAaaData;
            AmbaImgPrint64Ex2(PRINT_FLAG_DBG, "algo aaa buffer", (UINT64) pPort->ImageChanId.Ctx.Bits.VinId, 10U, (UINT64) MemInfo.Ctx.Data, 16U);
            j = 0U;
            for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((StatZoneId >> i) > 0U); i++) {
                if ((StatZoneId & (((UINT32) 1U) << i)) > 0U) {
                    AmbaImgPrint2(PRINT_FLAG_DBG, "zone 2 algo", i, pZone2AlgoTable[i]);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "weighting", j, (Weighting[j] > 0) ? (UINT32) Weighting[j] : 0U);
                    j++;
                }
            }
            AmbaImgPrint2(PRINT_FLAG_DBG, "stat count", pPort->ImageChanId.Ctx.Bits.VinId, pAaaData->StatCount);
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
        }
    }
}

/**
 *  Amba image main ae task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainAe_TaskEx(void *pParam)
{
    AE_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainAe_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main ae task
 *  @param[in] Param vin id with the chain id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAe_Task(UINT32 Param)
{
    static UINT32 AmbaImgMainAe_Zone2AlgoTable[32] = {0};

    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i, j;
     INT32 m;

    AE_MEM_INFO_s MemInfo;
    AE_MEM_INFO_s MemInfo2;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    static AMBA_IMG_STATISTICS_PORT_s AmbaImgMainAe_StatPort[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
    AMBA_IMG_STATISTICS_PORT_s *pPort;

    AE_TASK_PARAM_s TaskParam;
    UINT32 VinId;
    UINT32 ChainId;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 VrEnable = 0U;
    UINT32 VrMaster = 0U;

    AMBA_AE_INFO_s    *pAeInfo;
    const AMBA_DGAIN_INFO_s *pAuxAeInfo;
    AMBA_IK_WB_GAIN_s *pWbGain;
    AMBA_IMG_SENSOR_WB_s *pSensorWb;

    UINT32 TimingEnFlag = 0U;

    FLOAT VarF;
    UINT32 Factor[AMBA_IMG_MAX_EXPOSURE_NUM];

    UINT32 *pAgc;
    UINT32 *pDgc;
    const UINT32 *pDDgc;
    UINT32 *pShr;

    AMBA_IMG_SENSOR_SYNC_DATA_s SyncData;

    AMBA_3A_STATUS_s VideoStatus;
    AMBA_3A_STATUS_s StillStatus;

    UINT32 CurrMode;
    UINT32 NextMode;
    UINT32 PreviewMode;

    UINT32 AlgoId = 0U;
    UINT32 AlgoStatus = 0U;
    UINT32 StatisticsIndex = 0U;

    UINT32 HdrEnable = 0U;
    UINT32 ExposureNum = 1U;

    AMBA_AAA_OP_INFO_s AaaOpInfo;

    UINT32 ZoneId;
    UINT32 Framerate = 0U;

    UINT32 AeUpdate;
    UINT32 HdrPipeUpdate;
    UINT32 HdrWbUpdate;

    UINT32 ShutterTimeUL;
    UINT32 AgcGainUL;
    UINT32 SlsGainUL;
    UINT32 LastShutterTimeUL;
    UINT32 LastAgcGainUL;
    UINT32 LastSlsGainUL;

    UINT32 WbPosition = 0U;
    UINT32 WbId = (UINT32) WB_ID_FE;

    UINT32 SyncDgId = AMBA_IMG_MAIN_AE_SYNC_DGCID;

    UINT32 SyncMode;
    UINT32 SyncPreAikFlag = 0U;

    AMBA_IK_WB_GAIN_s LastWbGain[AMBA_IMG_MAX_EXPOSURE_NUM];
    AMBA_AE_INFO_s LastAeInfo[AMBA_IMG_MAX_EXPOSURE_NUM];
    AMBA_DGAIN_INFO_s LastAuxAeInfo[AMBA_IMG_MAX_EXPOSURE_NUM];

    AMBA_IMG_CHANNEL_CONTEXT_s *pCtx = NULL;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo = NULL;

    /* task param */
    TaskParam.Ctx.Data = Param;
    /* vin id */
    VinId = TaskParam.Ctx.Bits.VinId;
    /* chain id */
    ChainId = TaskParam.Ctx.Bits.ChainId;

    /* param init */
    ImageChanId.Ctx.Data = 0ULL;
    MemInfo.Ctx.Data = 0U;
    MemInfo2.Ctx.Data = 0U;

    /* param reset */
    FuncRetCode = AmbaWrap_memset(Factor, 0, sizeof(Factor));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* status reset */
    FuncRetCode = AmbaWrap_memset(&VideoStatus, 0, sizeof(VideoStatus));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(&StillStatus, 0, sizeof(StillStatus));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* info reset */
    FuncRetCode = AmbaWrap_memset(LastWbGain, 0, sizeof(LastWbGain));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(LastAeInfo, 0, sizeof(LastAeInfo));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(LastAuxAeInfo, 0, sizeof(LastAuxAeInfo));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    pPort = &(AmbaImgMainAe_StatPort[VinId][ChainId]);

    AmbaImgMainAe_Cmd[VinId][ChainId] = (UINT32) AE_TASK_CMD_STOP;

    while (AmbaImgMainAe_DummyFlag[VinId][ChainId] == 1U) {
        if (AmbaImgMainAe_Cmd[VinId][ChainId] == (UINT32) AE_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                /* efov tx rx cb unhook */
                pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AE] = NULL;
                pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AE] = NULL;
#endif
                /* statistics close */
                FuncRetCode = AmbaImgStatistics_Close(pPort);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae statistics close", VinId, FuncRetCode);
                }
            }

            /* task rdy2go */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAe_Ready[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae task rdy2go", VinId, FuncRetCode);
            }
            /* task wait2go */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAe_Go[VinId][ChainId], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae task wait2go", VinId, FuncRetCode);
            }

            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* ctx check */
            if (pCtx == NULL) {
                AmbaImgMainAe_Cmd[VinId][ChainId] = (UINT32) AE_TASK_CMD_STOP;
                continue;
            }

            /* hdr id reset */
            pCtx->Id.Ctx.Bits.HdrId = 0U;

            /* hdr info get */
            HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
            ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "hdr enable", VinId, HdrEnable);
            AmbaImgPrint2(PRINT_FLAG_DBG, "exposure num", VinId, ExposureNum);

            /* hdr id get */
            if (HdrEnable > 0U) {
                UINT32 HdrId = 0U;
                for (i = 0U; i < ExposureNum; i++) {
                    HdrId |= (((UINT32) 1U) << i);
                }
                pCtx->Id.Ctx.Bits.HdrId = (UINT8) HdrId;
            }

            /* image channel get */
            ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
            /* sync pre aik flag */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                if (pInfo->Op.Ctrl.PreAikCmd > 0U) {
                    SyncPreAikFlag = 1U;
                }
            }
            /* algo id get */
            AlgoId = ImageChanId.Ctx.Bits.AlgoId;

            /* statistics index */
            ZoneId = ImageChanId.Ctx.Bits.ZoneId;
            if (ImageChanId.Ctx.Bits.ZoneMsb == 0U) {
                for (m = 0; m < (INT32) AMBA_IMG_NUM_FOV_CHANNEL; m++) {
                    if ((ZoneId & (((UINT32) 1U) << ((UINT32) m))) > 0U) {
                        StatisticsIndex = (UINT32) m;
                        break;
                    }
                }
            } else {
                for (m = (((INT32) AMBA_IMG_NUM_FOV_CHANNEL) - 1); m >= 0; m--) {
                    if ((ZoneId & (((UINT32) 1U) << ((UINT32) m))) > 0U) {
                        StatisticsIndex = (UINT32) m;
                        break;
                    }
                }
            }

            /* vr id get */
            VrId = (UINT32) pCtx->VrMap.Id.Ctx.Bits.VrId;
            VrAltId = (UINT32) pCtx->VrMap.Id.Ctx.Bits.VrAltId;

            /* vr check */
            if (VrId > 0U) {
                VrEnable = 1U;
                if (((VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                    ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                    FuncRetCode = AmbaImgProc_GAAASetEnb(AlgoId, 1U);
                    if (FuncRetCode != OK) {
                        /* */
                    }
                    VrMaster = 1U;
                } else {
                    FuncRetCode = AmbaImgProc_GAAASetEnb(AlgoId, 0U);
                    if (FuncRetCode != OK) {
                        /* */
                    }
                    VrMaster = 0U;
                }
            } else {
                FuncRetCode = AmbaImgProc_GAAASetEnb(AlgoId, 0U);
                if (FuncRetCode != OK) {
                    /* */
                }
                VrEnable = 0U;
                VrMaster = 0U;
            }

            /* zone 2 algo table */
            AmbaImgMainAe_Zone2AlgoGet(pCtx, AmbaImgMainAe_Zone2AlgoTable);

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "vin id", VinId, ImageChanId.Ctx.Bits.VinId);
            AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, ImageChanId.Ctx.Bits.ChainId);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "sensor id", VinId, 10U, ImageChanId.Ctx.Bits.SensorId, 16U);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "hdr id", VinId, 10U, ImageChanId.Ctx.Bits.HdrId, 16U);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "vr id", VinId, 10U, pCtx->VrMap.Id.Ctx.Bits.VrId, 16U);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "vr alt id", VinId, 10U, pCtx->VrMap.Id.Ctx.Bits.VrAltId, 16U);
            AmbaImgPrint2(PRINT_FLAG_DBG, "algo id", VinId, AlgoId);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "zone id", VinId, 10U, ImageChanId.Ctx.Bits.ZoneId, 16U);
            AmbaImgPrint2(PRINT_FLAG_DBG, "statistics index", VinId, StatisticsIndex);

            AmbaImgPrint2(PRINT_FLAG_DBG, "vr enable", VinId, VrEnable);
            AmbaImgPrint2(PRINT_FLAG_DBG, "vr master", VinId, VrMaster);

            /* aik proc hook */
            pAmbaImgMainChannel[VinId][ChainId].AikProc = AmbaImgMainAe_AikProc;
            /* sls cb hook */
            pAmbaImgMainChannel[VinId][ChainId].SlsCb = AmbaImgMainAe_SlsCb;
#ifdef CONFIG_BUILD_IMGFRW_EFOV
            /* efov tx rx cb hook */
            if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_NONE) {
                /* none */
                pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AE] = NULL;
                pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AE] = NULL;
            } else {
                if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                    /* master */
                    pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AE] = AmbaImgMainAe_EFovTxCb;
                    pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AE] = NULL;
                } else {
                    /* slave */
                    pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AE] = NULL;
                    pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AE] = AmbaImgMainAe_EFovRxCb;
                }
            }

            AmbaImgPrint2(PRINT_FLAG_DBG, "efov mode", VinId, (UINT32) pCtx->EFov.Ctx.Bits.Mode);
#endif
            /* user data buffer register */
            MemInfo.Ctx.pUser = AmbaImgMainAe_UserData[VinId][ChainId];
            FuncRetCode = AmbaImgSensorSync_UserBuffer(ImageChanId, (UINT32) sizeof(AMBA_IMG_MAIN_AE_USER_DATA_s), AMBA_IMG_MAIN_AE_USER_DATA_NUM, MemInfo.Ctx.pUint8);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae user data register", VinId, FuncRetCode);
            }

            /* debug only */
            if (AmbaImgMainAe_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                /* timing mark enable */
                FuncRetCode = AmbaImgSensorHAL_TimingMarkEn(VinId, 1U);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaImgSystem_Debug(VinId, 3U);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                TimingEnFlag = 1U;
            } else {
                TimingEnFlag = 0U;
            }

            /* ae mode preset */
            CurrMode = IP_PREVIEW_MODE;
            NextMode = IP_PREVIEW_MODE;
            FuncRetCode = AmbaImgProc_SetMode(AlgoId, &CurrMode, &NextMode);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* photo preview? */
            if (AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode > (UINT8) VIDEO_OP_PHOTO_PREVIEW) {
                /* 2: raw enc */
                PreviewMode = (UINT32) VIDEO_OP_VIDEO;
            } else {
                /* 0: video, 1: photo preview */
                PreviewMode = AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode;
            }
            /* algo preview mode */
            FuncRetCode = AmbaImgProc_SetPhotoPreview(AlgoId, &PreviewMode);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            AmbaImgPrint2(PRINT_FLAG_MSG, "ae preview mode", VinId, PreviewMode);
            /* algo status reset (ae) */
            VideoStatus.Ae = AMBA_IDLE;
            StillStatus.Ae = AMBA_IDLE;
            FuncRetCode = AmbaImgProc_SetAEStatus(AlgoId, VideoStatus.Ae, StillStatus.Ae);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* frame rate */
            FuncRetCode = AmbaImgMainAe_FramerateGet(ImageChanId, &Framerate);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            AmbaImgPrint2(PRINT_FLAG_DBG, "ae frame rate", VinId, Framerate);
            FuncRetCode = AmbaImgProc_SetFrameRate(AlgoId, Framerate, 1000U);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* flicker */
            FuncRetCode = AmbaImgProc_AESetFlickerCMD(AlgoId, 0U);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* ae init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "ae algo init...", VinId, AlgoId);
            FuncRetCode = AmbaImgProc_AEInit(AlgoId, 0U);
            if(FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae algo init", VinId, AlgoId);
            }

            /* hdr? */
            if (HdrEnable > 0U) {
                /* awb init */
                AmbaImgPrint2(PRINT_FLAG_MSG, "awb algo init...", VinId, AlgoId);
                FuncRetCode = AmbaImgProc_AWBInit(AlgoId, 0U);
                if (FuncRetCode != OK) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb algo init", VinId, AlgoId);
                }
                /* adj init */
                AmbaImgPrint2(PRINT_FLAG_MSG, "adj algo init...", VinId, AlgoId);
                FuncRetCode = Amba_AdjInit(AlgoId);
                if (FuncRetCode != OK) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj algo init", VinId, AlgoId);
                }
            } else {
                /* adj init */
                AmbaImgPrint2(PRINT_FLAG_MSG, "adj algo init...", VinId, AlgoId);
                FuncRetCode = Amba_AdjInit(AlgoId);
                if (FuncRetCode != OK) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj algo init", VinId, AlgoId);
                }
            }

            /* wb position get (adj init needed) */
            FuncRetCode = AmbaImgProc_AWBGetWBPosition(AlgoId, &WbPosition);
            if (FuncRetCode == OK) {
                switch (WbPosition) {
                    case 16U:
                        WbId = (UINT32) WB_ID_SENSOR;
                        break;
                    case 32U:
                        WbId = (UINT32) WB_ID_FE;
                        break;
                    case 48U:
                        WbId = (UINT32) WB_ID_BE;
                        break;
                    case 64U:
                        WbId = (UINT32) WB_ID_BE_ALT;
                        break;
                    default:
                        /* */
                        break;
                }
            }
            AmbaImgPrint2(PRINT_FLAG_DBG, "wb position", VinId, WbPosition);
            AmbaImgPrint2(PRINT_FLAG_DBG, "wb id", VinId, WbId);

            /* user ae buf get */
            MemInfo.Ctx.pVoid = AmbaImgSensorSync_UserBufferGet(ImageChanId);

            /* user ae info */
            pAeInfo = MemInfo.Ctx.pUser->AeInfo.Ctx;
            /* user aux ae info */
            pAuxAeInfo = MemInfo.Ctx.pUser->AeInfo.AuxCtx;
            /* user ae shr/agc/dgc */
            pShr = MemInfo.Ctx.pUser->AeInfo.Sensor.Shr;
            pAgc = MemInfo.Ctx.pUser->AeInfo.Sensor.Agc;
            pDgc = MemInfo.Ctx.pUser->AeInfo.Sensor.Dgc;
            /* dg id put */
            FuncRetCode = AmbaImgMainAe_DgcPositionGet(&SyncDgId);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            MemInfo.Ctx.pUser->AeInfo.Dsp.SyncDgId = SyncDgId;
            /* user ae ddgc */
            switch (SyncDgId) {
                case SYNC_WB_ID_FE:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.FeDgc;
                    break;
                case SYNC_WB_ID_BE:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
                    break;
                case SYNC_WB_ID_BE_ALT:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[1];
                    break;
                default:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
                    break;
            }

            /* user wb info */
            pWbGain = MemInfo.Ctx.pUser->WbInfo.Ctx;
            /* wb id put */
            MemInfo.Ctx.pUser->WbInfo.Dsp.WbId = WbId;
            /* user sensor wgc */
            pSensorWb = MemInfo.Ctx.pUser->WbInfo.Sensor.Wgc;

            /* ae/awb info get */
            for (i = 0U; i < ExposureNum; i++) {
                /* ae info get */
                FuncRetCode = AmbaImgProc_AEGetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(pAeInfo[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* hdr? */
                if (HdrEnable > 0U) {
                    /* awb info get */
                    FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_VIDEO, &(pWbGain[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* hdr? */
            if (HdrEnable > 0U) {
                /* hdr control */
                Amba_Adj_VideoHdr_Control(AlgoId, pAeInfo, pWbGain);
            }

            /* gain rebuild for algo */
            FuncRetCode = AmbaImgMainAe_UserGainPut(ImageChanId, MemInfo.Ctx.pVoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* agc/dgc/wgc transfer */
            for (i = 0U; i < ExposureNum; i++) {
                VarF = pAeInfo[i].AgcGain * 4096.0f;
                Factor[i] = (UINT32) VarF;
            }
            FuncRetCode = AmbaImgSensorDrv_GainConvert(ImageChanId, Factor, pAgc, pDgc, pSensorWb);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* shr transfer */
            for (i = 0U; i < ExposureNum; i++) {
                VarF = pAeInfo[i].ShutterTime * 1024.0f * 1024.0f;
                Factor[i] = (UINT32) VarF;
            }
            FuncRetCode = AmbaImgSensorDrv_ShutterConvert(ImageChanId, Factor, pShr);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* user data put */
            FuncRetCode = AmbaImgMainAe_UserDataPut(ImageChanId, MemInfo.Ctx.pVoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* last info update */
            if (HdrEnable > 0U) {
                for (i = 0U; i < ExposureNum; i++) {
                    /* last wb get */
                    LastWbGain[i] = pWbGain[i];
                }
            }

            /* agc/dgc, shr attach and hdr preload */
            for (m = ((INT32) ExposureNum) - 1; m >= 0; m--) {
                /* agc */
                SyncData.Agc.Ctx.Bits.Agc = pAgc[m];
                /* wgc */
                MemInfo2.Ctx.pWgc = &(pSensorWb[m]);
                SyncData.Wgc.Ctx.Bits.Wgc = (UINT64) MemInfo2.Ctx.Data;
                /* dgc */
                SyncData.Dgc.Ctx.Bits.Dgc = pDgc[m];
                /* ddgc */
                SyncData.Dgc.Ctx.Bits.DDgc = pDDgc[m];
                /* shr */
                SyncData.Shr.Ctx.Bits.Shr = pShr[m];
                SyncData.User.Ctx.Bits.User = 0ULL;
                /* hdr? */
                if (HdrEnable > 0U) {
                    /* hdr preload */
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_HDR_PRELOAD | ((UINT32) m));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* user data attach */
            SyncData.User.Ctx.Bits.User = (UINT64) MemInfo.Ctx.Data;

            /* ae algo msg */
            for (i = 0U; i < ExposureNum; i++) {
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                /* ae */
                VarF = pAeInfo[i].ShutterTime*1000000.0f;
                AmbaImgPrint2(PRINT_FLAG_DBG, "ae info shr", i, (UINT32) VarF);
                VarF = pAeInfo[i].AgcGain*1000.0f;
                AmbaImgPrint2(PRINT_FLAG_DBG, "ae info agc", i, (UINT32) VarF);
                AmbaImgPrint2(PRINT_FLAG_DBG, "ae info dgc", i, (UINT32) pAeInfo[i].Dgain);
                VarF = pAeInfo[i].SensorDgain*1000.0f;
                AmbaImgPrint2(PRINT_FLAG_DBG, "ae info sgc", i, (UINT32) VarF);
                /* wb */
                AmbaImgPrint2(PRINT_FLAG_DBG, "wb info r", i, pWbGain[i].GainR);
                AmbaImgPrint2(PRINT_FLAG_DBG, "wb info g", i, pWbGain[i].GainG);
                AmbaImgPrint2(PRINT_FLAG_DBG, "wb info b", i, pWbGain[i].GainB);
                /* dgain */
                AmbaImgPrint2(PRINT_FLAG_DBG, "sensor dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[0]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "fe dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[1]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "be dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[2]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "be alt dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[3]);
            }
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            /* ae parameter */
            for (i = 0U; i < ExposureNum; i++) {
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                VarF = pAeInfo[i].ShutterTime*1000000.0f;
                AmbaImgPrint2(PRINT_FLAG_DBG, "shutter", VinId, (UINT32) VarF);
                VarF = pAeInfo[i].AgcGain*1000.0f;
                AmbaImgPrint2(PRINT_FLAG_DBG, "gain", VinId, (UINT32) VarF);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "agc", VinId, 10U, pAgc[i], 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "dgc", VinId, 10U, pDgc[i], 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "wgc", VinId, 10U, (UINT32) MemInfo2.Ctx.Data, 16U);
                for (j = 0U; j < AMBA_NUM_SWB_CHANNEL; j++) {
                    AmbaImgPrint2(PRINT_FLAG_DBG, " gain", j, MemInfo2.Ctx.pWgc[i].Gain[j]);
                }
                AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, (UINT32) pAeInfo[i].Dgain);
                AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, pDDgc[i]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "shr", VinId, pShr[i]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "flash", VinId, (UINT32) pAeInfo[i].Flash);
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            }
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "sls", VinId, 10U, MemInfo.Ctx.pUser->AeInfo.Sensor.Sls, 16U);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "User", VinId, 10U, (UINT32) SyncData.User.Ctx.Bits.User, 16U);
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");

            /* vin tone curve enabled for preset */
            AmbaImgMainAe_VinToneCurveFlag[VinId][ChainId] = 1U;

            /* direct request for updating ae parameter to DSP and Sensor */
            if (HdrEnable == 0U) {
                FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_DIRECT | SYNC_AIK | SYNC_SLS | SyncDgId);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            } else {
                if (SyncPreAikFlag == 0U) {
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_DIRECT | SYNC_AIK | SYNC_HDR | SYNC_SLS | SyncDgId);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                } else {
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_DIRECT | SYNC_PRE_AIK | SYNC_AIK | SYNC_HDR | SYNC_SLS | SyncDgId);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* vin tone curve settle after preset */
            AmbaImgMainAe_VinToneCurveFlag[VinId][ChainId] = AmbaImgMain_VinToneCurveUpd[VinId][ChainId];

            /* ae algo framerate update */
            FuncRetCode = AmbaImgMainAe_FramerateGet(ImageChanId, &Framerate);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaImgProc_SetFrameRate(AlgoId, Framerate, 1000U);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* algo status reset */
            AlgoStatus = 0U;

            /* last ae info */
            for (i = 0U; i < ExposureNum; i++) {
                LastAeInfo[i] = pAeInfo[i];
                LastAuxAeInfo[i] = pAuxAeInfo[i];
            }

            /* debug only */
            if (AmbaImgMainAe_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                /* timing mark print */
                AmbaImgSensorHAL_TimingMarkPrt(VinId);
            }

            /* statistics open */
            FuncRetCode = AmbaImgStatistics_Open(ImageChanId, pPort);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae statistics open", VinId, FuncRetCode);
            }

            /* active ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAe_Ack[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae task ack", VinId, FuncRetCode);
            }
        }

        /* statistics request */
        FuncRetCode = AmbaImgStatistics_Request(pPort, REQUEST_WAIT_FOREVER);
        if (FuncRetCode != OK_UL) {
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae statistics request", VinId, FuncRetCode);
        } else {
            /* task stop or statistics null? */
            if ((AmbaImgMainAe_Cmd[VinId][ChainId] == (UINT32) AE_TASK_CMD_STOP) ||
                (AmbaImgMainAe_Cmd[VinId][ChainId] == (UINT32) AE_TASK_CMD_LOCK_DOWN) ||
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                ((pCtx != NULL) && (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_SLAVE)) ||
#endif
                (pPort->Data.pCfa == NULL) ||
                (pPort->Data.pRgb == NULL)) {
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                if ((pCtx != NULL) && (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_SLAVE)) {
                    /* debug only */
                    if (AmbaImgMainAe_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                        /* timing msg print */
                        AmbaImgSensorHAL_TimingMarkPrt(VinId);
                        /* count accumulate */
                        AmbaImgMainAe_DebugCount[VinId][ChainId]++;
                        if (AmbaImgMainAe_DebugCount[VinId][ChainId] == AmbaImgMain_DebugCount) {
                            if (TimingEnFlag > 0U) {
                                /* timing msg disable */
                                FuncRetCode = AmbaImgSensorHAL_TimingMarkEn(VinId, 0U);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                                FuncRetCode = AmbaImgSystem_Debug(VinId, 0U);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            }
                            /* count reset */
                            AmbaImgMainAe_DebugCount[VinId][ChainId] = 0xFFFFFFFFU;
                        }
                    }
                }
#endif
                continue;
            }
#ifdef CONFIG_BUILD_IMGFRW_FIFO_PROT
            /* fifo check (overflow protect mechanism) */
            if (AmbaImgSensorHAL_FifoPutCheck(ImageChanId) != OK_UL) {
                /* fifo full */
                continue;
            }
#endif
#ifdef CONFIG_BUILD_IMGFRW_SMC
            /* ae smc on/off */
            switch (AmbaImgMainAe_Cmd[VinId][ChainId]) {
                case (UINT32) AE_TASK_CMD_SMC_ON:
                    /* msc cmd on */
                    if ((pInfo != NULL) &&
                        (pInfo->Op.User.ForceSsInVideo > 0U)) {
                        /* smc on */
                        AE_CONTROL_s AeCtrlCap;
                        FuncRetCode = AmbaImgProc_AEGetAEControlCap(AlgoId, &AeCtrlCap);
                        if (FuncRetCode == OK_UL) {
                            /* algo update */
                            AeCtrlCap.SlowShutter = 0;
                            FuncRetCode = AmbaImgProc_AESetAEControlCap(AlgoId, &AeCtrlCap);
                            if (FuncRetCode == OK_UL) {
                                /* force ss update */
                                pInfo->Op.User.ForceSsInVideo = 0x80000000U;
                                /* last ae info reset */
                                for (i = 0U; i < ExposureNum; i++) {
                                    FuncRetCode = AmbaWrap_memset(&(LastAeInfo[i]), 0, sizeof(AMBA_AE_INFO_s));
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                    FuncRetCode = AmbaWrap_memset(&(LastAuxAeInfo[i]), 0, sizeof(AMBA_DGAIN_INFO_s));
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                }
                            }
                        }
                    }
                    /* ae task cmd reset */
                    AmbaImgMainAe_Cmd[VinId][ChainId] = (UINT32) AE_TASK_CMD_START;
                    /* smc ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAe_SmcAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    break;
                case (UINT32) AE_TASK_CMD_SMC_OFF:
                    /* smc cmd off */
                    if ((pInfo != NULL) &&
                        (pInfo->Op.User.ForceSsInVideo > 0U)) {
                        /* smc off */
                        AE_CONTROL_s AeCtrlCap;
                        FuncRetCode = AmbaImgProc_AEGetAEControlCap(AlgoId, &AeCtrlCap);
                        if (FuncRetCode == OK_UL) {
                            /* algo update */
                            AeCtrlCap.SlowShutter = (INT32) pInfo->pAux->Ctx.Bits.MaxSlowShutterIndex;
                            FuncRetCode = AmbaImgProc_AESetAEControlCap(AlgoId, &AeCtrlCap);
                            if (FuncRetCode == OK_UL) {
                                /* forve ss update */
                                pInfo->Op.User.ForceSsInVideo = ((UINT32) pInfo->pAux->Ctx.Bits.MaxSlowShutterIndex) | 0x80000000U;
                                /* last ae info reset */
                                for (i = 0U; i < ExposureNum; i++) {
                                    FuncRetCode = AmbaWrap_memset(&(LastAeInfo[i]), 0, sizeof(AMBA_AE_INFO_s));
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                    FuncRetCode = AmbaWrap_memset(&(LastAuxAeInfo[i]), 0, sizeof(AMBA_DGAIN_INFO_s));
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                }
                            }
                        }
                    }
                    /* ae task cmd reset */
                    AmbaImgMainAe_Cmd[VinId][ChainId] = (UINT32) AE_TASK_CMD_START;
                    /* smc ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAe_SmcAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    break;
                default:
                    /* */
                    break;
            }
#endif
            /* ae lock/unlock for still */
            switch (AmbaImgMainAe_Cmd[VinId][ChainId]) {
                case (UINT32) AE_TASK_CMD_LOCK:
                    /* ae in lock */
                    CurrMode = IP_PREVIEW_MODE;
                    NextMode = IP_CAPTURE_MODE;
                    FuncRetCode = AmbaImgProc_SetMode(AlgoId, &CurrMode, &NextMode);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* aeb? */
                    if ((pCtx != NULL) && (pCtx->Snap.Ctx.Bits.AebId > 0U)) {
                        FuncRetCode = AmbaImgProc_SetCapFormat(AlgoId, IMG_CAP_AEB);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    } else {
                        FuncRetCode = AmbaImgProc_SetCapFormat(AlgoId, IMG_CAP_NORMAL);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                    /* ae cmd lock proc */
                    AmbaImgMainAe_Cmd[VinId][ChainId] = (UINT32) AE_TASK_CMD_LOCK_PROC;
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "ae lock", VinId, AlgoId);
                    break;
                case (UINT32) AE_TASK_CMD_UNLOCK:
                    /* ae in unlock */
                    CurrMode = IP_PREVIEW_MODE;
                    NextMode = IP_PREVIEW_MODE;
                    FuncRetCode = AmbaImgProc_SetMode(AlgoId, &CurrMode, &NextMode);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* ae status reset */
                    VideoStatus.Ae = AMBA_IDLE;
                    StillStatus.Ae = AMBA_IDLE;
                    FuncRetCode = AmbaImgProc_SetAEStatus(AlgoId, VideoStatus.Ae, StillStatus.Ae);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* ae cmd start */
                    AmbaImgMainAe_Cmd[VinId][ChainId] = (UINT32) AE_TASK_CMD_START;
                    /* unlock ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAe_UnlockAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "ae unlock", VinId, AlgoId);
                    break;
                default:
                    /* */
                    break;
            }

            /* aaa op get */
            FuncRetCode = AmbaImgProc_GetAAAOPInfo(AlgoId, &AaaOpInfo);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* op enable? */
            if (AaaOpInfo.AeOp > 0U) {
                /* statistics put */
                if (VrEnable == 0U) {
                    /* non-vr */
                    MemInfo.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                    FuncRetCode = AmbaImgProc_SetCFAAAAStat(AlgoId, MemInfo.Ctx.pCfa);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    MemInfo.Ctx.pCvoid = pPort->Data.pRgb[StatisticsIndex];
                    FuncRetCode = AmbaImgProc_SetPGAAAStat(AlgoId, MemInfo.Ctx.pRgb);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                } else {
                    /* vr-master */
                    AmbaImgMainAe_VrStatProc(pPort, AmbaImgMainAe_Zone2AlgoTable);
                }
                /* dgain set back */
                MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) pPort->Reg.User;
                if (MemInfo.Ctx.pUser != NULL) {
                    /* */
                    FuncRetCode = AmbaImgProc_AESetBfoCeGain(AlgoId, (UINT32) MemInfo.Ctx.pUser->AeInfo.AuxCtx[0].DgainInfo[2]);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
                /* aaa status get */
                FuncRetCode = AmbaImgProc_GetAAAStatus(AlgoId, &VideoStatus, &StillStatus);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* ae control */
                FuncRetCode = AmbaImgProc_AEControl(AlgoId, &VideoStatus, &StillStatus);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* ae status update */
                FuncRetCode = AmbaImgProc_SetAEStatus(AlgoId, VideoStatus.Ae, StillStatus.Ae);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* debug only */
                if (AmbaImgMainAe_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                    if (VrEnable == 0U) {
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                        AmbaImgPrint2(PRINT_FLAG_DBG, "[Ae Statistics]", VinId, StatisticsIndex);
                        MemInfo.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "cfa", VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                        AmbaImgPrint(PRINT_FLAG_DBG, "awb row", MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumRow);
                        AmbaImgPrint(PRINT_FLAG_DBG, "awb col", MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AWB.AwbTileNumCol);
                        MemInfo.Ctx.pCvoid = pPort->Data.pRgb[StatisticsIndex];
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "rgb", VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                        AmbaImgPrint(PRINT_FLAG_DBG, "ae row", MemInfo.Ctx.pRgb->AAA_HEADER_RGB_AE.AeTileNumRow);
                        AmbaImgPrint(PRINT_FLAG_DBG, "ae col", MemInfo.Ctx.pRgb->AAA_HEADER_RGB_AE.AeTileNumCol);
                    }
                }
            }
            /* ae status update */
            if (AlgoStatus != ((UINT32) VideoStatus.Ae)) {
                AlgoStatus = (UINT32) VideoStatus.Ae;
                AmbaImgPrintStr(PRINT_FLAG_STATUS, "\n\n");
                AmbaImgPrint(PRINT_FLAG_STATUS, "ae status", AlgoStatus);
                AmbaImgPrint(PRINT_FLAG_STATUS, " vin id", VinId);
                AmbaImgPrint(PRINT_FLAG_STATUS, " chain id", ChainId);
                AmbaImgPrint(PRINT_FLAG_STATUS, " algo id", AlgoId);
            }
            /* ae stable mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(AmbaImgMainAe_StableMutex[VinId][ChainId]), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* stable check? */
                if ((AmbaImgMainAe_StableCheck[VinId][ChainId] > 0U) &&
                    (VideoStatus.Ae == AMBA_LOCK)) {
                    /* ae stable ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAe_StableAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* stable check clear */
                    AmbaImgMainAe_StableCheck[VinId][ChainId] = 0U;
                }
                /* ae stable mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(AmbaImgMainAe_StableMutex[VinId][ChainId]));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
            /* ae lockdown */
            if (AmbaImgMainAe_Cmd[VinId][ChainId] == (UINT32) AE_TASK_CMD_LOCK_PROC) {
                /* me? */
                if (AaaOpInfo.AeOp == 0U) {
                    /* me lock */
                    VideoStatus.Ae = AMBA_LOCK;
                    StillStatus.Ae = AMBA_LOCK;
                    /* me status update */
                    FuncRetCode = AmbaImgProc_SetAEStatus(AlgoId, VideoStatus.Ae, StillStatus.Ae);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
                /* ae/me lock? */
                if (StillStatus.Ae == AMBA_LOCK) {
                    /* lockdown */
                    AmbaImgMainAe_Cmd[VinId][ChainId] = (UINT32) AE_TASK_CMD_LOCK_DOWN;
                    /* lock ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAe_LockAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "ae lockdown", VinId, AlgoId);
                } else {
                    /* lock loop */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "ae lock...", VinId, AlgoId);
                }
            }

            /* user ae buf get */
            MemInfo.Ctx.pVoid = AmbaImgSensorSync_UserBufferGet(ImageChanId);

            /* user ae info */
            pAeInfo = MemInfo.Ctx.pUser->AeInfo.Ctx;
            /* user aux ae info */
            pAuxAeInfo = MemInfo.Ctx.pUser->AeInfo.AuxCtx;
            /* user ae shr/agc/dgc */
            pShr = MemInfo.Ctx.pUser->AeInfo.Sensor.Shr;
            pAgc = MemInfo.Ctx.pUser->AeInfo.Sensor.Agc;
            pDgc = MemInfo.Ctx.pUser->AeInfo.Sensor.Dgc;
            /* dg id put */
            FuncRetCode = AmbaImgMainAe_DgcPositionGet(&SyncDgId);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            MemInfo.Ctx.pUser->AeInfo.Dsp.SyncDgId = SyncDgId;
            /* user ae ddgc */
            switch (SyncDgId) {
                case SYNC_WB_ID_FE:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.FeDgc;
                    break;
                case SYNC_WB_ID_BE:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
                    break;
                case SYNC_WB_ID_BE_ALT:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[1];
                    break;
                default:
                    pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
                    break;
            }

            /* user wb info */
            pWbGain = MemInfo.Ctx.pUser->WbInfo.Ctx;
            /* wb id put */
            MemInfo.Ctx.pUser->WbInfo.Dsp.WbId = WbId;
            /* user sensor wgc */
            pSensorWb = MemInfo.Ctx.pUser->WbInfo.Sensor.Wgc;

            /* ae/awb info get */
            for (i = 0U; i < ExposureNum; i++) {
                /* ae info get */
                FuncRetCode = AmbaImgProc_AEGetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(pAeInfo[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* hdr? */
#if 0
                if (HdrEnable > 0U) {
                    /* awb info get */
                    FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_VIDEO, &(pWbGain[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                } else {
                    /* awb info get for carry */
                    FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_VIDEO, &(pWbGain[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
#else
                /* awb info get */
                FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_VIDEO, &(pWbGain[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
#endif
            }

            /* hdr? */
            if (HdrEnable > 0U) {
                /* hdr control */
                if (AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode != (UINT8) VIDEO_OP_RAWENC) {
                    /* normal mode */
                    Amba_Adj_VideoHdr_Control(AlgoId, pAeInfo, pWbGain);
                } else {
                    /* raw enc mode */
#ifdef CONFIG_BUILD_IMGFRW_RAW_ENC
                    AMBA_IMGPROC_OFFLINE_AAA_INFO_s *pAaaInfo = &(MemInfo.Ctx.pUser->AaaInfo);
                    FuncRetCode = AmbaImgProc_GetOfflineAAAInfo(AlgoId, pAaaInfo);
                    if (FuncRetCode == OK) {
                        /* shift ae gain */
                        FuncRetCode = AmbaImgProc_AdjHdrSetShiftAeGain(AlgoId, pAaaInfo->ShiftAeGain);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* offline hdr control */
                        FuncRetCode = AmbaImgProc_OfflineHdrVideoControl(AlgoId, pAaaInfo->BlendRatio, pAaaInfo->MaxBlendRatio, pAaaInfo->MinBlendRatio);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
#endif
                }
            }

            /* gain build for algo */
            FuncRetCode = AmbaImgMainAe_UserGainPut(ImageChanId, MemInfo.Ctx.pVoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* agc/dgc/wgc transfer */
            for (i = 0U; i < ExposureNum; i++) {
                VarF = pAeInfo[i].AgcGain * 4096.0f;
                Factor[i] = (UINT32) VarF;
            }
            FuncRetCode = AmbaImgSensorDrv_GainConvert(ImageChanId, Factor, pAgc, pDgc, pSensorWb);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* shr transfer */
            for (i = 0U; i < ExposureNum; i++) {
                VarF = pAeInfo[i].ShutterTime * 1024.0f * 1024.0f;
                Factor[i] = (UINT32) VarF;
            }
            FuncRetCode = AmbaImgSensorDrv_ShutterConvert(ImageChanId, Factor, pShr);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* user data put */
            FuncRetCode = AmbaImgMainAe_UserDataPut(ImageChanId, MemInfo.Ctx.pVoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* hdr pipe flag */
            HdrPipeUpdate = 0U;
            if (HdrEnable > 0U) {
                if ((MemInfo.Ctx.pUser->HdrInfo.Op.Data & AMBA_IMG_HDR_INFO_FLAG_MASK) > 0ULL) {
                    HdrPipeUpdate = 1U;
                }
            }

            /* hdr pipe wb flag */
            HdrWbUpdate = 0U;
            if (HdrEnable > 0U) {
                for (i = 0U; i < ExposureNum; i++) {
                    /* compare */
                    if ((pWbGain[i].GainR != LastWbGain[i].GainR) ||
                        (pWbGain[i].GainG != LastWbGain[i].GainG) ||
                        (pWbGain[i].GainB != LastWbGain[i].GainB)) {
                        HdrWbUpdate = 1U;
                        break;
                    }
                }
                /* last wb get */
                for (i = 0U; i < ExposureNum; i++) {
                    LastWbGain[i] = pWbGain[i];
                }
            }

            /* ae info flag */
            AeUpdate = 0U;
            for (i = 0U; i < ExposureNum; i++) {
                /* agc gain */
                VarF = pAeInfo[i].AgcGain * 4096.0f;
                AgcGainUL = (UINT32) VarF;
                VarF = LastAeInfo[i].AgcGain * 4096.0f;
                LastAgcGainUL = (UINT32) VarF;
                /* sls gain */
                SlsGainUL = pAeInfo[i].Flash;
                LastSlsGainUL = LastAeInfo[i].Flash;
                /* shutter time */
                VarF = pAeInfo[i].ShutterTime * 1024.0f * 1024.0f;
                ShutterTimeUL = (UINT32) VarF;
                VarF = LastAeInfo[i].ShutterTime * 1024.0f * 1024.0f;
                LastShutterTimeUL = (UINT32) VarF;
                /* compare */
                if ((AgcGainUL != LastAgcGainUL) ||
                    (SlsGainUL != LastSlsGainUL) ||
                    (ShutterTimeUL != LastShutterTimeUL) ||
                    (pAuxAeInfo[i].DgainInfo[0] != LastAuxAeInfo[i].DgainInfo[0]) ||
                    (pAuxAeInfo[i].DgainInfo[1] != LastAuxAeInfo[i].DgainInfo[1]) ||
                    (pAuxAeInfo[i].DgainInfo[2] != LastAuxAeInfo[i].DgainInfo[2]) ||
                    (pAuxAeInfo[i].DgainInfo[3] != LastAuxAeInfo[i].DgainInfo[3])) {
                    AeUpdate = 1U;
                    break;
                }
            }

            /* debug only */
            if (AmbaImgMainAe_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "ae pipe flag", VinId, 10U, (UINT32) MemInfo.Ctx.pUser->HdrInfo.Op.Data, 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "ae pipe mask", VinId, 10U, (UINT32) AMBA_IMG_HDR_INFO_FLAG_MASK, 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "ae pipe flag (mask)", VinId, 10U, (UINT32) (MemInfo.Ctx.pUser->HdrInfo.Op.Data & AMBA_IMG_HDR_INFO_FLAG_MASK), 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "ae update", VinId, 10U, AeUpdate | (HdrWbUpdate << 1U) | (HdrPipeUpdate << 2U), 16U);
            }

            if ((AeUpdate > 0U) ||
                (HdrWbUpdate > 0U) ||
                (HdrPipeUpdate > 0U)) {
                /* agc/dgc/wgc, shr get */
                for (m = ((INT32) ExposureNum) - 1; m >= 0; m--) {
                    /* agc */
                    SyncData.Agc.Ctx.Bits.Agc = pAgc[m];
                    /* wgc */
                    MemInfo2.Ctx.pWgc = &(pSensorWb[m]);
                    SyncData.Wgc.Ctx.Bits.Wgc = (UINT64) MemInfo2.Ctx.Data;
                    /* dgc */
                    SyncData.Dgc.Ctx.Bits.Dgc = pDgc[m];
                    /* ddgc */
                    SyncData.Dgc.Ctx.Bits.DDgc = pDDgc[m];
                    /* shr */
                    SyncData.Shr.Ctx.Bits.Shr = pShr[m];
                    SyncData.User.Ctx.Bits.User = 0ULL;
                    /* hdr? */
                    if (HdrEnable > 0U) {
                        /* hdr preload */
                        FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_HDR_PRELOAD | ((UINT32) m));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                }

                /* user */
                SyncData.User.Ctx.Bits.User = (UINT64) MemInfo.Ctx.Data;

                /* sync mode */
                if (AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode != (UINT8) VIDEO_OP_RAWENC) {
                    /* normal mode */
                    SyncMode = SYNC_FIFO | SYNC_SLS;
                } else {
                    /* raw enc mode */
                    SyncMode = SYNC_DIRECT;
                }

                /* sync request */
                if (HdrEnable == 0U) {
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SyncMode | SYNC_AIK | SyncDgId);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                } else {
                    if (SyncPreAikFlag == 0U) {
                        FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SyncMode | SYNC_AIK | SYNC_HDR | SyncDgId);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    } else {
                        FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SyncMode | SYNC_PRE_AIK | SYNC_AIK | SYNC_HDR | SyncDgId);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                }

                /* ae algo framerate update */
                FuncRetCode = AmbaImgMainAe_FramerateGet(ImageChanId, &Framerate);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaImgProc_SetFrameRate(AlgoId, Framerate, 1000U);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* last ae info */
            for (i = 0U; i < ExposureNum; i++) {
                LastAeInfo[i] = pAeInfo[i];
                LastAuxAeInfo[i] = pAuxAeInfo[i];
            }

            /* debug only */
            if (AmbaImgMainAe_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                /* ae algo msg */
                for (i = 0U; i < ExposureNum; i++) {
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    /* ae */
                    VarF = pAeInfo[i].ShutterTime*1000000.0f;
                    AmbaImgPrint2(PRINT_FLAG_DBG, "ae info shr", i, (UINT32) VarF);
                    VarF = pAeInfo[i].AgcGain*1000.0f;
                    AmbaImgPrint2(PRINT_FLAG_DBG, "ae info agc", i, (UINT32) VarF);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "ae info dgc", i, (UINT32) pAeInfo[i].Dgain);
                    VarF = pAeInfo[i].SensorDgain*1000.0f;
                    AmbaImgPrint2(PRINT_FLAG_DBG, "ae info sgc", i, (UINT32) VarF);
                    /* wb */
                    AmbaImgPrint2(PRINT_FLAG_DBG, "wb info r", i, pWbGain[i].GainR);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "wb info g", i, pWbGain[i].GainG);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "wb info b", i, pWbGain[i].GainB);
                    /* dgain */
                    AmbaImgPrint2(PRINT_FLAG_DBG, "sensor dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[0]);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "fe dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[1]);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "be dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[2]);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "be alt dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[3]);
                }
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                /* ae parameter */
                for (i = 0U; i < ExposureNum; i++) {
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    VarF = pAeInfo[i].ShutterTime*1000000.0f;
                    AmbaImgPrint2(PRINT_FLAG_DBG, "shutter", VinId, (UINT32) VarF);
                    VarF = pAeInfo[i].AgcGain*1000.0f;
                    AmbaImgPrint2(PRINT_FLAG_DBG, "gain", VinId, (UINT32) VarF);
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "agc", VinId, 10U, pAgc[i], 16U);
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "dgc", VinId, 10U, pDgc[i], 16U);
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "wgc", VinId, 10U, (UINT32) MemInfo2.Ctx.Data, 16U);
                    for (j = 0U; j < AMBA_NUM_SWB_CHANNEL; j++) {
                        AmbaImgPrint2(PRINT_FLAG_DBG, " gain", j, MemInfo2.Ctx.pWgc[i].Gain[j]);
                    }
                    AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, (UINT32) pAeInfo[i].Dgain);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, pDDgc[i]);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "shr", VinId, pShr[i]);
                    AmbaImgPrint2(PRINT_FLAG_DBG, "flash", VinId, (UINT32) pAeInfo[i].Flash);
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                }
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "sls", VinId, 10U, MemInfo.Ctx.pUser->AeInfo.Sensor.Sls, 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "User", VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                /* timing debug print */
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                AmbaImgSensorHAL_TimingMarkPrt(VinId);
                /* count accumulate */
                AmbaImgMainAe_DebugCount[VinId][ChainId]++;
                if (AmbaImgMainAe_DebugCount[VinId][ChainId] == AmbaImgMain_DebugCount) {
                    /* tming debug disable */
                    if (TimingEnFlag > 0U) {
                        FuncRetCode = AmbaImgSensorHAL_TimingMarkEn(VinId, 0U);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        FuncRetCode = AmbaImgSystem_Debug(VinId, 0U);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                    /* count reset */
                    AmbaImgMainAe_DebugCount[VinId][ChainId] = 0xFFFFFFFFU;
                }
            }
        }
    }
}

/**
 *  Amba image main ae aaa algorithm id get
 *  @param[in] ImageChanId image channel id
 *  @return algorithm id
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAe_AaaAlgoIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 i;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 mVinId = 0U;
    UINT32 mChainId = 0U;

    UINT32 mAlgoId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    /* image channel id */
    ImageChanId2.Ctx.Data = pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->Id.Ctx.Data;
    /* vr map get */
    pVrMap = &(pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->VrMap);
    /* vr? */
    if (pVrMap->Id.Ctx.Bits.VrId > 0U) {
        /* vr id */
        VrId = pVrMap->Id.Ctx.Bits.VrId;
        /* vr alt id */
        VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;
        /* vr master? */
        if (((VrId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.VinId)) > 0U) &&
            ((VrAltId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.ChainId)) > 0U)) {
            /* algo id (vr) */
            mAlgoId = ImageChanId2.Ctx.Bits.AlgoId;
        } else {
            /* slave */
            for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                if ((VrId & (((UINT32) 1U) << i)) > 0U) {
                    /* master vin id */
                    mVinId = i;
                    break;
                }
            }
            for (i = 0U; i < AMBA_IMG_NUM_VIN_ALGO; i++) {
                if ((VrAltId & (((UINT32) 1U) << i)) > 0U) {
                    /* master chain id */
                    mChainId = i;
                    break;
                }
            }
            /* algo id (vr) */
            mAlgoId = pAmbaImgMainChannel[mVinId][mChainId].pCtx->Id.Ctx.Bits.AlgoId;
        }
    } else {
        /* algo id (non-vr) */
        mAlgoId = ImageChanId2.Ctx.Bits.AlgoId;
    }

    return mAlgoId;
}

/**
 *  Amba image main ae snap user data put
 *  @param[in] ImageChanId image channel id
 *  @param[in] AlgoId algorithm id
 *  @param[in] AebIndex aeb index
 *  @param[out] pShr pointer to the shutter data pointer
 *  @param[out] pAgc pointer to the analog gain data pointer
 *  @param[out] pDgc pointer to the digital gain data pointer
 *  @param[out] pDDgc pointer to the DSP digital gain data pointer
 *  @param[out] pSensorWb pointer to the sensor white balance gain data pointer
 *  @param[in] User user data address
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAe_SnapUserDataPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, UINT32 AebIndex, UINT32 **pShr, UINT32 **pAgc, UINT32 **pDgc, UINT32 **pDDgc, AMBA_IMG_SENSOR_WB_s **pSensorWb, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 HdrEnable;
    UINT32 ExposureNum;

    AMBA_AE_INFO_s    *pAeInfo;
    AMBA_IK_WB_GAIN_s *pWbGain;

    AMBA_AE_INFO_s StillAeInfo[AMBA_IMG_MAX_EXPOSURE_NUM][MAX_AEB_NUM];
    AMBA_IK_WB_GAIN_s StillWbInfo[AMBA_IMG_MAX_EXPOSURE_NUM][MAX_AEB_NUM];

    UINT32 SyncDgId = AMBA_IMG_MAIN_AE_SYNC_DGCID;

    UINT32 WbPosition = 0U;
    UINT32 WbId = (UINT32) WB_ID_FE;

     FLOAT VarF;
    UINT32 Factor[AMBA_IMG_MAX_EXPOSURE_NUM] = {0};

    UINT32 VinId;
    UINT32 ChainId;

    AE_SNAP_MEM_INFO_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* clr */
    FuncRetCode = AmbaWrap_memset(StillAeInfo, 0, sizeof(StillAeInfo));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(StillWbInfo, 0, sizeof(StillWbInfo));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* hdr info get */
    HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
    ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

    /* user ae buff get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) User;

    /* user ae info */
    pAeInfo = MemInfo.Ctx.pUser->AeInfo.Ctx;
    /* user ae shr/agc/dgc */
    *pShr = MemInfo.Ctx.pUser->AeInfo.Sensor.Shr;
    *pAgc = MemInfo.Ctx.pUser->AeInfo.Sensor.Agc;
    *pDgc = MemInfo.Ctx.pUser->AeInfo.Sensor.Dgc;

    /* dg id put */
    FuncRetCode = AmbaImgMainAe_DgcPositionGet(&SyncDgId);
    if (FuncRetCode != OK_UL) {
        /* */
    }
    MemInfo.Ctx.pUser->AeInfo.Dsp.SyncDgId = SyncDgId;
    /* user ae ddgc */
    switch (SyncDgId) {
        case SYNC_WB_ID_FE:
            *pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.FeDgc;
            break;
        case SYNC_WB_ID_BE:
            *pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
            break;
        case SYNC_WB_ID_BE_ALT:
            *pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[1];
            break;
        default:
            *pDDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
            break;
    }
    /* dg id debug */
    AmbaImgPrintEx2(PRINT_FLAG_DBG, "dg id", VinId, 10U, SyncDgId, 16U);

    /* wb position get */
    FuncRetCode = AmbaImgProc_AWBGetWBPosition(AlgoId, &WbPosition);
    if (FuncRetCode == OK) {
        switch (WbPosition) {
            case 16U:
                WbId = (UINT32) WB_ID_SENSOR;
                break;
            case 32U:
                WbId = (UINT32) WB_ID_FE;
                break;
            case 48U:
                WbId = (UINT32) WB_ID_BE;
                break;
            case 64U:
                WbId = (UINT32) WB_ID_BE_ALT;
                break;
            default:
                /* */
                break;
        }
    }
    /* wb id put */
    MemInfo.Ctx.pUser->WbInfo.Dsp.WbId = WbId;
    /* wb id debug */
    AmbaImgPrint2(PRINT_FLAG_DBG, "wb position", VinId, WbPosition);
    AmbaImgPrint2(PRINT_FLAG_DBG, "wb id", VinId, WbId);

    /* user wb info */
    pWbGain = MemInfo.Ctx.pUser->WbInfo.Ctx;
    /* user sensor wgc */
    *pSensorWb = MemInfo.Ctx.pUser->WbInfo.Sensor.Wgc;

    /* ae/awb info get */
    for (i = 0U; i < ExposureNum; i++) {
        /* ae info get */
        FuncRetCode = AmbaImgProc_AEGetExpInfo(AlgoId, i, IP_MODE_STILL, StillAeInfo[i]);
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* hdr? */
#if 0
        if (HdrEnable > 0U) {
            /* awb info get */
            FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_STILL, StillWbInfo[i]);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        } else {
            /* awb info get for carry */
            FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_STILL, StillWbInfo[i]);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
#else
        /* awb info get */
        FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_STILL, StillWbInfo[i]);
        if (FuncRetCode != OK_UL) {
            /* */
        }
#endif
    }

    /* hdr? */
    if (HdrEnable > 0U) {
        /* hdr control */
        Amba_Adj_StillHdr_Control(AlgoId, StillAeInfo, StillWbInfo);
    }

    /* ae/awb info get */
    for (i = 0U; i < ExposureNum; i++) {
        pAeInfo[i] = StillAeInfo[i][AebIndex];
        pWbGain[i] = StillWbInfo[i][AebIndex];
    }

    /* gain build for algo */
    FuncRetCode = AmbaImgMainAe_UserSnapGainPut(ImageChanId, AlgoId, AebIndex, MemInfo.Ctx.pVoid);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* debug msg */
    for (i = 0U; i < ExposureNum; i++) {
        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
        /* ae */
        VarF = pAeInfo[i].ShutterTime * 1000000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "ae info shr", i, (UINT32) VarF);
        VarF = pAeInfo[i].AgcGain * 1000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "ae info agc", i, (UINT32) VarF);
        AmbaImgPrint2(PRINT_FLAG_DBG, "ae info dgc", i, (UINT32) pAeInfo[i].Dgain);
        VarF = pAeInfo[i].SensorDgain * 1000.0f;
        AmbaImgPrint2(PRINT_FLAG_DBG, "ae info sgc", i, (UINT32) VarF);
        /* wb */
        AmbaImgPrint2(PRINT_FLAG_DBG, "wb info r", i, pWbGain[i].GainR);
        AmbaImgPrint2(PRINT_FLAG_DBG, "wb info g", i, pWbGain[i].GainG);
        AmbaImgPrint2(PRINT_FLAG_DBG, "wb info b", i, pWbGain[i].GainB);
        /* dgain */
        AmbaImgPrint2(PRINT_FLAG_DBG, "sensor dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[0]);
        AmbaImgPrint2(PRINT_FLAG_DBG, "fe dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[1]);
        AmbaImgPrint2(PRINT_FLAG_DBG, "be dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[2]);
        AmbaImgPrint2(PRINT_FLAG_DBG, "be alt dgain", i, MemInfo.Ctx.pUser->AeInfo.AuxCtx[i].DgainInfo[3]);
    }
    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");

    /* agc/dgc/wgc transfer */
    for (i = 0U; i < ExposureNum; i++) {
        VarF = pAeInfo[i].AgcGain * 4096.0f;
        Factor[i] = (UINT32) VarF;
    }
    FuncRetCode = AmbaImgSensorDrv_GainConvert(ImageChanId, Factor, *pAgc, *pDgc, *pSensorWb);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* shr transfer */
    for (i = 0U; i < ExposureNum; i++) {
        VarF = pAeInfo[i].ShutterTime * 1024.0f * 1024.0f;
        Factor[i] = (UINT32) VarF;
    }
    FuncRetCode = AmbaImgSensorDrv_ShutterConvert(ImageChanId, Factor, *pShr);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* user data put */
    FuncRetCode = AmbaImgMainAe_UserSnapDataPut(ImageChanId, AlgoId, MemInfo.Ctx.pVoid);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae snap
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] SnapCount snap count
 *  @param[out] pIsHiso pointer to the high iso enable flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Snap(UINT32 VinId, UINT32 ChainId, UINT32 SnapCount, UINT32 *pIsHiso)
{
    static AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_SnapPort[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM];
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j, k;

     INT32 m;

    UINT32 HdrEnable;
    UINT32 ExposureNum;

    AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    UINT32 AlgoId;
    UINT32 AebId;

    AMBA_IMG_SENSOR_SYNC_DATA_s SyncData;

    AE_SNAP_MEM_INFO_s MemInfo;
    AE_SNAP_MEM_INFO_s MemInfo2;

    AMBA_IMG_SENSOR_WB_s *pSensorWb;

    UINT32 *pAgc;
    UINT32 *pDgc;
    UINT32 *pDDgc;
    UINT32 *pShr;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    UINT32 SyncPreAikFlag = 0U;

    FLOAT VarF;

    MemInfo.Ctx.Data = 0U;
    MemInfo2.Ctx.Data = 0U;

    if ((pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* hdr id reset */
            pCtx->Id.Ctx.Bits.HdrId = 0U;

            /* hdr info get */
            HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
            ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "still ae", VinId, ChainId);
            AmbaImgPrint2(PRINT_FLAG_DBG, "hdr enable", VinId, HdrEnable);
            AmbaImgPrint2(PRINT_FLAG_DBG, "exposure num", VinId, ExposureNum);

            /* hdr id put */
            if (HdrEnable > 0U) {
                UINT32 HdrId = 0U;
                for (i = 0U; i < ExposureNum; i++) {
                    HdrId |= (((UINT32) 1U) << i);
                }
                pCtx->Id.Ctx.Bits.HdrId = (UINT8) (HdrId & 0xFFU);
            }

            /* image channel get */
            ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
            /* zone id get */
            ImageChanId.Ctx.Bits.ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId);

            /* algo id get */
            AlgoId = AmbaImgMainAe_AaaAlgoIdGet(ImageChanId);

            /* sync pre aik flag */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                if (pInfo->Op.Ctrl.PreAikCmd > 0U) {
                    SyncPreAikFlag = 1U;
                }
            }

            /* aik proc unhook */
            pAmbaImgMainChannel[VinId][ChainId].AikProc = AmbaImgMainAe_AikSnapProc;
            /* sls snao cb hook */
            pAmbaImgMainChannel[VinId][ChainId].SlsCb = AmbaImgMainAe_SlsSnapCb;

            /* user data buffer register */
            MemInfo.Ctx.pUser = AmbaImgMainAe_SnapUserData[VinId][ChainId];
            FuncRetCode = AmbaImgSensorSync_UserBuffer(ImageChanId, (UINT32) sizeof(AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s), AMBA_IMG_MAIN_AE_USER_SNAP_DATA_NUM, MemInfo.Ctx.pUint8);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: ae user still data register", VinId, FuncRetCode);
            }

            /* user ae buf get */
            MemInfo.Ctx.pVoid = AmbaImgSensorSync_UserBufferGet(ImageChanId);
            /* user ae data put */
            FuncRetCode = AmbaImgMainAe_SnapUserDataPut(ImageChanId, AlgoId, 0U, &pShr, &pAgc, &pDgc, &pDDgc, &pSensorWb, (UINT64) MemInfo.Ctx.Data);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* user ae buff put */
            AmbaImgMainAe_SnapUser[VinId][ChainId][0] = (UINT64) MemInfo.Ctx.Data;
            /* user ae count reset */
            AmbaImgMainAe_SnapAebCount[VinId][ChainId] = 0U;

            /* agc/dgc, shr attach and hdr preload */
            for (m = ((INT32) ExposureNum) - 1; m >= 0; m--) {
                /* agc */
                SyncData.Agc.Ctx.Bits.Agc = pAgc[m];
                /* wgc */
                MemInfo2.Ctx.pWgc = &(pSensorWb[m]);
                SyncData.Wgc.Ctx.Bits.Wgc = (UINT64) MemInfo2.Ctx.Data;
                /* dgc */
                SyncData.Dgc.Ctx.Bits.Dgc = pDgc[m];
                /* ddgc */
                SyncData.Dgc.Ctx.Bits.DDgc = pDDgc[m];
                /* shr */
                SyncData.Shr.Ctx.Bits.Shr = pShr[m];
                SyncData.User.Ctx.Bits.User = 0ULL;
                /* hdr? */
                if (HdrEnable > 0U) {
                    /* hdr preload */
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_HDR_PRELOAD | ((UINT32) m));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* user data attach */
            SyncData.User.Ctx.Bits.User = (UINT64) MemInfo.Ctx.Data;
            /* vin tone curve enabled for preset */
            AmbaImgMainAe_VinToneCurveFlag[VinId][ChainId] = 1U;

            /* snap in */
            if (HdrEnable == 0U) {
                FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_SNAP | SYNC_SLS);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            } else {
                if (SyncPreAikFlag == 0U) {
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_SNAP | SYNC_AIK | SYNC_HDR | SYNC_SLS);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                } else {
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_SNAP | SYNC_PRE_AIK | SYNC_AIK | SYNC_HDR | SYNC_SLS);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* vin tone curve settle after preset */
            AmbaImgMainAe_VinToneCurveFlag[VinId][ChainId] = AmbaImgMain_VinToneCurveUpd[VinId][ChainId];

            /* debug msg */
            for (i = 0U; i < ExposureNum; i++) {
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                VarF = MemInfo.Ctx.pUser->AeInfo.Ctx[i].ShutterTime*1000000.0f;
                AmbaImgPrint2(PRINT_FLAG_DBG, "shutter", VinId, (VarF > 0.0f) ? (UINT32) VarF : 0U);
                VarF = MemInfo.Ctx.pUser->AeInfo.Ctx[i].AgcGain*1000.0f;
                AmbaImgPrint2(PRINT_FLAG_DBG, "gain", VinId, (VarF > 0.0f) ? (UINT32) VarF : 0U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "agc", VinId, 10U, pAgc[i], 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "dgc", VinId, 10U, pDgc[i], 16U);
                AmbaImgPrintEx2(PRINT_FLAG_DBG, "wgc", VinId, 10U, (UINT32) MemInfo2.Ctx.Data, 16U);
                for (j = 0U; j < AMBA_NUM_SWB_CHANNEL; j++) {
                    AmbaImgPrint2(PRINT_FLAG_DBG, " gain", j, MemInfo2.Ctx.pWgc[i].Gain[j]);
                }
                AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, (MemInfo.Ctx.pUser->AeInfo.Ctx[i].Dgain > 0) ? (UINT32) MemInfo.Ctx.pUser->AeInfo.Ctx[i].Dgain : 0U);
                AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, pDDgc[i]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "shr", VinId, pShr[i]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "flash", VinId, (UINT32) MemInfo.Ctx.pUser->AeInfo.Ctx[i].Flash);
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            }
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "sls", VinId, 10U, MemInfo.Ctx.pUser->AeInfo.Sensor.Sls, 16U);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "User", VinId, 10U, (UINT32) SyncData.User.Ctx.Bits.User, 16U);
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");

            /* aeb? */
            if (pCtx->Snap.Ctx.Bits.AebId > 0U) {
                AMBA_IMG_SENSOR_SYNC_DATA_s SyncAebData;

                AMBA_IMG_SENSOR_WB_s *pAebSensorWb;

                UINT32 *pAebAgc;
                UINT32 *pAebDgc;
                UINT32 *pAebDDgc;
                UINT32 *pAebShr;

                /* aeb id get */
                AebId = pCtx->Snap.Ctx.Bits2.AebCnt;
                /* aeb port hook */
                for (i = 1U; i < AebId; i++) {
                    /* user ae buf get */
                    MemInfo.Ctx.pVoid = AmbaImgSensorSync_UserBufferGet(ImageChanId);
                    /* user ae data put */
                    FuncRetCode = AmbaImgMainAe_SnapUserDataPut(ImageChanId, AlgoId, i, &pAebShr, &pAebAgc, &pAebDgc, &pAebDDgc, &pAebSensorWb, (UINT64) MemInfo.Ctx.Data);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* user ae buff put */
                    AmbaImgMainAe_SnapUser[VinId][ChainId][i] = (UINT64) MemInfo.Ctx.Data;

                    /* agc/dgc, shr attach and hdr preload */
                    for (m = ((INT32) ExposureNum) - 1; m >= 0; m--) {
                        /* agc */
                        SyncAebData.Agc.Ctx.Bits.Agc = pAebAgc[m];
                        /* wgc */
                        MemInfo2.Ctx.pWgc = &(pAebSensorWb[m]);
                        SyncAebData.Wgc.Ctx.Bits.Wgc = (UINT64) MemInfo2.Ctx.Data;
                        /* dgc */
                        SyncAebData.Dgc.Ctx.Bits.Dgc = pAebDgc[m];
                        /* ddgc */
                        SyncAebData.Dgc.Ctx.Bits.DDgc = pAebDDgc[m];
                        /* shr */
                        SyncAebData.Shr.Ctx.Bits.Shr = pAebShr[m];
                        SyncAebData.User.Ctx.Bits.User = 0ULL;
                        /* hdr? */
                        if (HdrEnable > 0U) {
                            /* hdr preload */
                            FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncAebData, SYNC_HDR_PRELOAD | ((UINT32) m));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                    }

                    /* user data attach */
                    SyncAebData.User.Ctx.Bits.User = (UINT64) MemInfo.Ctx.Data;

                    /* snap sequence */
                    if (HdrEnable == 0U) {
                        FuncRetCode = AmbaImgSensorSync_SnapSequence(ImageChanId, &(AmbaImgMainAe_SnapPort[VinId][ChainId][i]), &SyncAebData, SYNC_SNAP | SYNC_SLS | i);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    } else {
                        if (SyncPreAikFlag == 0U) {
                            FuncRetCode = AmbaImgSensorSync_SnapSequence(ImageChanId, &(AmbaImgMainAe_SnapPort[VinId][ChainId][i]), &SyncAebData, SYNC_SNAP | SYNC_AIK | SYNC_HDR | SYNC_SLS | i);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        } else {
                            FuncRetCode = AmbaImgSensorSync_SnapSequence(ImageChanId, &(AmbaImgMainAe_SnapPort[VinId][ChainId][i]), &SyncAebData, SYNC_SNAP | SYNC_PRE_AIK | SYNC_AIK | SYNC_HDR | SYNC_SLS | i);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                    }

                    /* debug msg */
                    for (j = 0U; j < ExposureNum; j++) {
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                        VarF = MemInfo.Ctx.pUser->AeInfo.Ctx[j].ShutterTime*1000000.0f;
                        AmbaImgPrint2(PRINT_FLAG_DBG, "shutter", VinId, (VarF > 0.0f) ? (UINT32) VarF : 0U);
                        VarF = MemInfo.Ctx.pUser->AeInfo.Ctx[j].AgcGain*1000.0f;
                        AmbaImgPrint2(PRINT_FLAG_DBG, "gain", VinId, (VarF > 0.0f) ? (UINT32) VarF : 0U);
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "agc", VinId, 10U, pAebAgc[j], 16U);
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "dgc", VinId, 10U, pAebDgc[j], 16U);
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "wgc", VinId, 10U, (UINT32) MemInfo2.Ctx.Data, 16U);
                        for (k = 0U; k < AMBA_NUM_SWB_CHANNEL; k++) {
                            AmbaImgPrint2(PRINT_FLAG_DBG, " gain", k, MemInfo2.Ctx.pWgc[j].Gain[k]);
                        }
                        AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, (MemInfo.Ctx.pUser->AeInfo.Ctx[j].Dgain > 0) ? (UINT32) MemInfo.Ctx.pUser->AeInfo.Ctx[j].Dgain : 0U);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "ddgc", VinId, pAebDDgc[j]);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "shr", VinId, pAebShr[j]);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "flash", VinId, (UINT32) MemInfo.Ctx.pUser->AeInfo.Ctx[j].Flash);
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    }
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "sls", VinId, 10U, MemInfo.Ctx.pUser->AeInfo.Sensor.Sls, 16U);
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "User", VinId, 10U, (UINT32) SyncAebData.User.Ctx.Bits.User, 16U);
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                }
            }

            /* snap request */
            if (HdrEnable == 0U) {
                FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_SNAP | SnapCount);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            } else {
                if (SyncPreAikFlag == 0U) {
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_SNAP | SYNC_AIK | SYNC_HDR | SnapCount);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                } else {
                    FuncRetCode = AmbaImgSensorSync_Request(ImageChanId, &SyncData, SYNC_SNAP | SYNC_PRE_AIK | SYNC_AIK | SYNC_HDR | SnapCount);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* sanp exif info */
            {
                COMPUTE_EXIF_PARAMS_s ComputeExif;

                ComputeExif.ChannelNo = AlgoId;
                ComputeExif.Mode = IMG_EXIF_STILL;

                /* aeb id get */
                if (pCtx->Snap.Ctx.Bits.AebId > 0U) {
                    AebId = pCtx->Snap.Ctx.Bits2.AebCnt;
                } else {
                    AebId = 1U;
                }

                /* exif get */
                for (i = 0U; i < AebId; i++) {
                    ComputeExif.AeIdx = (UINT8) i;
                    FuncRetCode = AmbaWrap_memset(&(AmbaImgMainAe_SnapExif[VinId][ChainId][i]), 0, sizeof(EXIF_INFO_s));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    Amba_Exif_Compute_AAA_Exif(&ComputeExif);
                    (void) Amba_Exif_Get_Exif_Info((UINT8) i, &(AmbaImgMainAe_SnapExif[VinId][ChainId][i]));
                }

                /* exif aeb count clr */
                AmbaImgMainAe_SnapExifAebCount[VinId][ChainId] = 0U;
            }

            /* aeb id get */
            if (pCtx->Snap.Ctx.Bits.AebId > 0U) {
                AebId = pCtx->Snap.Ctx.Bits2.AebCnt;
            } else {
                AebId = 1U;
            }

            for (i = 0U; i < AebId; i++) {
                /* iso mode get */
                MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) AmbaImgMainAe_SnapUser[VinId][ChainId][i];
                FuncRetCode = AmbaImgProc_ADJChkStillIdx(AlgoId, &(MemInfo.Ctx.pUser->AeInfo.Ctx[0]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* iso mode put, app layer need prepare the array for IsHiso, if aeb is enabled */
                if (MemInfo.Ctx.pUser->AeInfo.Ctx[0].Mode == (UINT16) IP_MODE_LISO_STILL) {
                    /* liso (TBD) */
                    pIsHiso[i] = 0U;
                } else {
                    /* hiso (TBD) */
                    pIsHiso[i] = 1U;
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
/**
 *  Amba image main channel id get
 *  @param[in] ImageChanId image channel id
 *  @return image channel id
 *  @note this function is intended for internal use only
 */
static UINT64 AmbaImgMainAe_ImageChanIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 i;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 mVinId = 0U;
    UINT32 mChainId = 0U;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s mImageChanId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    /* image channel id */
    ImageChanId2.Ctx.Data = pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->Id.Ctx.Data;
    /* vr map get */
    pVrMap = &(pAmbaImgMainChannel[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pCtx->VrMap);
    /* vr? */
    if (pVrMap->Id.Ctx.Bits.VrId > 0U) {
        /* vr id */
        VrId = pVrMap->Id.Ctx.Bits.VrId;
        /* vr alt id */
        VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;
        /* vr master? */
        if (((VrId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.VinId)) > 0U) &&
            ((VrAltId & (((UINT32) 1U) << ImageChanId2.Ctx.Bits.ChainId)) > 0U)) {
            /* image channel id (vr) */
            mImageChanId.Ctx.Data = ImageChanId2.Ctx.Data;
        } else {
            /* slave */
            for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                if ((VrId & (((UINT32) 1U) << i)) > 0U) {
                    /* master vin id */
                    mVinId = i;
                    break;
                }
            }
            for (i = 0U; i < AMBA_IMG_NUM_VIN_ALGO; i++) {
                if ((VrAltId & (((UINT32) 1U) << i)) > 0U) {
                    /* master chain id */
                    mChainId = i;
                    break;
                }
            }
            /* algo id (vr) */
            mImageChanId.Ctx.Data = pAmbaImgMainChannel[mVinId][mChainId].pCtx->Id.Ctx.Data;
        }
    } else {
        /* algo id (non-vr) */
        mImageChanId.Ctx.Data = ImageChanId2.Ctx.Data;
    }

    return mImageChanId.Ctx.Data;
}

/**
 *  @private
 *  Amba image main ae piv
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] RawCapSeq raw capture sequence
 *  @param[out] pIsHiso pointer to the high iso enable flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_Piv(UINT32 VinId, UINT32 ChainId, UINT32 RawCapSeq, UINT32 *pIsHiso)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 HdrEnable;
    UINT32 ExposureNum;

    UINT32 AlgoId;

    AMBA_IMG_SENSOR_HAL_DATA_REG_s Reg;
    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    COMPUTE_EXIF_PARAMS_s ComputeExif;

    AE_MEM_INFO_s MemInfo;
    AE_SNAP_MEM_INFO_s MemInfo2;

    AEB_INFO_s AebInfo;

    AMBA_AE_INFO_s StillAeInfo[AMBA_IMG_MAX_EXPOSURE_NUM][MAX_AEB_NUM];
    AMBA_IK_WB_GAIN_s StillWbInfo[AMBA_IMG_MAX_EXPOSURE_NUM][MAX_AEB_NUM];

    /* clr */
    FuncRetCode = AmbaWrap_memset(StillAeInfo, 0, sizeof(StillAeInfo));
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaWrap_memset(StillWbInfo, 0, sizeof(StillWbInfo));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    if ((pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        /* ctx get */
        pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
        /* image channel id get */
        ImageChanId.Ctx.Data = AmbaImgMainAe_ImageChanIdGet(pCtx->Id);
        /* algo id get */
        AlgoId = ImageChanId.Ctx.Bits.AlgoId;

        /* hdr info get */
        HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
        ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

        /* reg get */
        FuncRetCode = AmbaImgSensorHAL_RingGet(ImageChanId, &Reg, RawCapSeq);
        if (FuncRetCode == OK_UL) {
            /* video user get */
            MemInfo.Ctx.Data = (AMBA_IMG_MEM_ADDR) Reg.User;
            /* hdr? */
            if (HdrEnable > 0U) {
            /* offline aaa info put (TBD) */
                FuncRetCode = AmbaImgProc_SetOfflinePivAAAInfo(AlgoId, &(MemInfo.Ctx.pUser->AaaInfo));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }
            /* aeb reset */
            AebInfo.Num = 1;
            for (i = 0U; i < MAX_AEB_NUM; i++) {
                AebInfo.EvBias[i] = (INT8) 0;
            }
            FuncRetCode = AmbaImgProc_AESetAEBInfo(AlgoId, &AebInfo);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* piv user buf get */
            MemInfo2.Ctx.pUser = &(AmbaImgMainAe_SnapUserData[VinId][ChainId][0]);
            /* piv user ae buff put */
            AmbaImgMainAe_SnapUser[VinId][ChainId][0] = (UINT64) MemInfo.Ctx.Data;

            /* piv ae/wb info get */
            for (i = 0U; i < ExposureNum; i++) {
                FuncRetCode = AmbaWrap_memcpy(&(StillAeInfo[i][0]), &(MemInfo.Ctx.pUser->AeInfo.Ctx[i]), sizeof(AMBA_AE_INFO_s));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaWrap_memcpy(&(StillWbInfo[i][0]), &(MemInfo.Ctx.pUser->WbInfo.Ctx[i]), sizeof(AMBA_IK_WB_GAIN_s));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* piv ae/awb info put */
            for (i = 0U; i < ExposureNum; i++) {
                MemInfo2.Ctx.pUser->AeInfo.Ctx[i] = StillAeInfo[i][0];
                MemInfo2.Ctx.pUser->WbInfo.Ctx[i] = StillWbInfo[i][0];
            }

            /* wb id put */
            MemInfo2.Ctx.pUser->WbInfo.Dsp.WbId = MemInfo.Ctx.pUser->WbInfo.Dsp.WbId;

            if (HdrEnable > 0U) {
                /* shift ae gain (TBD) */
                FuncRetCode = AmbaImgProc_AdjHdrSetStillShiftAeGain(AlgoId, 0U, MemInfo.Ctx.pUser->AaaInfo.ShiftAeGain);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* offline hdr control (TBD) */
                FuncRetCode = AmbaImgProc_OfflineHdrPivControl(AlgoId, MemInfo.Ctx.pUser->AaaInfo.BlendRatio, MemInfo.Ctx.pUser->AaaInfo.MaxBlendRatio, MemInfo.Ctx.pUser->AaaInfo.MinBlendRatio);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* piv gain build for algo */
            FuncRetCode = AmbaImgMainAe_UserPivGainPut(ImageChanId, AlgoId, MemInfo2.Ctx.pVoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* piv user data put */
            FuncRetCode = AmbaImgMainAe_UserPivDataPut(ImageChanId, AlgoId, MemInfo2.Ctx.pVoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* dol hdr? */
            if (HdrEnable == 1U) {
                /* piv hdr raw offset */
                MemInfo2.Ctx.pUser->HdrInfo.Op.Bits.RawOffset = MemInfo.Ctx.pUser->HdrInfo.Op.Bits.RawOffset;
                FuncRetCode = AmbaWrap_memcpy(&(MemInfo2.Ctx.pUser->HdrInfo.RawOffset), &(MemInfo.Ctx.pUser->HdrInfo.RawOffset), sizeof(AMBA_IK_HDR_RAW_INFO_s));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* iso mode get */
            FuncRetCode = AmbaImgProc_ADJChkStillIdx(AlgoId, &(MemInfo2.Ctx.pUser->AeInfo.Ctx[0]));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* iso mode put, app layer need prepare the array for IsHiso, if aeb is enabled */
            if (MemInfo2.Ctx.pUser->AeInfo.Ctx[0].Mode == (UINT16) IP_MODE_LISO_STILL) {
                /* liso (TBD) */
                pIsHiso[0] = 0U;
            } else {
                /* hiso (TBD) */
                pIsHiso[0] = 1U;
            }
        } else {
            /* ae null */
            RetCode = NG_UL;
        }

        /* exif info cfg (TBD) */
        ComputeExif.ChannelNo = AlgoId;
        ComputeExif.Mode = IMG_EXIF_PIV;
        ComputeExif.AeIdx = 0U;
        FuncRetCode = AmbaWrap_memset(&(AmbaImgMainAe_SnapExif[VinId][ChainId][0]), 0, sizeof(EXIF_INFO_s));
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* exif info calculate */
        Amba_Exif_Compute_AAA_Exif(&ComputeExif);
        /* exif info get */
        (void) Amba_Exif_Get_Exif_Info(0U, &(AmbaImgMainAe_SnapExif[VinId][ChainId][0]));

        /* exif aeb count clr */
        AmbaImgMainAe_SnapExifAebCount[VinId][ChainId] = 0U;
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}
#endif
/**
 *  @private
 *  Amba image main ae snap dsp dgc
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] pCfa pointer to the CFA statistics data
 *  @param[in] IsPiv piv enable flag
 *  @param[in] IsHiso high iso enable flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_SnapDspDgc(UINT32 VinId, UINT32 ChainId, const AMBA_IK_CFA_3A_DATA_s *pCfa, UINT32 IsPiv, UINT32 IsHiso)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 SnapAebIndex;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    (void) pCfa;
    (void) IsHiso;

    if ((pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* timing debug (TBD) */
            if (AmbaImgMain_DebugStillTiming > 0U) {
                AmbaImgSensorHAL_TimingMarkPrt(VinId);
            }

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "still ae iso", VinId, ChainId);

            /* image channel get */
            ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
            /* zone id get */
            ImageChanId.Ctx.Bits.ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId);

            if (IsPiv == 1U) {
                /* piv (r2y), evaluated only */
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
                /* piv proc */
                FuncRetCode = AmbaImgMainAe_AikPivIso(ImageChanId, AmbaImgMainAe_SnapUser[VinId][ChainId][0]);
                if (FuncRetCode != OK_UL) {
                    /* piv proc fail */
                    RetCode = NG_UL;
                }
#endif
            } else {
                /* aeb index get */
                SnapAebIndex = AmbaImgMainAe_SnapAebCount[VinId][ChainId];
                /* still proc */
                FuncRetCode = AmbaImgMainAe_AikSnapIso(ImageChanId, AmbaImgMainAe_SnapUser[VinId][ChainId][SnapAebIndex]);
                if (FuncRetCode != OK_UL) {
                    /* still proc fail */
                    RetCode = NG_UL;
                }
                /* aeb index next */
                if ((pCtx->Snap.Ctx.Bits.AebId > 0U) && (IsPiv == 0U)) {
                    AmbaImgMainAe_SnapAebCount[VinId][ChainId]++;
                }
                /* r2r? */
                if (IsPiv == 3U) {
                    /* r2r */
                    AMBA_IMG_CHANNEL_ID_s ImageChanId2;
                    const UINT32 Wgc[3] = {4096U, 4096U, 4096U};
                    /* image channel id get */
                    ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
                    /* zone id get */
                    ImageChanId2.Ctx.Bits.ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId2);
                    /* wb (FE/BE/BE_ALT) reset (r2r) */
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "wb reset (r2r)");
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, AMBA_IK_PIPE_STILL | ((UINT32) WB_PIPE_FE), Wgc);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, AMBA_IK_PIPE_STILL | ((UINT32) WB_PIPE_BE), Wgc);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, AMBA_IK_PIPE_STILL | ((UINT32) WB_PIPE_BE_ALT), Wgc);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae snap exif get
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] pExifInfo pointer to the exif information pointer
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_SnapExifGet(UINT32 VinId, UINT32 ChainId, void **pExifInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 SnapAebIndex;

    AE_SNAP_MEM_INFO_s MemInfo;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    if ((pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* aeb index get */
            SnapAebIndex = AmbaImgMainAe_SnapExifAebCount[VinId][ChainId];
            /* exif info get */
            MemInfo.Ctx.pExifInfo = &(AmbaImgMainAe_SnapExif[VinId][ChainId][SnapAebIndex]);
            /* exif info put */
            *pExifInfo = MemInfo.Ctx.pVoid;
            /* aeb index next */
            if (pCtx->Snap.Ctx.Bits.AebId > 0U) {
                AmbaImgMainAe_SnapExifAebCount[VinId][ChainId]++;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

