/**
 *  @file AmbaImgMainSync.c
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
 *  @details Amba Image Main Sync
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"
#include "AmbaDSP_Event.h"

#include "AmbaDSP.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaSensor.h"

#include "AmbaDspInt.h"

#include "AmbaImgFramework.h"
//#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainSync.h"
#include "AmbaImgMainSync_Platform.h"

#define OK_UL    ((UINT32)  0U)
#define NG_UL    ((UINT32)  1U)

typedef union /*_SYNC_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR     Data;
    void                       *pVoid;
    const void                 *pCvoid;
    AMBA_DSP_RAW_DATA_RDY_s    *pRaw;
} SYNC_MEM_INFO_u;

typedef struct /*_SYNC_MEM_INFO_s_*/ {
    SYNC_MEM_INFO_u Ctx;
} SYNC_MEM_INFO_s;

typedef union /*_SYNC_TASK_PARAM_u_*/ {
    UINT32 Data;
    struct {
        UINT32 VinId:    16;
        UINT32 ChainId:  16;
    } Bits;
} SYNC_TASK_PARAM_u;

typedef struct /*_SYNC_TASK_PARAM_s_*/ {
    SYNC_TASK_PARAM_u    Ctx;
} SYNC_TASK_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_SYNC_TASK_INFO_s_*/ {
    char Name[32];
} AMBA_IMG_MAIN_SYNC_TASK_INFO_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainSyncTaskId[AMBA_IMG_NUM_VIN_CHANNEL];
static void AmbaImgMainSync_Task(UINT32 Param);
static void *AmbaImgMainSync_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainSync_Ready[AMBA_IMG_NUM_VIN_CHANNEL];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainSync_Go[AMBA_IMG_NUM_VIN_CHANNEL];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainSync_Ack[AMBA_IMG_NUM_VIN_CHANNEL];

static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainSync_Cmd[AMBA_IMG_NUM_VIN_CHANNEL];

static AMBA_DSP_INT_PORT_s GNU_SECTION_NOZEROINIT AmbaImgMainSync_Port[AMBA_IMG_NUM_VIN_CHANNEL];
#define AMBA_IMG_MAIN_SYNC_PUSH_EVENT_FLAG    0x80000000U

/* sync task dummy flag */
static UINT32 AmbaImgMainSync_DummyFlag[AMBA_IMG_NUM_VIN_CHANNEL];

static UINT32 AmbaImgMainSync_SofEndMargin[AMBA_IMG_NUM_VIN_CHANNEL];
static UINT32 AmbaImgMainSync_MaskFlag = 0xFFFFFFFFU;

static AMBA_KAL_MUTEX_t GNU_SECTION_NOZEROINIT AmbaImgMainSync_RawRegisterMtx;
static UINT32 AmbaImgMainSync_RawRegisterCnt = 0U;

UINT32 AmbaImgMainSync_DebugCount[AMBA_IMG_NUM_VIN_CHANNEL] = {0};
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainSync_VinNum;

/**
 *  @private
 *  Amba image main sync create
 *  @param[in] VinId vin id
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainSync_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_SYNC_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainSync_TaskStack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_MAIN_SYNC_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_SYNC_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainSync_TaskInfo[AMBA_IMG_NUM_VIN_CHANNEL];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    SYNC_TASK_PARAM_s ParamInfo;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU ; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
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
                    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainSync_Ready[VinId], NULL, 0U);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainSync_Go[VinId], NULL, 0);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainSync_Ack[VinId], NULL, 0);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    /* task param */
                    ParamInfo.Ctx.Bits.VinId = (UINT8) VinId;
                    ParamInfo.Ctx.Bits.ChainId = (UINT8) 0U;
                    /* task name */
                    {
                        char str[11];
                        UINT32 VinStrNum = (AmbaImgMainSync_VinNum > 9U) ? (UINT32) 2U : (UINT32) 1U;
                        AmbaImgMainSync_TaskInfo[VinId].Name[0] = '\0';
                        (void) var_strcat(AmbaImgMainSync_TaskInfo[VinId].Name, "ImageVin");
                        var_utoa(VinId, str, 10U, VinStrNum, (UINT32) VAR_LEADING_ZERO);
                        (void) var_strcat(AmbaImgMainSync_TaskInfo[VinId].Name, str);
                        (void) var_strcat(AmbaImgMainSync_TaskInfo[VinId].Name, "Sync");
                    }
                    /* task */
                    if (RetCode == OK) {
                        AmbaImgPrint(PRINT_FLAG_MSG, "image create sync task", VinId);
                        {
                            SYNC_MEM_INFO_s MemInfo;
                            MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) ParamInfo.Ctx.Data;
                            FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainSyncTaskId[VinId],
                                                             AmbaImgMainSync_TaskInfo[VinId].Name,
                                                             Priority,
                                                             AmbaImgMainSync_TaskEx,
                                                             MemInfo.Ctx.pVoid,
                                                             AmbaImgMainSync_TaskStack[VinId],
                                                             AMBA_IMG_MAIN_SYNC_TASK_STACK_SIZE,
                                                             AMBA_KAL_DONT_START);
                        }
                        if (FuncRetCode == OK) {
                            UINT32 SyncCoreInclusion = CoreInclusion;
                            if (AmbaImgMain_ImgAaaCore[VinId][i] > 0U) {
                                SyncCoreInclusion = AmbaImgMain_ImgAaaCore[VinId][i];
                            }
                            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainSyncTaskId[VinId], SyncCoreInclusion);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                            FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainSyncTaskId[VinId]);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                        } else {
                            RetCode = NG_UL;
                        }
                    }
                    /* */
                    break;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main sync delete
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainSync_Delete(UINT32 VinId)
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
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
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
                    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainSyncTaskId[VinId]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainSyncTaskId[VinId]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    /* semaphore */
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainSync_Ready[VinId]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainSync_Go[VinId]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainSync_Ack[VinId]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    /* */
                    break;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main sync init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainSync_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    /* sync task dummy flag init */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        AmbaImgMainSync_DummyFlag[i] = 1U;
        AmbaImgMainSync_SofEndMargin[i] = 0U;
    }
    /* sync debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            AmbaImgMainSync_DebugCount[i] = 0xFFFFFFFFU;
        }
    }

    /* raw register cnt & mtx init */
    AmbaImgMainSync_RawRegisterCnt = 0U;
    FuncRetCode = AmbaKAL_MutexCreate(&AmbaImgMainSync_RawRegisterMtx, NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    AmbaImgMainSync_VinNum = AMBA_IMG_NUM_VIN_CHANNEL;

    return RetCode;
}

/**
 *  @private
 *  Amba image main sync active
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainSync_Active(UINT32 VinId)
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
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
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
                    if (AmbaImgMainSync_Cmd[VinId] == (UINT32) SYNC_TASK_CMD_STOP) {
                        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainSync_Ready[VinId], AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* cmd start */
                            AmbaImgMainSync_Cmd[VinId] = (UINT32) SYNC_TASK_CMD_START;
                            /* task go */
                            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainSync_Go[VinId]);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* ack take */
                                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainSync_Ack[VinId], AMBA_KAL_WAIT_FOREVER);
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
                    /* */
                    break;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main sync inactive
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainSync_Inactive(UINT32 VinId)
{
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
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
                    if (AmbaImgMainSync_Cmd[VinId] == (UINT32) SYNC_TASK_CMD_START) {
                        AmbaImgMainSync_Cmd[VinId] = (UINT32) SYNC_TASK_CMD_STOP;
                    }
                    /* */
                    break;
                }
            }
        }
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba image main sync push
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainSync_Push(UINT32 VinId)
{
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU ; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
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
                    if (AmbaImgMainSync_Cmd[VinId] == (UINT32) SYNC_TASK_CMD_STOP) {
                        (void) AmbaDspInt_AuxGive(&(AmbaImgMainSync_Port[VinId]), AMBA_IMG_MAIN_SYNC_PUSH_EVENT_FLAG);
                    }
                    /* */
                    break;
                }
            }
        }
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba image main sync idle wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainSync_IdleWait(UINT32 VinId)
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
            if (pAmbaImgMainChannel[VinId][i].pCtx != NULL) {
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
                    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainSync_Ready[VinId], AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainSync_Ready[VinId]);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                    } else {
                        RetCode = NG_UL;
                    }
                    /* */
                    break;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main sync raw
 *  @param[in] pData pointer to the raw data ready information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainSync_Raw(const void *pData)
{
    UINT32 FuncRetCode;
    SYNC_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pCvoid = pData;
    FuncRetCode = AmbaImgSensorSync_RawCapSeqPut(MemInfo.Ctx.pRaw->VinId, (UINT32) (MemInfo.Ctx.pRaw->CapSequence & 0xFFFFFFFFULL));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    return OK;
}

/**
 *  @private
 *  Amba image main sync enable
 *  @param[in] VinId vin id
 *  @param[in] Enable enable/disable
 *  @note this function is intended for internal use only
 */
void AmbaImgMainSync_Enable(UINT32 VinId, UINT32 Enable)
{
    UINT32 SyncFlags;

    SyncFlags = AmbaImgMainSync_Flag[VinId].Sof | AmbaImgMainSync_Flag[VinId].Eof;

    if (Enable == 0U) {
        /* disable */
        AmbaImgMainSync_MaskFlag &= ~SyncFlags;
    } else {
        /* enable */
        AmbaImgMainSync_MaskFlag |= SyncFlags;
    }
}

/**
 *  @private
 *  Amba image main sync margin
 *  @param[in] VinId vin id
 *  @param[in] Margin margin value
 *  @param[in] Mode margin mode
 *  @note this function is intended for internal use only
 */
void AmbaImgMainSync_Margin(UINT32 VinId, UINT32 Margin, UINT32 Mode)
{
    (void) Mode;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        AmbaImgMainSync_SofEndMargin[VinId] = Margin;
    }
}

/**
 *  Amba image main sync timing update
 *  @param[in] ImageChanId image channel id
 *  @param[in] pShr pointer to the HDR shutter data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainSync_TimingUpdate(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_SENSOR_HDR_INFO_s SensorHdrInfo;
    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = (UINT32) ImageChanId.Ctx.Bits.VinId;
    ChainId = (UINT32) ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].pInfo != NULL)) {
        /* dol-hdr? */
        if (AmbaImgSystem_Chan[VinId][ChainId].pInfo->Pipe.Hdr.Bits.Enable == 1U) {
            /* sensor id get */
            SensorChanId.VinID = VinId;
            for (i = 0U; (i < AMBA_IMG_NUM_VIN_ALGO) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> i) > 0U); i++) {
                if ((((UINT32) ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << i)) > 0U) {
                    SensorChanId.SensorID = AmbaImgMain_SensorIdTable[VinId][i];
                    break;
                }
            }

            /* sensor hdr info get */
            FuncRetCode = AmbaSensor_GetHdrInfo(&SensorChanId, pShr, &SensorHdrInfo);
            if (FuncRetCode == SENSOR_ERR_NONE) {
                UINT32 ExposureNum = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;
                /* exposure num check */
                if ((ExposureNum > 1U) &&
                    (SensorHdrInfo.ActiveChannels == ExposureNum)) {
                    UINT32 MaxOffsetLine;
                    UINT32 OffsetLine = (UINT32) (SensorHdrInfo.ChannelInfo[ExposureNum-1U].EffectiveArea.StartY * ExposureNum);
                    UINT32 OffsetTimeInMs;

                    UINT32 CaptureHeight = AmbaImgSensorHAL_Info[VinId][ChainId].Cfg.Height;
                    UINT32 ImageHeight = AmbaImgSensorHAL_Info[VinId][ChainId].Image.Height;
                    UINT32 LineInNs = AmbaImgSensorHAL_Info[VinId][ChainId].Frame.TotalNsInLine;

                    UINT32 TempHeight;
                    UINT32 EofMoveLine = 0U;
                    UINT32 EofMoveTimeInMs;

                    /* post sof time */
                    OffsetTimeInMs = (((OffsetLine + 2U) * LineInNs) / 1000000U) + 1U;
                    if (OffsetTimeInMs < 2U) {
                        OffsetTimeInMs = 2U;
                    }

                    /* max offset line */
                    if (CaptureHeight > ImageHeight) {
                        TempHeight = ImageHeight * ExposureNum;
                        if (CaptureHeight > TempHeight) {
                            MaxOffsetLine = CaptureHeight - TempHeight;
                        } else {
                            MaxOffsetLine = 0U;
                            RetCode = NG_UL;
                        }
                    } else {
                        MaxOffsetLine = 0U;
                        RetCode = NG_UL;
                    }

                    /* eof moving line */
                    if (MaxOffsetLine >= OffsetLine) {
                        EofMoveLine = MaxOffsetLine - OffsetLine;
                    } else {
                        RetCode = NG_UL;
                    }

                    /* eof moving time */
                    EofMoveTimeInMs = ((EofMoveLine * LineInNs) / 1000000U) + 1U;

                    /* post sof */
                    AmbaImgSensorHAL_Info[VinId][ChainId].Op.User.ForcePostSof = OffsetTimeInMs;
                    /* max eof move time */
                    AmbaImgSensorHAL_Info[VinId][ChainId].Op.User.EofMoveMaxTime = EofMoveTimeInMs;

                    if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                        char str[11];
                        str[0] = ' ';str[1] = ' ';
                        var_utoa(OffsetTimeInMs, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        var_utoa(EofMoveTimeInMs, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    }

                    /* debug msg */
                    if (AmbaImgMainSync_DebugCount[VinId] < AmbaImgMain_DebugCount) {
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                        AmbaImgPrint2(PRINT_FLAG_DBG, "timing update", VinId, ChainId);
                        AmbaImgPrint(PRINT_FLAG_DBG, "exposure num", ExposureNum);
                        AmbaImgPrint(PRINT_FLAG_DBG, "offset line", OffsetLine);
                        AmbaImgPrint(PRINT_FLAG_DBG, "post sof time", OffsetTimeInMs);
                        AmbaImgPrint(PRINT_FLAG_DBG, "max offset line", MaxOffsetLine);
                        AmbaImgPrint(PRINT_FLAG_DBG, "eof move time", EofMoveTimeInMs);
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main sync task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainSync_TaskEx(void *pParam)
{
    SYNC_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainSync_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main sync task
 *  @param[in] Param vin id with the chain id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainSync_Task(UINT32 Param)
{
    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;

    UINT32 SofFlag;
    UINT32 EofFlag;
    UINT32 SyncFlags;
    UINT32 ActualFlags;

    UINT32 AmbaImgMainSync_Mask = 0xFFFFFFFFU;

    SYNC_TASK_PARAM_s TaskParam;
    UINT32 VinId;

    AMBA_DSP_INT_PORT_s *pSyncIntPort;

    UINT32 SofDtsCarry = 0U;
    UINT32 SofEndDtsCarry = 0U;
    UINT32 EofDtsCarry = 0U;

    UINT32 Dts;

    UINT32 LastSofDts = 0U;
    UINT32 LastSofEndDts = 0U;
    UINT32 LastEofDts = 0U;

    UINT64 SofDts = 0ULL;
    UINT64 SofEndDts = 0ULL;
    UINT64 EofDts = 0ULL;

    UINT64 SofDtsTemp = 0ULL;
    UINT64 SofDtsDiff = 0ULL;

    /* task param */
    TaskParam.Ctx.Data = Param;
    /* vin id */
    VinId = TaskParam.Ctx.Bits.VinId;

    /* sync port */
    pSyncIntPort = &(AmbaImgMainSync_Port[VinId]);

    /* sync flag */
    SofFlag = AmbaImgMainSync_Flag[VinId].Sof;
    EofFlag = AmbaImgMainSync_Flag[VinId].Eof;

    SyncFlags = SofFlag | EofFlag | AMBA_IMG_MAIN_SYNC_PUSH_EVENT_FLAG;

    AmbaImgMainSync_Cmd[VinId] = (UINT32) SYNC_TASK_CMD_STOP;

    while (AmbaImgMainSync_DummyFlag[VinId] == 1U) {
        if (AmbaImgMainSync_Cmd[VinId] == (UINT32) SYNC_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
                /* unregister raw event */
                FuncRetCode = AmbaKAL_MutexTake(&AmbaImgMainSync_RawRegisterMtx, AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* consumed counter */
                    if (AmbaImgMainSync_RawRegisterCnt > 0U) {
                        AmbaImgMainSync_RawRegisterCnt--;
                    }
                    /* last unregister */
                    if (AmbaImgMainSync_RawRegisterCnt == 0U) {
                        FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, AmbaImgMainSync_Raw);
                        if (FuncRetCode != OK) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync raw event unregister");
                        }
                    }
                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&AmbaImgMainSync_RawRegisterMtx);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
                /* dsp interrupt close */
                FuncRetCode = AmbaDspInt_Close(pSyncIntPort);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: svc dsp int close");
                }
            }

            /* task rdy2go */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainSync_Ready[VinId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: sync task rdy2go", VinId, FuncRetCode);
            }
            /* task wait2go */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainSync_Go[VinId], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: sync task wait2go", VinId, FuncRetCode);
            }

            /* register raw event */
            FuncRetCode = AmbaKAL_MutexTake(&AmbaImgMainSync_RawRegisterMtx, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* first register */
                if (AmbaImgMainSync_RawRegisterCnt == 0U) {
                    FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, AmbaImgMainSync_Raw);
                    if (FuncRetCode != OK) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync raw event register");
                    }
                }
                /* accumulated counter */
                AmbaImgMainSync_RawRegisterCnt++;
                /* mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&AmbaImgMainSync_RawRegisterMtx);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* register sensor sync timing cb */
            FuncRetCode = AmbaImgSensorSync_TimingCb(VinId, AmbaImgMainSync_TimingUpdate);
            if (FuncRetCode != OK) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: sync timing check register");
            }

            /* dsp interrupt open */
            FuncRetCode = AmbaDspInt_Open(pSyncIntPort);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrintStr(PRINT_FLAG_ERR, "error: svc dsp int open");
                AmbaImgPrint_Flush();
            }

            /* dts carry reset */
            SofDtsCarry = 0U;
            SofEndDtsCarry = 0U;
            EofDtsCarry = 0U;

            /* last dts reset */
            LastSofDts = 0U;
            LastSofEndDts = 0U;
            LastEofDts = 0U;

            /* dts reset */
            SofDts = 0xFFFFFFFFFFFFFFFFULL;
            SofEndDts = 0ULL;
            EofDts = 0ULL;

            /* sync mask */
            AmbaImgMainSync_Mask = AmbaImgMainSync_MaskFlag;

            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainSync_Ack[VinId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: sync task ack", VinId, FuncRetCode);
            }
        }

        FuncRetCode = AmbaDspInt_Take(pSyncIntPort, SyncFlags, &ActualFlags, 0xFFFFU);
        if (FuncRetCode == OK_UL) {
            if (AmbaImgMainSync_Cmd[VinId] == (UINT32) SYNC_TASK_CMD_STOP) {
                /* stop */
                continue;
            }
            if ((ActualFlags & EofFlag) > 0U) {
                /* eof dts calculate */
                FuncRetCode = AmbaKAL_GetSysTickCount(&Dts);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* carry? */
                    if (LastEofDts > Dts) {
                        EofDtsCarry++;
                    }
                    /* eof dts */
                    EofDts = (((UINT64) EofDtsCarry) << 32ULL) | ((UINT64) Dts);
                    /* last dts update */
                    LastEofDts = Dts;
                }
                /* debug message */
                if (AmbaImgMainSync_DebugCount[VinId] < AmbaImgMain_DebugCount) {
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "sync eof", VinId, 10U, (UINT32) EofDts, 16U);
                }
                /* eof mask check */
                if ((AmbaImgMainSync_Mask & EofFlag) > 0U) {
                    /* eof proc */
                    FuncRetCode = AmbaImgSensorSync_Eof(VinId);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* sync warning msg */
                    if (EofDts <= (SofEndDts + ((UINT64) AmbaImgMainSync_SofEndMargin[VinId]))) {
                        AmbaImgPrintStr(PRINT_FLAG_SYNC, "\n\n");
                        AmbaImgPrintStr(PRINT_FLAG_SYNC, "warning:");
                        AmbaImgPrintEx(PRINT_FLAG_SYNC,  "        vin", VinId, 10U);
                        AmbaImgPrintEx2(PRINT_FLAG_SYNC, "        sof", SofDtsCarry, 16U, (UINT32) SofDts, 16U);
                        AmbaImgPrintEx2(PRINT_FLAG_SYNC, "        sof end", SofEndDtsCarry, 16U, (UINT32) SofEndDts, 16U);
                        AmbaImgPrintEx2(PRINT_FLAG_SYNC, "        eof", EofDtsCarry, 16U, (UINT32) EofDts, 16U);
                        AmbaImgPrintEx(PRINT_FLAG_SYNC,  "        sof rest", (UINT32) (EofDts - SofEndDts), 10U);
                        AmbaImgPrintEx(PRINT_FLAG_SYNC,  "        sof dist", (UINT32) SofDtsDiff, 10U);
                        AmbaImgPrintStr(PRINT_FLAG_SYNC, "\n\n");
                    }
                    /* debug message */
                    if (AmbaImgMainSync_DebugCount[VinId] < AmbaImgMain_DebugCount) {
                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                        AmbaImgPrintEx(PRINT_FLAG_DBG,  "vin", VinId, 10U);
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "sof", SofDtsCarry, 16U, (UINT32) SofDts, 16U);
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "sof end", SofEndDtsCarry, 16U, (UINT32) SofEndDts, 16U);
                        AmbaImgPrintEx2(PRINT_FLAG_DBG, "eof", EofDtsCarry, 16U, (UINT32) EofDts, 16U);
                        AmbaImgPrintEx(PRINT_FLAG_DBG,  "sof rest", (UINT32) (EofDts - SofEndDts), 10U);
                    }
                }
                /* sync mask update */
                AmbaImgMainSync_Mask = AmbaImgMainSync_MaskFlag;
                /* debug message count */
                if (AmbaImgMainSync_DebugCount[VinId] < AmbaImgMain_DebugCount) {
                    /* count accumulate */
                    AmbaImgMainSync_DebugCount[VinId]++;
                    if (AmbaImgMainSync_DebugCount[VinId] == AmbaImgMain_DebugCount) {
                        /* count reset */
                        AmbaImgMainSync_DebugCount[VinId] = 0xFFFFFFFFU;
                    }
                }
            }
            if ((ActualFlags & SofFlag) > 0U) {
                /* sof dts calculate */
                FuncRetCode = AmbaKAL_GetSysTickCount(&Dts);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* carry? */
                    if (LastSofDts > Dts) {
                        SofDtsCarry++;
                    }
                    /* sof dts temp */
                    SofDtsTemp = SofDts;
                    /* sof dts */
                    SofDts = (((UINT64) SofDtsCarry) << 32ULL) | ((UINT64) Dts);
                    /* sof dts diff */
                    if (SofDts > SofDtsTemp) {
                        SofDtsDiff = SofDts - SofDtsTemp;
                    } else {
                        SofDtsDiff = 0ULL;
                    }
                    /* last sof dts */
                    LastSofDts = Dts;
                }
                /* debug message */
                if (AmbaImgMainSync_DebugCount[VinId] < AmbaImgMain_DebugCount) {
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    AmbaImgPrintEx(PRINT_FLAG_DBG,  "sync vin", VinId, 10U);
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "sync sof", SofDtsCarry, 16U, (UINT32) SofDts, 16U);
                    AmbaImgPrintEx(PRINT_FLAG_DBG,  "sync sof dist", (UINT32) SofDtsDiff, 10U);
                }
                /* sof mask check */
                if ((AmbaImgMainSync_Mask & SofFlag) > 0U) {
                    /* sof proc */
                    FuncRetCode = AmbaImgSensorSync_Sof(VinId);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* sof end */
                    FuncRetCode = AmbaKAL_GetSysTickCount(&Dts);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* carry? */
                        if (LastSofEndDts > Dts) {
                            SofEndDtsCarry++;
                        }
                        /* sof end dts */
                        SofEndDts = (((UINT64) SofEndDtsCarry) << 32ULL) | ((UINT64) Dts);
                        /* last sof end dts */
                        LastSofEndDts = Dts;
                    }
                }
            }
        }
    }
}

