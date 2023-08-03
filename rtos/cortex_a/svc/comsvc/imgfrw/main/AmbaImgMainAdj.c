/**
 *  @file AmbaImgMainAdj.c
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
 *  @details Amba Image Main Adj
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"
#include "AmbaWrap.h"

#include "AmbaKAL.h"

#include "AmbaDSP.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AeAwbAdj_Control.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainAdj.h"
#include "AmbaImgMainAvm.h"
#include "AmbaImgMainFov.h"
#ifdef CONFIG_BUILD_IMGFRW_EFOV
#include "AmbaImgMainEFov.h"
#endif
#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_ADJ_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR       Data;
    void                         *pVoid;
    const void                   *pCvoid;
    AMBA_IK_CFA_3A_DATA_s        *pCfa;
    AMBA_IK_PG_3A_DATA_s         *pRgb;
    AMBA_IP_GLOBAL_AAA_DATA_s    *pAaaData;
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    AMBA_IMG_MAIN_ADJ_EFOV_USER_s    *pEFovUser;
    AMBA_IMG_MAIN_ADJ_EFOV_TXRX_s    *pTxRx;
#endif
} ADJ_MEM_INFO_u;

typedef struct /*_ADJ_MEM_INFO_s_*/ {
    ADJ_MEM_INFO_u    Ctx;
} ADJ_MEM_INFO_s;

typedef union /*_ADJ_TASK_PARAM_u_*/ {
    UINT32 Data;
    struct {
        UINT32 VinId:    16;
        UINT32 ChainId:  16;
    } Bits;
} ADJ_TASK_PARAM_u;

typedef struct /*_ADJ_TASK_PARAM_s_*/ {
    ADJ_TASK_PARAM_u    Ctx;
} ADJ_TASK_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_TASK_INFO_s_*/ {
    char Name[32];
} AMBA_IMG_MAIN_ADJ_TASK_INFO_s;

#define AMBA_IMG_MAIN_ADJ_AIK_SYNC
#ifdef AMBA_IMG_MAIN_ADJ_AIK_SYNC
#define AMBA_IMG_MAIN_ADJ_AIK_SLOT IMG_CHAN_AIK_SLOT_ADJ
#endif

#ifdef CONFIG_BUILD_IMGFRW_EFOV
static AMBA_IMG_MAIN_ADJ_EFOV_TXRX_s GNU_SECTION_NOZEROINIT AmbaImgMainAdj_EFovTxRxData[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_IMG_MAIN_ADJ_EFOV_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainAdj_EFovInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
#endif

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainAdjTaskId[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static void AmbaImgMainAdj_Task(UINT32 Param);
static void *AmbaImgMainAdj_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAdj_Ready[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAdj_Go[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAdj_Ack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAdj_LockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAdj_UnlockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAdj_Cmd[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static UINT32 AmbaImgMainAdj_DummyFlag[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAdj_DebugCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAdj_VinNum;

/**
 *  @private
 *  Amba image main adj create
 *  @param[in] VinId vin id
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_ADJ_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainAdj_TaskStack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_ADJ_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_ADJ_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainAdj_TaskInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    ADJ_TASK_PARAM_s ParamInfo;

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
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAdj_Ready[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAdj_Go[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAdj_Ack[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAdj_LockAck[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAdj_UnlockAck[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* task param */
                ParamInfo.Ctx.Bits.VinId = (UINT8) VinId;
                ParamInfo.Ctx.Bits.ChainId = (UINT8) i;
                /* task name */
                {
                    char str[11];
                    UINT32 VinStrNum = (AmbaImgMainAdj_VinNum > 9U) ? (UINT32) 2U : (UINT32) 1U;
                    AmbaImgMainAdj_TaskInfo[VinId][i].Name[0] = '\0';
                    (void) var_strcat(AmbaImgMainAdj_TaskInfo[VinId][i].Name, "ImageVin");
                    var_utoa(VinId, str, 10U, VinStrNum, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAdj_TaskInfo[VinId][i].Name, str);
                    (void) var_strcat(AmbaImgMainAdj_TaskInfo[VinId][i].Name, "Adj");
                    var_utoa(i, str, 10U, 1U, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAdj_TaskInfo[VinId][i].Name, str);
                }
                /* task */
                if (RetCode == OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_MSG, "image create adj task", VinId, i);
                    {
                        ADJ_MEM_INFO_s MemInfo;
                        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) ParamInfo.Ctx.Data;
                        FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainAdjTaskId[VinId][i],
                                                         AmbaImgMainAdj_TaskInfo[VinId][i].Name,
                                                         Priority,
                                                         AmbaImgMainAdj_TaskEx,
                                                         MemInfo.Ctx.pVoid,
                                                         AmbaImgMainAdj_TaskStack[VinId][i],
                                                         AMBA_IMG_MAIN_ADJ_TASK_STACK_SIZE,
                                                         AMBA_KAL_DONT_START);
                    }
                    if (FuncRetCode == OK) {
                        UINT32 AdjCoreInclusion = CoreInclusion;
                        if (AmbaImgMain_ImgAaaCore[VinId][i] > 0U) {
                            AdjCoreInclusion = AmbaImgMain_ImgAaaCore[VinId][i];
                        }
                        FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainAdjTaskId[VinId][i], AdjCoreInclusion);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainAdjTaskId[VinId][i]);
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
 *  Amba image main adj delete
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_Delete(UINT32 VinId)
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
                FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainAdjTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainAdjTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* semaphore */
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAdj_Ready[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAdj_Go[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAdj_Ack[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAdj_LockAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAdj_UnlockAck[VinId][i]);
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
 *  Amba image main adj init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 i, j;

    /* adj task dummy flag init */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            /* dummy flag */
            AmbaImgMainAdj_DummyFlag[i][j] = 1U;
#ifdef CONFIG_BUILD_IMGFRW_EFOV
            {
                UINT32 FuncRetCode;
                /* mem reset */
                FuncRetCode = AmbaWrap_memset(&(AmbaImgMainAdj_EFovInfo[i][j]), 0, sizeof(AMBA_IMG_MAIN_ADJ_EFOV_INFO_s));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }
#endif
        }
    }
    /* adj debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
                AmbaImgMainAdj_DebugCount[i][j] = 0xFFFFFFFFU;
            }
        }
    }

    AmbaImgMainAdj_VinNum = AMBA_IMG_NUM_VIN_CHANNEL;

    return RetCode;
}

/**
 *  @private
 *  Amba image main adj active
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_Active(UINT32 VinId)
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
                if (AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_STOP) {
                    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAdj_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* cmd start */
                        AmbaImgMainAdj_Cmd[VinId][i] = (UINT32) ADJ_TASK_CMD_START;
                        /* task go */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAdj_Go[VinId][i]);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* ack take */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAdj_Ack[VinId][i], AMBA_KAL_WAIT_FOREVER);
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
 *  Amba image main adj inactive
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_Inactive(UINT32 VinId)
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
                if ((AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_START) ||
                    (AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_LOCK_DOWN)) {
                    AmbaImgMainAdj_Cmd[VinId][i] = (UINT32) ADJ_TASK_CMD_STOP;
#ifdef AMBA_IMG_MAIN_ADJ_AIK_SYNC
                    if ((pAmbaImgMainChannel[VinId][i].pCtx->pAikSync != NULL) &&
                        (pAmbaImgMainChannel[VinId][i].pCtx->pAikSync->Slot[AMBA_IMG_MAIN_ADJ_AIK_SLOT].En > 0U)) {
                        UINT32 FuncRetCode;
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(pAmbaImgMainChannel[VinId][i].pCtx->pAikSync->Slot[AMBA_IMG_MAIN_ADJ_AIK_SLOT].Sem.Ack));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    }
#endif
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
 *  Amba image main adj idle wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_IdleWait(UINT32 VinId)
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
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAdj_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAdj_Ready[VinId][i]);
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
 *  Amba image main adj lock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_Lock(UINT32 VinId)
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
                if (AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_START) {
                    /* lock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAdj_LockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* lock cmd */
                    AmbaImgMainAdj_Cmd[VinId][i] = (UINT32) ADJ_TASK_CMD_LOCK;
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
 *  Amba image main adj lock wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_LockWait(UINT32 VinId)
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
                if ((AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_LOCK) ||
                    (AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_LOCK_DOWN)) {
                    /* lock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAdj_LockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgPrint2(PRINT_FLAG_LOCK, "adj lock ack", VinId, (UINT32) ImageChanId.Ctx.Bits.AlgoId);
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
 *  Amba image main adj unlock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_Unlock(UINT32 VinId)
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
                if ((AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_LOCK) ||
                    (AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAdj_Cmd[VinId][i] == (UINT32) ADJ_TASK_CMD_LOCK_DOWN)) {
                    /* unlock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAdj_UnlockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* unlock cmd */
                    AmbaImgMainAdj_Cmd[VinId][i] = (UINT32) ADJ_TASK_CMD_UNLOCK;
                    /* unlock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAdj_UnlockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
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
#ifdef CONFIG_BUILD_IMGFRW_EFOV
/**
 *  Amba image main adj external fov tx cb
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] TxData tx data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAdj_EFovTxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 TxData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    ADJ_MEM_INFO_s MemInfo;
    ADJ_MEM_INFO_s MemInfo2;

    VinId = ImageChanId.Ctx.Bits.VinId;

    /* msg get */
    Msg2.Ctx.Data = Msg;
    /* tx data get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) TxData;

    if (Msg2.Ctx.Bits2.Adj > 0U) {
        /* msg put */
        MemInfo.Ctx.pTxRx->Msg = Msg2.Ctx.Data;
        /* user get */
        MemInfo2.Ctx.pEFovUser = &(MemInfo.Ctx.pTxRx->User);
        /* efov tx adj put */

        /* efov tx adj write */
        FuncRetCode = AmbaImgMainEFov_Tx(ImageChanId, TxData, (UINT32) sizeof(AMBA_IMG_MAIN_ADJ_EFOV_TXRX_s));
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
        /* debug msg */
        if (FuncRetCode == OK) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_J");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_J!");
        }
        {
            char str[11];
            /* raw cap seq */
            str[0] = 'm';str[1] = ' ';
            var_utoa(MemInfo2.Ctx.pEFovUser->AaaStat.RawCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        }
    }

    return RetCode;
}

/**
 *  Amba image main adj external fov rx cb
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] RxData rx data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAdj_EFovRxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 RxData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 AlgoId;
    UINT32 ZoneId;

    UINT32 ExposureNum;

     INT64 aRawCapSeq;
     INT64 mRawCapSeq = 0LL;
     INT64 sRawCapSeq = 0LL;

     INT64 RawCapSeqTmp;
    UINT32 RawCapSeq;

    UINT32 Index;

    UINT8 Enable;
    AMBA_IP_GLOBAL_AAA_DATA_s *pAaaData;

    AMBA_IMG_MAIN_ADJ_EFOV_INFO_s *pEFovInfo;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    ADJ_MEM_INFO_s MemInfo;
    ADJ_MEM_INFO_s MemInfo2;
    ADJ_MEM_INFO_s MemInfo3;
    ADJ_MEM_INFO_s MemInfo4;

    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_MODE_CFG_s *pImgMode;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* msg get */
    Msg2.Ctx.Data = Msg;
    /* rx data get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) RxData;

    if (Msg2.Ctx.Bits2.Adj > 0U) {
        /* algo id get */
        AlgoId = ImageChanId.Ctx.Bits.AlgoId;
        /* zone id get */
        ZoneId = ImageChanId.Ctx.Bits.ZoneId;
        /* user get */
        MemInfo2.Ctx.pEFovUser = &(MemInfo.Ctx.pTxRx->User);
        /* hdr info get */
        ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

        /* cfa get */
        aRawCapSeq = (INT64) MemInfo2.Ctx.pEFovUser->AaaStat.RawCapSeq;
        /* ring get */
        pEFovInfo = &(AmbaImgMainAdj_EFovInfo[VinId][ChainId]);
        /* raw cap seq get */
        FuncRetCode = AmbaKAL_MutexTake(&(pEFovInfo->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            mRawCapSeq = (INT64) pEFovInfo->Raw.mCapSeq;
            sRawCapSeq = (INT64) pEFovInfo->Raw.sCapSeq;
            FuncRetCode = AmbaKAL_MutexGive(&(pEFovInfo->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
        /* stat index get */
        RawCapSeqTmp = (aRawCapSeq - mRawCapSeq) + sRawCapSeq;
        RawCapSeq = (RawCapSeqTmp > 0LL) ? (UINT32) RawCapSeqTmp : 0U;
        Index = RawCapSeq % AMBA_IMG_MAIN_ADJ_EFOV_STAT_NUM;

        /* cfa get */
        MemInfo3.Ctx.pCvoid = pEFovInfo->Ring.pCfa[Index];
        /* rgb get */
        MemInfo4.Ctx.pCvoid = pEFovInfo->Ring.pRgb[Index];

        /* ae/awb info put */
        for (i = 0U; i < ExposureNum; i++) {
            FuncRetCode = AmbaImgProc_AESetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(MemInfo2.Ctx.pEFovUser->AeInfo.Ctx[i]));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaImgProc_AWBSetWBGain(AlgoId, i, IP_MODE_VIDEO, &(MemInfo2.Ctx.pEFovUser->WbInfo.Ctx[i]));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
        /* stat put (TBD) */
        FuncRetCode = AmbaImgProc_SetCFAAAAStat(AlgoId, MemInfo3.Ctx.pCfa);
        if (FuncRetCode != OK_UL) {
            /* */
        }
        FuncRetCode = AmbaImgProc_SetPGAAAStat(AlgoId, MemInfo4.Ctx.pRgb);
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* histo put */
        FuncRetCode = AmbaImgProc_GAAAGetEnb(AlgoId, &Enable);
        if (FuncRetCode == OK) {
            if (Enable == 0U) {
                /* use the cfa histo (TBD) */
            } else {
                /* global aaa histo */
                FuncRetCode = AmbaImgProc_GAAAGetStat(AlgoId, &pAaaData);
                if (FuncRetCode == OK) {
                    for (i = 0U; i < AMBA_IK_CFA_HISTO_COUNT; i++) {
                        pAaaData->CfaHist.HisBinY[i] = MemInfo2.Ctx.pEFovUser->AaaStat.Histo.BinY[i];
                    }
                }
            }
        }

        /* adj calculate (TBD) */
        Amba_AdjControl(AlgoId);

        /* adj update */
        ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
        for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((ZoneId >> i) > 0U); i++) {
            if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                /* img mode get */
                ImageChanId2.Ctx.Bits.ZoneId = ((UINT32) 1U) << i;
                FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                if (FuncRetCode == OK_UL) {
                    FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                    if (FuncRetCode == OK_UL) {
                        /* context id get */
                        ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                        /* adj settle */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                        FuncRetCode = AmbaImgProc_ADJSetVideoParam(AlgoId, &ImgMode);
#else
                        FuncRetCode = Amba_Adj_SetVideoPipeCtrlParams(AlgoId, &ImgMode);
#endif
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
#if 1
                        {
                            char str[11];
                            /* fov */
                            str[0] = 'f';str[1] = ' ';
                            var_utoa(i, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[0] = 'a';str[1] = ' ';
                            var_utoa(AlgoId, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        }
#endif
                    }
                }
            }
        }
        /* adj reset flags */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
        FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 0U);
#else
        FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 0U);
#endif
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* debug msg */
        AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Rx_J");
        {
            char str[11];
            /* raw cap seq */
            str[0] = 'm';str[1] = ' ';
            var_utoa(MemInfo2.Ctx.pEFovUser->AaaStat.RawCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            str[0] = 's';str[1] = ' ';
            var_utoa(RawCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        }
    } else if (Msg2.Ctx.Bits2.Raw > 0U) {
        /* ring get */
        pEFovInfo = &(AmbaImgMainAdj_EFovInfo[VinId][ChainId]);
        /* raw sync mechanism in mof */
        FuncRetCode = AmbaKAL_MutexTake(&(pEFovInfo->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            pEFovInfo->Raw.mCapSeq = Msg2.Ctx.Bits2.Var;
            pEFovInfo->Raw.sCapSeq = AmbaImgSensorSync_Vin[VinId].Counter.Raw;
            FuncRetCode = AmbaKAL_MutexGive(&(pEFovInfo->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }

        /* debug msg */
        AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Rx_R");
        {
            char str[11];
            /* raw cap seq */
            str[0] = 'm';str[1] = ' ';
            var_utoa(pEFovInfo->Raw.mCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            str[0] = 's';str[1] = ' ';
            var_utoa(pEFovInfo->Raw.sCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        }
    } else {
        /* */
    }

    return RetCode;
}
#endif
#ifdef AMBA_IMG_MAIN_ADJ_AIK_SYNC
/**
 *  Amba image main adj aik callback
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAdj_AikCb(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
    AMBA_IMG_CHANNEL_AIK_SYNC_s *pAikSync;
    AMBA_IMG_CHANNEL_AIK_SLOT_s *pAikSlot;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    UINT32 AlgoId;
    UINT32 InclusiveZoneId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pAmbaImgMainChannel != NULL) &&
        (pAmbaImgMainChannel[VinId] != NULL)) {
        if (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) {
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            if (pCtx != NULL) {
                /* aik sync get */
                pAikSync = pCtx->pAikSync;
                if (pAikSync != NULL) {
                    /* aik slot get */
                    pAikSlot = &(pAikSync->Slot[AMBA_IMG_MAIN_ADJ_AIK_SLOT]);
                    /* sync enable? */
                    if (pAikSlot->En > 0U) {
                        /* req? */
                        FuncRetCode = AmbaKAL_SemaphoreTake(&(pAikSlot->Sem.Req), AMBA_KAL_NO_WAIT);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* info get */
                            InclusiveZoneId = pAikSlot->Msg[0];
                            /* algo id get */
                            AlgoId = pAikSlot->Msg[1];
                            /* adj set */
                            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
                            for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                                if ((InclusiveZoneId >> i) == 0U) {
                                    break;
                                }
                                if ((InclusiveZoneId & (((UINT32) 1U) << i)) > 0U) {
                                    ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                                    FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                                    if (FuncRetCode == OK_UL) {
                                        FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                                        if (FuncRetCode == OK_UL) {
                                            ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                                            FuncRetCode = AmbaImgProc_ADJSetVideoParam(AlgoId, &ImgMode);
#else
                                            FuncRetCode = Amba_Adj_SetVideoPipeCtrlParams(AlgoId, &ImgMode);
#endif
                                            if (FuncRetCode != OK_UL) {
                                                /* */
                                            }
                                            /* debug msg */
                                            FuncRetCode = AmbaImgSensorHAL_TimingMarkChk(VinId);
                                            if (FuncRetCode == OK_UL) {
                                                char str[11];
                                                if (pAikSlot->pName != NULL) {
                                                    AmbaImgSensorHAL_TimingMarkPut(VinId, pAikSlot->pName);
                                                }
                                                str[0] = 'c';str[1] = ' ';
                                                var_utoa(ImgMode.ContextId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                                str[0] = 'a';str[1] = ' ';
                                                var_utoa(AlgoId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                            }
                                        }
                                    }
                                }
                            }
                            /* adj reset flags */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                            FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 0U);
#else
                            FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 0U);
#endif
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                            /* adj ack */
                            FuncRetCode = AmbaKAL_SemaphoreGive(&(pAikSlot->Sem.Ack));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                            /* efov adj tx cb */
                            if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                                ADJ_MEM_INFO_s MemInfo;
                                AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;
                                Msg2.Ctx.Data = 0ULL;
                                Msg2.Ctx.Bits2.Adj = 1U;
                                MemInfo.Ctx.pTxRx = &(AmbaImgMainAdj_EFovTxRxData[VinId][ChainId]);
                                /* efov adj tx */
                                (void) pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_ADJ](pCtx->Id, Msg2.Ctx.Data, (UINT64) MemInfo.Ctx.Data);
                            }
#endif
                        }
                    }
                } else {
                    /* aik sync null */
                    RetCode = NG_UL;
                }
            } else {
                /* ctx null */
                RetCode = NG_UL;
            }
        } else {
            /* chan invalid */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}
#endif
/**
 *  Amba image main adj zone to algorithm channel get
 *  @param[in] pCtx pointer to the image channel context data
 *  @param[out] pZone2AlgoTable pointer to the zone to algorithm channel table
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAdj_Zone2AlgoGet(const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx, UINT32 *pZone2AlgoTable)
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
 *  Amba image main adj vr statistics processing
 *  @param[in] pPort pointer to the statistics port data
 *  @param[in] pZone2AlgoTable pointer to the zone to algorithm channel table
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAdj_VrStatProc(const AMBA_IMG_STATISTICS_PORT_s *pPort, const UINT32 *pZone2AlgoTable)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 AlgoId = pPort->ImageChanId.Ctx.Bits.AlgoId;
    UINT32 StatZoneId = pPort->ZoneId.Req;

    UINT32 StatCnt = 0U;

    UINT32 TileRow;
    UINT32 TileCol;

    ADJ_MEM_INFO_s MemInfo;
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
        if (AmbaImgMainAdj_DebugCount[pPort->ImageChanId.Ctx.Bits.VinId][pPort->ImageChanId.Ctx.Bits.ChainId] < AmbaImgMain_DebugCount) {
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
 *  Amba image main adj task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainAdj_TaskEx(void *pParam)
{
    ADJ_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainAdj_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main adj task
 *  @param[in] Param vin id with the chain id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAdj_Task(UINT32 Param)
{
    static UINT32 AmbaImgMainAdj_Zone2AlgoTable[32] = {0};

    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;
     INT32 m;

    UINT32 ExclusiveZoneId;
    UINT32 InclusiveZoneId = 0U;

    ADJ_MEM_INFO_s MemInfo;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    static AMBA_IMG_STATISTICS_PORT_s AmbaImgMainAdj_StatPort[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
    AMBA_IMG_STATISTICS_PORT_s *pPort;

    ADJ_TASK_PARAM_s TaskParam;
    UINT32 VinId;
    UINT32 ChainId;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 VrEnable = 0U;
    UINT32 VrMaster = 0U;

    UINT32 AlgoId = 0U;
    UINT32 StatisticsIndex = 0U;
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    UINT32 HdrEnable = 0U;
    UINT32 ExposureNum = 1U;
    AMBA_IMG_MAIN_ADJ_EFOV_INFO_s *pEFovInfo = NULL;
#endif
    AMBA_AAA_OP_INFO_s AaaOpInfo;

    UINT32 ZoneId;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx = NULL;
#ifdef AMBA_IMG_MAIN_ADJ_AIK_SYNC
    AMBA_IMG_CHANNEL_AIK_SLOT_s *pAikSlot = NULL;
#endif

    /* task param */
    TaskParam.Ctx.Data = Param;
    /* vin id */
    VinId = TaskParam.Ctx.Bits.VinId;
    /* chain id */
    ChainId = TaskParam.Ctx.Bits.ChainId;

    pPort = &(AmbaImgMainAdj_StatPort[VinId][ChainId]);

    AmbaImgMainAdj_Cmd[VinId][ChainId] = (UINT32) ADJ_TASK_CMD_STOP;

    while (AmbaImgMainAdj_DummyFlag[VinId][ChainId] == 1U) {
        if (AmbaImgMainAdj_Cmd[VinId][ChainId] == (UINT32) ADJ_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
#ifdef AMBA_IMG_MAIN_ADJ_AIK_SYNC
                if ((pCtx != NULL) &&
                    (pCtx->pAikSync != NULL) &&
                    (pAikSlot != NULL)) {
                    /* aik cb unhook */
                    pAikSlot->pFunc = NULL;
                    /* sync sem disable */
                    pAikSlot->En = 0U;
                    /* sync sem delete */
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&(pAikSlot->Sem.Req));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* sem req delete fail */
                    }
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&(pAikSlot->Sem.Ack));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* sem ack delete fail */
                    }
                }
#endif
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                /* efov tx rx cb unhook */
                pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = NULL;
                pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = NULL;
                /* ring mutex */
                if (pEFovInfo != NULL) {
                    FuncRetCode = AmbaKAL_MutexDelete(&(pEFovInfo->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    pEFovInfo = NULL;
                }
#endif
                /* statistics close */
                FuncRetCode = AmbaImgStatistics_Close(pPort);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj statistics close", VinId, FuncRetCode);
                }
            }

            /* task rdy2go */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAdj_Ready[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj task rdy2go", VinId, FuncRetCode);
            }
            /* task wait2go */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAdj_Go[VinId][ChainId], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj task wait2go", VinId, FuncRetCode);
            }
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* ctx check */
            if (pCtx == NULL) {
                AmbaImgMainAdj_Cmd[VinId][ChainId] = (UINT32) ADJ_TASK_CMD_STOP;
                continue;
            }
#ifdef CONFIG_BUILD_IMGFRW_EFOV
            /* hdr info get */
            HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
            ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "hdr enable", VinId, HdrEnable);
            AmbaImgPrint2(PRINT_FLAG_DBG, "exposure num", VinId, ExposureNum);
#endif
            /* image channel get */
            ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
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
                for (m = (((INT32) AMBA_IMG_NUM_FOV_CHANNEL) - 1); m >= 0L; m--) {
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
            AmbaImgMainAdj_Zone2AlgoGet(pCtx, AmbaImgMainAdj_Zone2AlgoTable);

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "vin id", VinId, ImageChanId.Ctx.Bits.VinId);
            AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, ImageChanId.Ctx.Bits.ChainId);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "sensor id", VinId, 10U, ImageChanId.Ctx.Bits.SensorId, 16U);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "vr id", VinId, 10U, pCtx->VrMap.Id.Ctx.Bits.VrId, 16U);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "vr alt id", VinId, 10U, pCtx->VrMap.Id.Ctx.Bits.VrAltId, 16U);
            AmbaImgPrint2(PRINT_FLAG_DBG, "algo id", VinId, AlgoId);
            AmbaImgPrintEx2(PRINT_FLAG_DBG, "zone id", VinId, 10U, ImageChanId.Ctx.Bits.ZoneId, 16U);
            AmbaImgPrint2(PRINT_FLAG_DBG, "statistics index", VinId, StatisticsIndex);

            AmbaImgPrint2(PRINT_FLAG_DBG, "vr enable", VinId, VrEnable);
            AmbaImgPrint2(PRINT_FLAG_DBG, "vr master", VinId, VrMaster);
#ifdef AMBA_IMG_MAIN_ADJ_AIK_SYNC
            if (pCtx->pAikSync != NULL) {
                /* aik slot get */
                pAikSlot = &(pCtx->pAikSync->Slot[AMBA_IMG_MAIN_ADJ_AIK_SLOT]);
                /* sync sem create */
                FuncRetCode = AmbaKAL_SemaphoreCreate(&(pAikSlot->Sem.Req), NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* sem req create fail */
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&(pAikSlot->Sem.Ack), NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* sem ack create fail */
                }
                /* sync sem enable */
                pAikSlot->En = 0x1U;
                /* aik cb hook */
                pAikSlot->pFunc = AmbaImgMainAdj_AikCb;
            }
#endif
#ifdef CONFIG_BUILD_IMGFRW_EFOV
            /* efov tx rx cb hook */
            if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_NONE) {
                /* none */
                pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = NULL;
                pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = NULL;
            } else {
                /* ring get */
                pEFovInfo = &(AmbaImgMainAdj_EFovInfo[VinId][ChainId]);
                /* ring mutex */
                FuncRetCode = AmbaKAL_MutexCreate(&(pEFovInfo->Mutex), NULL);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
                /* callback hook */
                if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                    /* master */
                    pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = AmbaImgMainAdj_EFovTxCb;
                    pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = NULL;
                } else {
                    /* slave */
                    pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = NULL;
                    pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_ADJ] = AmbaImgMainAdj_EFovRxCb;
                }
            }

            AmbaImgPrint2(PRINT_FLAG_DBG, "efov mode", VinId, (UINT32) pCtx->EFov.Ctx.Bits.Mode);
#endif
            /* adj init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "adj algo init...", VinId, AlgoId);
            FuncRetCode = Amba_AdjInit(AlgoId);
            if(FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj algo init", VinId, AlgoId);
            }

            /* exclusive zone id*/
            ExclusiveZoneId = 0U;

            /* avm adj */
            if (AmbaImgMainAvm_ImageChanId.Ctx.Bits.ZoneId > 0U) {
                ExclusiveZoneId |= AmbaImgMainAvm_ImageChanId.Ctx.Bits.ZoneId;
            }

            /* fov adj */
            for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
                ExclusiveZoneId |= AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId;
            }

            /* inclusive zone id */
            InclusiveZoneId = ImageChanId.Ctx.Bits.ZoneId & (~ExclusiveZoneId);

            /* adj preset */
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                if ((InclusiveZoneId >> i) == 0U) {
                    break;
                }
                if ((InclusiveZoneId & (((UINT32) 1U) << i)) > 0U) {
                    ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                    FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                    if (FuncRetCode == OK_UL) {
                        FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                        if (FuncRetCode == OK_UL) {
                            INT32 FuncRetCode_L;
                            ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            AmbaImgPrint2(PRINT_FLAG_DBG, "adj context", i, ImgMode.ContextId);
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            /* tile settings */
                            FuncRetCode_L = App_Image_Init_Ae_Tile_Config(AlgoId, &ImgMode);
                            if (FuncRetCode_L != 0) {
                                /* */
                            }
                            FuncRetCode_L = App_Image_Init_Awb_Tile_Config(AlgoId, &ImgMode);
                            if (FuncRetCode_L != 0) {
                                /* */
                            }
                            FuncRetCode_L = App_Image_Init_Af_Tile_Config(AlgoId, &ImgMode);
                            if (FuncRetCode_L != 0) {
                                /* */
                            }
                            /* video param set */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                            FuncRetCode = AmbaImgProc_ADJSetVideoParam(AlgoId, &ImgMode);
#else
                            FuncRetCode = Amba_Adj_SetVideoPipeCtrlParams(AlgoId, &ImgMode);
#endif
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                    }
                }
            }
            /* adj reset flags */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
            FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 0U);
#else
            FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 0U);
#endif
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* statistic open */
            FuncRetCode = AmbaImgStatistics_Open(ImageChanId, pPort);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj statistics open", VinId, FuncRetCode);
            }

            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAdj_Ack[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj task ack", VinId, FuncRetCode);
            }
        }

        FuncRetCode = AmbaImgStatistics_Request(pPort, REQUEST_WAIT_FOREVER);
        if (FuncRetCode != OK_UL) {
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: adj statistics request", VinId, FuncRetCode);
        } else {
#ifdef CONFIG_BUILD_IMGFRW_EFOV
            /* statistic put */
            if ((pEFovInfo != NULL) &&
                (pCtx != NULL) &&
                (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_SLAVE)) {
                /* mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(pEFovInfo->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    UINT32 Index;
                    /* cfa */
                    MemInfo.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                    Index = MemInfo.Ctx.pCfa->Header.RawPicSeqNum % AMBA_IMG_MAIN_ADJ_EFOV_STAT_NUM;
                    pEFovInfo->Ring.pCfa[Index] = MemInfo.Ctx.pCvoid;
                    /* rgb */
                    MemInfo.Ctx.pCvoid = pPort->Data.pRgb[StatisticsIndex];
                    Index = MemInfo.Ctx.pRgb->Header.RawPicSeqNum % AMBA_IMG_MAIN_ADJ_EFOV_STAT_NUM;
                    pEFovInfo->Ring.pRgb[Index] = MemInfo.Ctx.pCvoid;
                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pEFovInfo->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }
#endif
            /* task stop or statistics null? */
            if ((AmbaImgMainAdj_Cmd[VinId][ChainId] == (UINT32) ADJ_TASK_CMD_STOP) ||
                (AmbaImgMainAdj_Cmd[VinId][ChainId] == (UINT32) ADJ_TASK_CMD_LOCK_DOWN) ||
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                ((pCtx != NULL) && (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_SLAVE)) ||
#endif
                (pPort->Data.pCfa == NULL) ||
                (pPort->Data.pRgb == NULL)) {
                continue;
            }

            /* adj lock/unlock for still */
            switch (AmbaImgMainAdj_Cmd[VinId][ChainId]) {
                case (UINT32) ADJ_TASK_CMD_LOCK:
                    /* adj in lock */
                    /* adj cmd lock proc */
                    AmbaImgMainAdj_Cmd[VinId][ChainId] = (UINT32) ADJ_TASK_CMD_LOCK_PROC;
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "adj lock", VinId, AlgoId);
                    break;
                case (UINT32) ADJ_TASK_CMD_UNLOCK:
                    /* adj in unlock */
                    /* adj cmd start */
                    AmbaImgMainAdj_Cmd[VinId][ChainId] = (UINT32) ADJ_TASK_CMD_START;
                    /* unlock ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAdj_UnlockAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "adj unlock", VinId, AlgoId);
                    break;
                default:
                    /* */
                    break;
            }

            /* adj lockdown */
            if ((AmbaImgMainAdj_Cmd[VinId][ChainId] == (UINT32) ADJ_TASK_CMD_LOCK_PROC)) {
                /* lockdown */
                AmbaImgMainAdj_Cmd[VinId][ChainId] = (UINT32) ADJ_TASK_CMD_LOCK_DOWN;
                /* lock ack */
                FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAdj_LockAck[VinId][ChainId]));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
                /* */
                AmbaImgPrint2(PRINT_FLAG_LOCK, "adj lockdown", VinId, AlgoId);
            }

            /* aaa op get */
            FuncRetCode = AmbaImgProc_GetAAAOPInfo(AlgoId, &AaaOpInfo);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* op enable? */
            if (AaaOpInfo.AdjOp > 0U) {
                if ((AaaOpInfo.AeOp == 0U) &&
                    (AaaOpInfo.AwbOp == 0U)) {
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
                        AmbaImgMainAdj_VrStatProc(pPort, AmbaImgMainAdj_Zone2AlgoTable);
                    }
                    /* debug only */
                    if (AmbaImgMainAdj_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                        if (VrEnable == 0U) {
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "[Adj Statistics]");
                            MemInfo.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "cfa", VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                            MemInfo.Ctx.pCvoid = pPort->Data.pRgb[StatisticsIndex];
                            AmbaImgPrintEx2(PRINT_FLAG_DBG, "rgb", VinId, 10U, (UINT32) MemInfo.Ctx.Data, 16U);
                        }
                    }
                }

                /* adj control */
                Amba_AdjControl(AlgoId);
#ifndef AMBA_IMG_MAIN_ADJ_AIK_SYNC
                /* adj set */
                ImageChanId2.Ctx.Data = pPort->ImageChanId.Ctx.Data;
                for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                    if ((InclusiveZoneId >> i) == 0U) {
                        break;
                    }
                    if ((InclusiveZoneId & (((UINT32) 1U) << i)) > 0U) {
                        ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                        FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                        if (FuncRetCode == OK_UL) {
                            FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                            if (FuncRetCode == OK_UL) {
                                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                                /* debug only */
                                if (AmbaImgMainAdj_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                                    AmbaImgPrint2(PRINT_FLAG_DBG, "adj context", i, ImgMode.ContextId);
                                }
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                                FuncRetCode = AmbaImgProc_ADJSetVideoParam(AlgoId, &ImgMode);
#else
                                FuncRetCode = Amba_Adj_SetVideoPipeCtrlParams(AlgoId, &ImgMode);
#endif
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            }
                        }
                    }
                }
                /* adj reset flags */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 0U);
#else
                FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 0U);
#endif
                if (FuncRetCode != OK_UL) {
                    /* */
                }
#else
                if (AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode != (UINT8) VIDEO_OP_RAWENC) {
                    /* adj aik sync */
                    if ((pCtx != NULL) &&
                        (pCtx->pAikSync != NULL) &&
                        (pAikSlot != NULL)) {
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                        if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                            /* ae awb histo carry */
                            UINT8 Enable;
                            AMBA_IP_GLOBAL_AAA_DATA_s *pAaaData;

                            AMBA_IMG_MAIN_ADJ_EFOV_USER_s *pEFovUser;

                            /* user data get */
                            pEFovUser = &(AmbaImgMainAdj_EFovTxRxData[VinId][ChainId].User);

                            /* ae/awb info get */
                            for (i = 0U; i < ExposureNum; i++) {
                                /* ae info get */
                                FuncRetCode = AmbaImgProc_AEGetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(pEFovUser->AeInfo.Ctx[i]));
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                                /* awb info get */
                                FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_VIDEO, &(pEFovUser->WbInfo.Ctx[i]));
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            }

                            /* cfa get */
                            MemInfo.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                            /* raw cap seq get */
                            pEFovUser->AaaStat.RawCapSeq = MemInfo.Ctx.pCfa->Header.RawPicSeqNum;

                            /* histo info get */
                            FuncRetCode = AmbaImgProc_GAAAGetEnb(AlgoId, &Enable);
                            if (FuncRetCode == OK) {
                                if (Enable == 0U) {
                                    /* cfa histo */
                                    for (i = 0U; i < AMBA_IK_CFA_HISTO_COUNT; i++) {
                                        pEFovUser->AaaStat.Histo.BinY[i] = MemInfo.Ctx.pCfa->Histogram.HisBinY[i];
                                    }
                                } else {
                                    /* global aaa histo */
                                    FuncRetCode = AmbaImgProc_GAAAGetStat(AlgoId, &pAaaData);
                                    if (FuncRetCode == OK) {
                                        for (i = 0U; i < AMBA_IK_CFA_HISTO_COUNT; i++) {
                                            pEFovUser->AaaStat.Histo.BinY[i] = pAaaData->CfaHist.HisBinY[i];
                                        }
                                    }
                                }
                            }
                        }
#endif
                        /* data put */
                        pAikSlot->Msg[0] = InclusiveZoneId;
                        pAikSlot->Msg[1] = AlgoId;
                        pAikSlot->pName = "Adj_Param";
                        /* adj req put */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(pAikSlot->Sem.Req));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                        /* adj ack get */
                        FuncRetCode = AmbaKAL_SemaphoreTake(&(pAikSlot->Sem.Ack), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                        /* debug only */
                        if (AmbaImgMainAdj_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                            ImageChanId2.Ctx.Data = pPort->ImageChanId.Ctx.Data;
                            for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                                if ((InclusiveZoneId >> i) == 0U) {
                                    break;
                                }
                                if ((InclusiveZoneId & (((UINT32) 1U) << i)) > 0U) {
                                    ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                                    FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                                    if (FuncRetCode == OK_UL) {
                                        FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                                        if (FuncRetCode == OK_UL) {
                                            ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                                            AmbaImgPrint2(PRINT_FLAG_DBG, "adj context", i, ImgMode.ContextId);
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    /* adj set */
                    ImageChanId2.Ctx.Data = pPort->ImageChanId.Ctx.Data;
                    for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                        if ((InclusiveZoneId >> i) == 0U) {
                            break;
                        }
                        if ((InclusiveZoneId & (((UINT32) 1U) << i)) > 0U) {
                            ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                            FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                            if (FuncRetCode == OK_UL) {
                                FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                                if (FuncRetCode == OK_UL) {
                                    ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                                    /* debug only */
                                    if (AmbaImgMainAdj_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                                        AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                                        AmbaImgPrint2(PRINT_FLAG_DBG, "adj context", i, ImgMode.ContextId);
                                    }
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                                    FuncRetCode = AmbaImgProc_ADJSetVideoParam(AlgoId, &ImgMode);
#else
                                    FuncRetCode = Amba_Adj_SetVideoPipeCtrlParams(AlgoId, &ImgMode);
#endif
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                }
                            }
                        }
                    }
                    /* adj reset flags */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                    FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 0U);
#else
                    FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 0U);
#endif
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
#endif
            }
            /* debug only */
            if (AmbaImgMainAdj_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                /* count accumulate */
                AmbaImgMainAdj_DebugCount[VinId][ChainId]++;
                if (AmbaImgMainAdj_DebugCount[VinId][ChainId] == AmbaImgMain_DebugCount) {
                    /* count reset */
                    AmbaImgMainAdj_DebugCount[VinId][ChainId] = 0xFFFFFFFFU;
                }
            }
        }
    }
}

/**
 *  Amba image main adj aaa algorithm id get
 *  @param[in] ImageChanId image channel id
 *  @return algorithm id
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAdj_AaaAlgoIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
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
 *  @private
 *  Amba image main adj snap
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] SnapCount snap count
 *  @param[out] pIsHiso pointer to the high iso enable flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAdj_SnapAebCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
UINT32 AmbaImgMainAdj_Snap(UINT32 VinId, UINT32 ChainId, UINT32 SnapCount, const UINT32 *pIsHiso)
{
    UINT32 RetCode = OK_UL;

    (void) SnapCount;

    if(pIsHiso == NULL) {
        RetCode = NG_UL;
    }

    AmbaImgMainAdj_SnapAebCount[VinId][ChainId] = 0U;

    return RetCode;
}

/**
 *  @private
 *  Amba image main adj snap dsp filter
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] pCfa pointer to the CFA statistics data
 *  @param[in] IsPiv piv enable flag
 *  @param[in] IsHiso high iso enable flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAdj_SnapDspFilter(UINT32 VinId, UINT32 ChainId, const AMBA_IK_CFA_3A_DATA_s *pCfa, UINT32 IsPiv, UINT32 IsHiso)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 HdrEnable;
    UINT32 ExposureNum;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    AMBA_IK_MODE_CFG_s *pImgMode = NULL;

    UINT32 AlgoId;

    UINT32 AebIndex;

    (void) pCfa;

    if ((pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL) {
            /* hdr info get */
            HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
            ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;
            /* debug msg */
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "still adj iso", VinId, ChainId);
            AmbaImgPrint2(PRINT_FLAG_DBG, "hdr enable", VinId, HdrEnable);
            AmbaImgPrint2(PRINT_FLAG_DBG, "exposure num", VinId, ExposureNum);

            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* image channel get */
            ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
            /* zone id get */
            ImageChanId.Ctx.Bits.ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId);

            /* algo id get */
            AlgoId = AmbaImgMainAdj_AaaAlgoIdGet(ImageChanId);

            /* still image mode set */
            FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId, AMBA_IK_PIPE_STILL, &pImgMode);
            if (FuncRetCode == OK_UL) {
                /* debug msg */
                AmbaImgPrint(PRINT_FLAG_DBG, "still adj context id", pImgMode->ContextId);

                if (IsPiv == 1U) {
                    /* piv (r2y), evaluated only */
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
                    if (IsHiso > 0U) {
                        /* hiso (TBD) */
                        Amba_AdjPivControl(AlgoId, IP_MODE_HISO_STILL);
                    } else {
                        /* liso (TBD) */
                        Amba_AdjPivControl(AlgoId, IP_MODE_LISO_STILL);
                    }
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                    FuncRetCode = AmbaImgProc_ADJSetStillParam(AlgoId, 0U, pImgMode);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* flag reset */
                    FuncRetCode = AmbaImgProc_ADJResetStillFlags(AlgoId, 0U/*iso*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#else
                    FuncRetCode = Amba_Adj_SetStillPipeCtrlParams(AlgoId, 0U, pImgMode);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* flag reset */
                    FuncRetCode = Amba_Adj_ResetStillFlags(AlgoId, 0U/*iso*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#endif
#endif
                } else {
                    /* aeb index get */
                    AebIndex = AmbaImgMainAdj_SnapAebCount[VinId][ChainId];
                    /* still */
                    if (IsHiso > 0U) {
                        /* hiso */
                        Amba_AdjStillControl(AlgoId, AebIndex, IP_MODE_HISO_STILL);
                    } else {
                        /* liso */
                        Amba_AdjStillControl(AlgoId, AebIndex, IP_MODE_LISO_STILL);
                    }
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                    /* params setting */
                    FuncRetCode = AmbaImgProc_ADJSetStillParam(AlgoId, AebIndex, pImgMode);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* flag reset */
                    FuncRetCode = AmbaImgProc_ADJResetStillFlags(AlgoId, 0U/*iso*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#else
                    /* params setting */
                    FuncRetCode = Amba_Adj_SetStillPipeCtrlParams(AlgoId, AebIndex, pImgMode);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* flag reset */
                    FuncRetCode = Amba_Adj_ResetStillFlags(AlgoId, 0U/*iso*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#endif
                    /* aeb index next */
                    if ((pCtx->Snap.Ctx.Bits.AebId > 0U) && (IsPiv == 0U)) {
                        AmbaImgMainAdj_SnapAebCount[VinId][ChainId]++;
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

