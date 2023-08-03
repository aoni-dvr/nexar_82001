/**
 *  @file AmbaImgMainAf.c
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
 *  @details Amba Image Main Af
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Image3aStatistics.h"

#include "AmbaImg_External_CtrlFunc.h"
#ifdef CONFIG_BUILD_IMGFRW_AF
#include "AmbaAf_AfProc.h"
#endif
#include "AmbaImgFramework.h"
//#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainAf.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_AF_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR       Data;
    void                         *pVoid;
    const void                   *pCvoid;
    AMBA_IK_CFA_3A_DATA_s        *pCfa;
    AMBA_IK_PG_3A_DATA_s         *pRgb;
} AF_MEM_INFO_u;

typedef struct /*_AF_MEM_INFO_s_*/ {
    AF_MEM_INFO_u    Ctx;
} AF_MEM_INFO_s;

typedef union /*_AF_TASK_PARAM_u_*/ {
    UINT32 Data;
    struct {
        UINT32 VinId:    16;
        UINT32 ChainId:  16;
    } Bits;
} AF_TASK_PARAM_u;

typedef struct /*_AF_TASK_PARAM_s_*/ {
    AF_TASK_PARAM_u    Ctx;
} AF_TASK_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_AF_TASK_INFO_s_*/ {
    char Name[32];
} AMBA_IMG_MAIN_AF_TASK_INFO_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainAfTaskId[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static void AmbaImgMainAf_Task(UINT32 Param);
static void *AmbaImgMainAf_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAf_Ready[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAf_Go[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAf_Ack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAf_LockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAf_UnlockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAf_Cmd[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAf_Enable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};

static UINT32 AmbaImgMainAf_DummyFlag[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAf_DebugCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAf_VinNum;

/**
 *  @private
 *  Amba image main af create
 *  @param[in] VinId vin id
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_AF_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainAf_TaskStack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AF_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_AF_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainAf_TaskInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    AF_TASK_PARAM_s ParamInfo;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task create (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                /* semaphore */
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAf_Ready[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAf_Go[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAf_Ack[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAf_LockAck[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAf_UnlockAck[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* task param */
                ParamInfo.Ctx.Bits.VinId = (UINT8) VinId;
                ParamInfo.Ctx.Bits.ChainId = (UINT8) i;
                /* task name */
                {
                    char str[11];
                    UINT32 VinStrNum = (AmbaImgMainAf_VinNum > 9U) ? (UINT32) 2U : (UINT32) 1U;
                    AmbaImgMainAf_TaskInfo[VinId][i].Name[0] = '\0';
                    (void) var_strcat(AmbaImgMainAf_TaskInfo[VinId][i].Name, "ImageVin");
                    var_utoa(VinId, str, 10U, VinStrNum, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAf_TaskInfo[VinId][i].Name, str);
                    (void) var_strcat(AmbaImgMainAf_TaskInfo[VinId][i].Name, "Af");
                    var_utoa(i, str, 10U, 1U, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAf_TaskInfo[VinId][i].Name, str);
                }
                /* task */
                if (RetCode == OK) {
                    AmbaImgPrint2(PRINT_FLAG_MSG, "image create af task", VinId, i);
                    {
                        AF_MEM_INFO_s MemInfo;
                        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) ParamInfo.Ctx.Data;
                        FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainAfTaskId[VinId][i],
                                                         AmbaImgMainAf_TaskInfo[VinId][i].Name,
                                                         Priority,
                                                         AmbaImgMainAf_TaskEx,
                                                         MemInfo.Ctx.pVoid,
                                                         AmbaImgMainAf_TaskStack[VinId][i],
                                                         AMBA_IMG_MAIN_AF_TASK_STACK_SIZE,
                                                         AMBA_KAL_DONT_START);
                    }
                    if (FuncRetCode == OK) {
                        UINT32 AfCoreInclusion = CoreInclusion;
                        if (AmbaImgMain_ImgAaaCore[VinId][i] > 0U) {
                            AfCoreInclusion = AmbaImgMain_ImgAaaCore[VinId][i];
                        }
                        FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainAfTaskId[VinId][i], AfCoreInclusion);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainAfTaskId[VinId][i]);
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
 *  Amba image main af delete
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_Delete(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task delete (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                /* task */
                FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainAfTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainAfTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* semaphore */
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAf_Ready[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAf_Go[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAf_Ack[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAf_LockAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAf_UnlockAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main af init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 i, j;

    /* af task dummy flag init */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgMainAf_DummyFlag[i][j] = 1U;
        }
    }
    /* adj debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
                AmbaImgMainAf_DebugCount[i][j] = 0xFFFFFFFFU;
            }
        }
    }

    AmbaImgMainAf_VinNum = AMBA_IMG_NUM_VIN_CHANNEL;

    return RetCode;
}

/**
 *  @private
 *  Amba image main af active
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_Active(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task active (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                if (AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_STOP) {
                    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAf_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* cmd start */
                        AmbaImgMainAf_Cmd[VinId][i] = (UINT32) AF_TASK_CMD_START;
                        /* task go */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAf_Go[VinId][i]);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* ack take */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAf_Ack[VinId][i], AMBA_KAL_WAIT_FOREVER);
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
 *  Amba image main af inactive
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_Inactive(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task inactive (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                if ((AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_START) ||
                    (AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_LOCK_DOWN)) {
                    AmbaImgMainAf_Cmd[VinId][i] = (UINT32) AF_TASK_CMD_STOP;
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
 *  Amba image main af idle wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_IdleWait(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task idlewait (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAf_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAf_Ready[VinId][i]);
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
 *  Amba image main af lock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_Lock(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task active (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                if (AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_START) {
                    /* lock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAf_LockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* lock cmd */
                    AmbaImgMainAf_Cmd[VinId][i] = (UINT32) AF_TASK_CMD_LOCK;
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
 *  Amba image main af lock wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_LockWait(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task active (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                if ((AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_LOCK) ||
                    (AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_LOCK_DOWN)) {
                    /* lock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAf_LockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgPrint2(PRINT_FLAG_LOCK, "af lock ack", VinId, (UINT32) ImageChanId.Ctx.Bits.AlgoId);
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
 *  Amba image main af unlock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAf_Unlock(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pAmbaImgMainChannel != NULL) && (pAmbaImgMainChannel[VinId] != NULL)) {
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pAmbaImgMainChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* image channel get */
            ImageChanId.Ctx.Data = pAmbaImgMainChannel[VinId][i].pCtx->Id.Ctx.Data;

            /* chain id get */
            ChainId = ImageChanId.Ctx.Bits.ChainId;

            /* task active (af enable) */
            if (AmbaImgMainAf_Enable[VinId][ChainId] > 0U) {
                if ((AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_LOCK) ||
                    (AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAf_Cmd[VinId][i] == (UINT32) AF_TASK_CMD_LOCK_DOWN)) {
                    /* unlock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAf_UnlockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* unlock cmd */
                    AmbaImgMainAf_Cmd[VinId][i] = (UINT32) AF_TASK_CMD_UNLOCK;
                    /* unlock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAf_UnlockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
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

/**
 *  Amba image main af aaa algorithm id get
 *  @param[in] ImageChanId image channel id
 *  @return algorithm  id
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAf_AaaAlgoIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
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
 *  Amba image main af dummy focus check
 *  @param[in] pFlag pointer to flag
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAf_DummyFocusChk(UINT32 *pFlag)
{
    *pFlag = 1U;
}

/**
 *  Amba image main af task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainAf_TaskEx(void *pParam)
{
    AF_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainAf_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main af task
 *  @param[in] Param vin id with the chain id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAf_Task(UINT32 Param)
{
    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
     INT32 m;

    AF_MEM_INFO_s MemInfo;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    static AMBA_IMG_STATISTICS_PORT_s AmbaImgMainAf_StatPort[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
    AMBA_IMG_STATISTICS_PORT_s *pPort;

    AF_TASK_PARAM_s TaskParam;
    UINT32 VinId;
    UINT32 ChainId;

    UINT32 AlgoId = 0U;
    UINT32 StatisticsIndex = 0U;

    UINT32 AaaAlgoId = 0U;

    AMBA_AAA_OP_INFO_s AaaOpInfo;

    UINT32 ZoneId;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx = NULL;

    UINT32 FocusOnFlag;

    /* task param */
    TaskParam.Ctx.Data = Param;
    /* vin id */
    VinId = TaskParam.Ctx.Bits.VinId;
    /* chain id */
    ChainId = TaskParam.Ctx.Bits.ChainId;

    /* param init */
    ImageChanId.Ctx.Data = 0ULL;

    pPort = &(AmbaImgMainAf_StatPort[VinId][ChainId]);

    AmbaImgMainAf_Cmd[VinId][ChainId] = (UINT32) AF_TASK_CMD_STOP;

    while (AmbaImgMainAf_DummyFlag[VinId][ChainId] == 1U) {
        if (AmbaImgMainAf_Cmd[VinId][ChainId] == (UINT32) AF_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
                /* statistics close */
                FuncRetCode = AmbaImgStatistics_Close(pPort);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: af statistics close", VinId, FuncRetCode);
                }
            }

            /* task rdy2go */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAf_Ready[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: af task rdy2go", VinId, FuncRetCode);
            }
            /* task wait2go */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAf_Go[VinId][ChainId], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: af task wait2go", VinId, FuncRetCode);
            }
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* ctx check */
            if (pCtx == NULL) {
                AmbaImgMainAf_Cmd[VinId][ChainId] = (UINT32) AF_TASK_CMD_STOP;
                continue;
            }

            /* image channel get */
            ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
            /* algo id get */
            AlgoId = ImageChanId.Ctx.Bits.AlgoId;
            /* aaa algo id get */
            AaaAlgoId = AmbaImgMainAf_AaaAlgoIdGet(ImageChanId);

            /* zone id, belong to this sensor */
            ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId);
            /* statistics index */
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

            /* af zone id get */
            ImageChanId.Ctx.Bits.ZoneId = (((UINT32) 1U) << StatisticsIndex);

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "vin id", VinId, ImageChanId.Ctx.Bits.VinId);
            AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, ImageChanId.Ctx.Bits.ChainId);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "sensor id", VinId, 10U, ImageChanId.Ctx.Bits.SensorId, 16U);
            AmbaImgPrint2(PRINT_FLAG_DBG, "algo id", VinId, AlgoId);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "zone id", VinId, 10U, ImageChanId.Ctx.Bits.ZoneId, 16U);
            AmbaImgPrint2(PRINT_FLAG_DBG, "statistics index", VinId, StatisticsIndex);
            AmbaImgPrint(PRINT_FLAG_DBG, "aaa algo id", AaaAlgoId);

            /* af init */

            /* statistic open */
            FuncRetCode = AmbaImgStatistics_Open(ImageChanId, pPort);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: af statistics open", VinId, FuncRetCode);
            }

            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAf_Ack[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: af task ack", VinId, FuncRetCode);
            }
        }

        FuncRetCode = AmbaImgStatistics_Request(pPort, REQUEST_WAIT_FOREVER);
        if (FuncRetCode != OK_UL) {
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: af statistics request", VinId, FuncRetCode);
        } else {
            /* task stop or statistics null? */
            if ((AmbaImgMainAf_Cmd[VinId][ChainId] == (UINT32) AF_TASK_CMD_STOP) ||
                (AmbaImgMainAf_Cmd[VinId][ChainId] == (UINT32) AF_TASK_CMD_LOCK_DOWN) ||
                (pPort->Data.pCfa == NULL) ||
                (pPort->Data.pRgb == NULL)) {
                continue;
            }

            /* af lock/unlock for still */
            switch (AmbaImgMainAf_Cmd[VinId][ChainId]) {
                case (UINT32) AF_TASK_CMD_LOCK:
                    /* af in lock */

                    /* af cmd lock proc */
                    AmbaImgMainAf_Cmd[VinId][ChainId] = (UINT32) AF_TASK_CMD_LOCK_PROC;
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "af lock", VinId, AlgoId);
                    break;
                case (UINT32) AF_TASK_CMD_UNLOCK:
                    /* af in unlock */

                    /* af cmd start */
                    AmbaImgMainAf_Cmd[VinId][ChainId] = (UINT32) AF_TASK_CMD_START;
                    /* unlock ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAf_UnlockAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "af unlock", VinId, AlgoId);
                    break;
                default:
                    /* */
                    break;
            }

            /* af lock proc */
            if ((AmbaImgMainAf_Cmd[VinId][ChainId] == (UINT32) AF_TASK_CMD_LOCK_PROC)) {
                /* af lock */
                AmbaImgMainAf_DummyFocusChk(&FocusOnFlag);
                /* af lockdown? */
                if (FocusOnFlag == 1U) {
                    /* lockdown */
                    AmbaImgMainAf_Cmd[VinId][ChainId] = (UINT32) AF_TASK_CMD_LOCK_DOWN;
                    /* lock ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAf_LockAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "af lockdown", VinId, AlgoId);
                }
            }

            /* aaa op get */
            FuncRetCode = AmbaImgProc_GetAAAOPInfo(AlgoId, &AaaOpInfo);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* op enable? */
            if (AaaOpInfo.AfOp > 0U) {
                /* debug only */
                if (AmbaImgMainAf_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "[Af Statistics]");
                    MemInfo.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "cfa", VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                    AmbaImgPrint(PRINT_FLAG_DBG, "af row", MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AF.AfTileNumRow);
                    AmbaImgPrint(PRINT_FLAG_DBG, "af col", MemInfo.Ctx.pCfa->AAA_HEADER_CFA_AF.AfTileNumCol);
                    MemInfo.Ctx.pCvoid = pPort->Data.pRgb[StatisticsIndex];
                    AmbaImgPrintEx2(PRINT_FLAG_DBG, "rgb", VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                    AmbaImgPrint(PRINT_FLAG_DBG, "af row", MemInfo.Ctx.pRgb->AAA_HEADER_RGB_AF.AfTileNumRow);
                    AmbaImgPrint(PRINT_FLAG_DBG, "af col", MemInfo.Ctx.pRgb->AAA_HEADER_RGB_AF.AfTileNumCol);
                }

                /* af */
#ifdef CONFIG_BUILD_IMGFRW_AF
                {
                    AF_MEM_INFO_s MemInfo2;
                    AF_MEM_INFO_s MemInfo3;

                    MemInfo2.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                    MemInfo3.Ctx.pCvoid = pPort->Data.pRgb[StatisticsIndex];

                    FuncRetCode = AmbaAf_AfProcess(MemInfo2.Ctx.pCfa, MemInfo3.Ctx.pRgb);
                    if (FuncRetCode != AF_ERR_NONE) {
                        /* */
                    }
                }
#endif
            }

            /* debug only */
            if (AmbaImgMainAf_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                /* count accumulate */
                AmbaImgMainAf_DebugCount[VinId][ChainId]++;
                if (AmbaImgMainAf_DebugCount[VinId][ChainId] == AmbaImgMain_DebugCount) {
                    /* count reset */
                    AmbaImgMainAf_DebugCount[VinId][ChainId] = 0xFFFFFFFFU;
                }
            }
        }
    }
}

