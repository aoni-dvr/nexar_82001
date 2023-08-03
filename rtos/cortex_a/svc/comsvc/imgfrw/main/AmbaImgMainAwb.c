/**
 *  @file AmbaImgMainAwb.c
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
 *  @details Amba Image Main Awb
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

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainAwb.h"
#ifdef CONFIG_BUILD_IMGFRW_EFOV
#include "AmbaImgMainEFov.h"
#endif
#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_AWB_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR       Data;
    void                         *pVoid;
    const void                   *pCvoid;
    AMBA_IK_CFA_3A_DATA_s        *pCfa;
    AMBA_IK_PG_3A_DATA_s         *pRgb;
    AMBA_IP_GLOBAL_AAA_DATA_s    *pAaaData;
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    AMBA_IMG_MAIN_AWB_EFOV_USER_s    *pEFovUser;
    AMBA_IMG_MAIN_AWB_EFOV_TXRX_s    *pTxRx;
#endif
} AWB_MEM_INFO_u;

typedef struct /*_AWB_MEM_INFO_s_*/ {
    AWB_MEM_INFO_u    Ctx;
} AWB_MEM_INFO_s;

typedef union /*_AWB_TASK_PARAM_u_*/ {
    UINT32 Data;
    struct {
        UINT32 VinId:    16;
        UINT32 ChainId:  16;
    } Bits;
} AWB_TASK_PARAM_u;

typedef struct /*_AWB_TASK_PARAM_s_*/ {
    AWB_TASK_PARAM_u    Ctx;
} AWB_TASK_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_AWB_TASK_INFO_s_*/ {
    char Name[32];
} AMBA_IMG_MAIN_AWB_TASK_INFO_s;

#define AMBA_IMG_MAIN_AWB_AIK_SYNC
#ifdef AMBA_IMG_MAIN_AWB_AIK_SYNC
#define AMBA_IMG_MAIN_AWB_AIK_SLOT IMG_CHAN_AIK_SLOT_AWB
#endif

#ifdef CONFIG_BUILD_IMGFRW_EFOV
static AMBA_IMG_MAIN_AWB_EFOV_TXRX_s GNU_SECTION_NOZEROINIT AmbaImgMainAwb_EFovTxRxData[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
#endif

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainAwbTaskId[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static void AmbaImgMainAwb_Task(UINT32 Param);
static void *AmbaImgMainAwb_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAwb_Ready[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAwb_Go[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAwb_Ack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAwb_LockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainAwb_UnlockAck[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAwb_Cmd[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

static UINT32 AmbaImgMainAwb_DummyFlag[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAwb_DebugCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAwb_VinNum;

/**
 *  @private
 *  Amba image main awb create
 *  @param[in] VinId vin id
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_AWB_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainAwb_TaskStack[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO][AMBA_IMG_MAIN_AWB_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_AWB_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainAwb_TaskInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;
    UINT32 VrId;
    UINT32 VrAltId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    AWB_TASK_PARAM_s ParamInfo;

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
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAwb_Ready[VinId][i], NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAwb_Go[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAwb_Ack[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAwb_LockAck[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainAwb_UnlockAck[VinId][i], NULL, 0);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* task param */
                ParamInfo.Ctx.Bits.VinId = (UINT8) VinId;
                ParamInfo.Ctx.Bits.ChainId = (UINT8) i;
                /* task name */
                {
                    char str[11];
                    UINT32 VinStrNum = (AmbaImgMainAwb_VinNum > 9U) ? (UINT32) 2U : (UINT32) 1U;
                    AmbaImgMainAwb_TaskInfo[VinId][i].Name[0] = '\0';
                    (void) var_strcat(AmbaImgMainAwb_TaskInfo[VinId][i].Name, "ImageVin");
                    var_utoa(VinId, str, 10U, VinStrNum, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAwb_TaskInfo[VinId][i].Name, str);
                    (void) var_strcat(AmbaImgMainAwb_TaskInfo[VinId][i].Name, "Awb");
                    var_utoa(i, str, 10U, 1U, (UINT32) VAR_LEADING_ZERO);
                    (void) var_strcat(AmbaImgMainAwb_TaskInfo[VinId][i].Name, str);
                }
                /* task */
                if (RetCode == OK) {
                    AmbaImgPrint2(PRINT_FLAG_MSG, "image create awb task", VinId, i);
                    {
                        AWB_MEM_INFO_s MemInfo;
                        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) ParamInfo.Ctx.Data;
                        FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainAwbTaskId[VinId][i],
                                                         AmbaImgMainAwb_TaskInfo[VinId][i].Name,
                                                         Priority,
                                                         AmbaImgMainAwb_TaskEx,
                                                         MemInfo.Ctx.pVoid,
                                                         AmbaImgMainAwb_TaskStack[VinId][i],
                                                         AMBA_IMG_MAIN_AWB_TASK_STACK_SIZE,
                                                         AMBA_KAL_DONT_START);
                    }
                    if (FuncRetCode == OK) {
                        UINT32 AwbCoreInclusion = CoreInclusion;
                        if (AmbaImgMain_ImgAaaCore[VinId][i] > 0U) {
                            AwbCoreInclusion = AmbaImgMain_ImgAaaCore[VinId][i];
                        }
                        FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainAwbTaskId[VinId][i], AwbCoreInclusion);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainAwbTaskId[VinId][i]);
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
 *  Amba image main awb delete
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_Delete(UINT32 VinId)
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
        for (i = 0U; pAmbaImgMainChannel[VinId][i].Magic == 0xCafeU ; i++) {
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
                FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainAwbTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainAwbTaskId[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* semaphore */
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAwb_Ready[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAwb_Go[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAwb_Ack[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAwb_LockAck[VinId][i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainAwb_UnlockAck[VinId][i]);
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
 *  Amba image main awb init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 i, j;

    /* awb task dummy flag init */
    for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
        for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
            AmbaImgMainAwb_DummyFlag[i][j] = 1U;
        }
    }
    /* awb debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
                AmbaImgMainAwb_DebugCount[i][j] = 0xFFFFFFFFU;
            }
        }
    }

    AmbaImgMainAwb_VinNum = AMBA_IMG_NUM_VIN_CHANNEL;

    return RetCode;
}

/**
 *  @private
 *  Amba image main awb active
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_Active(UINT32 VinId)
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
                if (AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_STOP) {
                    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAwb_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* cmd start */
                        AmbaImgMainAwb_Cmd[VinId][i] = (UINT32) AWB_TASK_CMD_START;
                        /* task go */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAwb_Go[VinId][i]);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* ack take */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAwb_Ack[VinId][i], AMBA_KAL_WAIT_FOREVER);
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
 *  Amba image main awb inactive
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_Inactive(UINT32 VinId)
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
                if ((AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_START) ||
                    (AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_LOCK_DOWN)) {
                    AmbaImgMainAwb_Cmd[VinId][i] = (UINT32) AWB_TASK_CMD_STOP;
#ifdef AMBA_IMG_MAIN_AWB_AIK_SYNC
                    if ((pAmbaImgMainChannel[VinId][i].pCtx->pAikSync != NULL) &&
                        (pAmbaImgMainChannel[VinId][i].pCtx->pAikSync->Slot[AMBA_IMG_MAIN_AWB_AIK_SLOT].En > 0U)) {
                        UINT32 FuncRetCode;
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(pAmbaImgMainChannel[VinId][i].pCtx->pAikSync->Slot[AMBA_IMG_MAIN_AWB_AIK_SLOT].Sem.Ack));
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
 *  Amba image main awb idle wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_IdleWait(UINT32 VinId)
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
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAwb_Ready[VinId][i], AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAwb_Ready[VinId][i]);
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
 *  Amba image main awb lock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_Lock(UINT32 VinId)
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
                if (AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_START) {
                    /* lock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAwb_LockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* lock cmd */
                    AmbaImgMainAwb_Cmd[VinId][i] = (UINT32) AWB_TASK_CMD_LOCK;
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
 *  Amba image main awb lock wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_LockWait(UINT32 VinId)
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
                if ((AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_LOCK) ||
                    (AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_LOCK_DOWN)) {
                    /* lock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAwb_LockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgPrint2(PRINT_FLAG_LOCK, "awb lock ack", VinId, (UINT32) ImageChanId.Ctx.Bits.AlgoId);
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
 *  Amba image main awb unlock
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_Unlock(UINT32 VinId)
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
                if ((AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_LOCK) ||
                    (AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_LOCK_PROC) ||
                    (AmbaImgMainAwb_Cmd[VinId][i] == (UINT32) AWB_TASK_CMD_LOCK_DOWN)) {
                    /* unlock ack reset */
                    while (AmbaKAL_SemaphoreTake(&(AmbaImgMainAwb_UnlockAck[VinId][i]), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                    /* unlock cmd */
                    AmbaImgMainAwb_Cmd[VinId][i] = (UINT32) AWB_TASK_CMD_UNLOCK;
                    /* unlock wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaImgMainAwb_UnlockAck[VinId][i]), AMBA_KAL_WAIT_FOREVER);
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
 *  Amba image main awb external fov tx cb
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] TxData tx data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAwb_EFovTxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 TxData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    AWB_MEM_INFO_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;

    /* msg get */
    Msg2.Ctx.Data = Msg;
    /* tx data get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) TxData;

    if (Msg2.Ctx.Bits2.Awb > 0U) {
        /* msg put */
        MemInfo.Ctx.pTxRx->Msg = Msg2.Ctx.Data;
        /* user get */
        MemInfo.Ctx.pEFovUser = &(MemInfo.Ctx.pTxRx->User);
        /* efov tx awb put */

        /* efov tx awb write */
        FuncRetCode = AmbaImgMainEFov_Tx(ImageChanId, TxData, (UINT32) sizeof(AMBA_IMG_MAIN_AWB_EFOV_TXRX_s));
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }

        /* debug msg */
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_W");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_W!");
        }
    }

    return RetCode;
}

/**
 *  Amba image main awb external fov rx cb
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] RxData rx data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAwb_EFovRxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 RxData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 AlgoId;

    UINT32 HdrEnable;

    UINT32 WbPosition;
    UINT32 WbId = (UINT32) WB_ID_FE;
    UINT32 Mode = AMBA_IK_PIPE_VIDEO;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    AWB_MEM_INFO_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* msg get */
    Msg2.Ctx.Data = Msg;
    /* rx data get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) RxData;

    if (Msg2.Ctx.Bits2.Awb > 0U) {
        /* algo id get */
        AlgoId = ImageChanId.Ctx.Bits.AlgoId;
        /* user get */
        MemInfo.Ctx.pEFovUser = &(MemInfo.Ctx.pTxRx->User);
        /* hdr info get */
        HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
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

        /* awb update */
        if (HdrEnable == 0U) {
            UINT32 Wgc[3];
            /* wb settle */
            Wgc[0] = MemInfo.Ctx.pEFovUser->WbInfo.Ctx.GainR;
            Wgc[1] = MemInfo.Ctx.pEFovUser->WbInfo.Ctx.GainG;
            Wgc[2] = MemInfo.Ctx.pEFovUser->WbInfo.Ctx.GainB;
            FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId, Mode | WbId, Wgc);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }

        /* debug msg */
        AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Rx_W");
    }

    return RetCode;
}
#endif
#ifdef AMBA_IMG_MAIN_AWB_AIK_SYNC
/**
 *  Amba image main awb aik callback
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAwb_AikCb(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
    AMBA_IMG_CHANNEL_AIK_SYNC_s *pAikSync;
    AMBA_IMG_CHANNEL_AIK_SLOT_s *pAikSlot;

    UINT32 Mode;
    UINT32 Wgc[3];

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
                    pAikSlot = &(pAikSync->Slot[AMBA_IMG_MAIN_AWB_AIK_SLOT]);
                    /* aik slot enable? */
                    if (pAikSlot->En > 0U) {
                        /* req? */
                        FuncRetCode = AmbaKAL_SemaphoreTake(&(pAikSlot->Sem.Req), AMBA_KAL_NO_WAIT);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* mode get */
                            Mode = pAikSlot->Msg[0];
                            /* wgc get */
                            Wgc[0] = pAikSlot->Msg[1];
                            Wgc[1] = pAikSlot->Msg[2];
                            Wgc[2] = pAikSlot->Msg[3];
                            /* wgc write */
                            FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId, Mode, Wgc);
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
                                str[0] = 'f';str[1] = ' ';
                                var_utoa(ImageChanId.Ctx.Bits.ZoneId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }
                            /* awb ack */
                            FuncRetCode = AmbaKAL_SemaphoreGive(&(pAikSlot->Sem.Ack));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                            /* efov awb tx cb */
                            if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                                AWB_MEM_INFO_s MemInfo;
                                AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;
                                Msg2.Ctx.Data = 0ULL;
                                Msg2.Ctx.Bits2.Awb = 1U;
                                MemInfo.Ctx.pTxRx = &(AmbaImgMainAwb_EFovTxRxData[VinId][ChainId]);
                                /* efov awb tx */
                                (void) pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AWB](pCtx->Id, Msg2.Ctx.Data, (UINT64) MemInfo.Ctx.Data);
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
 *  Amba image main awb zone to algorithm channel get
 *  @param[in] pCtx pointer to the image channel context data
 *  @param[out] pZone2AlgoTable pointer to the zone to algorithm channel table
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAwb_Zone2AlgoGet(const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx, UINT32 *pZone2AlgoTable)
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
                pZone2AlgoTable[k] =  (UINT32) pCtx->Id.Ctx.Bits.AlgoId;
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
                                pZone2AlgoTable[k] = (UINT32) ImageChanId2.Ctx.Bits.AlgoId;
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
 *  Amba image main awb vr statistics processing
 *  @param[in] pPort pointer to the statistics port data
 *  @param[in] pZone2AlgoTable pointer to the zone to algorithm channel table
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAwb_VrStatProc(const AMBA_IMG_STATISTICS_PORT_s *pPort, const UINT32 *pZone2AlgoTable)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 AlgoId = pPort->ImageChanId.Ctx.Bits.AlgoId;
    UINT32 StatZoneId = pPort->ZoneId.Req;

    UINT32 StatCnt = 0U;

    UINT32 TileRow;
    UINT32 TileCol;

    AWB_MEM_INFO_s MemInfo;
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
        if (AmbaImgMainAwb_DebugCount[pPort->ImageChanId.Ctx.Bits.VinId][pPort->ImageChanId.Ctx.Bits.ChainId] < AmbaImgMain_DebugCount) {
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
 *  Amba image main awb task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainAwb_TaskEx(void *pParam)
{
    AWB_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainAwb_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main awb task
 *  @param[in] Param vin id with the chain id
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainAwb_Task(UINT32 Param)
{
    static UINT32 AmbaImgMainAwb_Zone2AlgoTable[32] = {0};

    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;
     INT32 m;

    AWB_MEM_INFO_s MemInfo;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    static AMBA_IMG_STATISTICS_PORT_s AmbaImgMainAwb_StatPort[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO] = {0};
    AMBA_IMG_STATISTICS_PORT_s *pPort;

    AWB_TASK_PARAM_s TaskParam;
    UINT32 VinId;
    UINT32 ChainId;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 VrEnable = 0U;
    UINT32 VrMaster = 0U;

    AMBA_IK_WB_GAIN_s WbGain[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};
    UINT32 Wgc[3];
    UINT32 LastWgc[3];

    UINT32 AlgoId = 0U;
    UINT32 StatisticsIndex = 0U;

    AMBA_3A_STATUS_s VideoStatus;
    AMBA_3A_STATUS_s StillStatus;

    UINT32 HdrEnable = 0U;
    UINT32 ExposureNum = 1U;

    AMBA_AAA_OP_INFO_s AaaOpInfo;

    UINT32 ZoneId;

    AMBA_IMG_CHANNEL_CONTEXT_s *pCtx = NULL;
#ifdef AMBA_IMG_MAIN_AWB_AIK_SYNC
    AMBA_IMG_CHANNEL_AIK_SLOT_s *pAikSlot = NULL;
#endif
    UINT32 WbPosition = 0U;
    UINT32 WbId = (UINT32) WB_ID_FE;
    UINT32 Mode = AMBA_IK_PIPE_VIDEO;

    /* task param */
    TaskParam.Ctx.Data = Param;
    /* vin id */
    VinId = TaskParam.Ctx.Bits.VinId;
    /* chain id */
    ChainId = TaskParam.Ctx.Bits.ChainId;

    /* param init */
    ImageChanId.Ctx.Data = 0ULL;

    /* param reset */
    FuncRetCode = AmbaWrap_memset(LastWgc, 0, sizeof(LastWgc));
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

    pPort = &(AmbaImgMainAwb_StatPort[VinId][ChainId]);

    AmbaImgMainAwb_Cmd[VinId][ChainId] = (UINT32) AWB_TASK_CMD_STOP;

    while (AmbaImgMainAwb_DummyFlag[VinId][ChainId] == 1U) {
        if (AmbaImgMainAwb_Cmd[VinId][ChainId] == (UINT32) AWB_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
#ifdef AMBA_IMG_MAIN_AWB_AIK_SYNC
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
                pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AWB] = NULL;
                pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AWB] = NULL;
#endif
                /* statistics close */
                FuncRetCode = AmbaImgStatistics_Close(pPort);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb statistics close", VinId, FuncRetCode);
                }
            }

            /* task rdy2go */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAwb_Ready[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb task rdy2go", VinId, FuncRetCode);
            }
            /* task wait2go */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainAwb_Go[VinId][ChainId], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb task wait2go", VinId, FuncRetCode);
            }

            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* ctx check */
            if (pCtx == NULL) {
                AmbaImgMainAwb_Cmd[VinId][ChainId] = (UINT32) AWB_TASK_CMD_STOP;
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
            AmbaImgMainAwb_Zone2AlgoGet(pCtx, AmbaImgMainAwb_Zone2AlgoTable);

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
#ifdef AMBA_IMG_MAIN_AWB_AIK_SYNC
            if (pCtx->pAikSync != NULL) {
                /* aik slot get */
                pAikSlot = &(pCtx->pAikSync->Slot[AMBA_IMG_MAIN_AWB_AIK_SLOT]);
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
                pAikSlot->pFunc = AmbaImgMainAwb_AikCb;
            }
#endif
#ifdef CONFIG_BUILD_IMGFRW_EFOV
            /* efov tx rx cb hook */
            if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_NONE) {
                /* none */
                pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AWB] = NULL;
                pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AWB] = NULL;
            } else {
                if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                    /* master */
                    pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AWB] = AmbaImgMainAwb_EFovTxCb;
                    pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AWB] = NULL;
                } else {
                    /* slave */
                    pAmbaImgMainChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AWB] = NULL;
                    pAmbaImgMainChannel[VinId][ChainId].EFovRxCb[IMG_CHAN_EFOV_CB_ID_AWB] = AmbaImgMainAwb_EFovRxCb;
                }
            }

            AmbaImgPrint2(PRINT_FLAG_DBG, "efov mode", VinId, (UINT32) pCtx->EFov.Ctx.Bits.Mode);
#endif
            /* algo status reset (awb) */
            VideoStatus.Awb = AMBA_IDLE;
            StillStatus.Awb = AMBA_IDLE;
            FuncRetCode = AmbaImgProc_SetAWBStatus(AlgoId, VideoStatus.Awb, StillStatus.Awb);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* awb init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "awb algo init...", VinId, AlgoId);
            FuncRetCode = AmbaImgProc_AWBInit(AlgoId, 0U);
            if(FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb algo init", VinId, AlgoId);
            }

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
            AmbaImgPrint2(PRINT_FLAG_DBG, "wb position", VinId, WbPosition);
            AmbaImgPrint2(PRINT_FLAG_DBG, "wb id", VinId, WbId);

            /* awb info get */
            FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, 0, IP_MODE_VIDEO, WbGain);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* fe wb update */
            Wgc[0] = WbGain[0].GainR;
            Wgc[1] = WbGain[0].GainG;
            Wgc[2] = WbGain[0].GainB;
            if (HdrEnable == 0U) {
                FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId, Mode | (((UINT32) WbId) << 16U), Wgc);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* last wb */
            LastWgc[0] = Wgc[0];
            LastWgc[1] = Wgc[1];
            LastWgc[2] = Wgc[2];

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "wgc[R]", VinId, Wgc[0]);
            AmbaImgPrint2(PRINT_FLAG_DBG, "wgc[G]", VinId, Wgc[1]);
            AmbaImgPrint2(PRINT_FLAG_DBG, "wgc[B]", VinId, Wgc[2]);
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");

            /* statistics open */
            FuncRetCode = AmbaImgStatistics_Open(ImageChanId, pPort);
            if (FuncRetCode != OK_UL) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb statistics open", VinId, FuncRetCode);
            }

            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainAwb_Ack[VinId][ChainId]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb task ack", VinId, FuncRetCode);
            }
        }

        FuncRetCode = AmbaImgStatistics_Request(pPort, REQUEST_WAIT_FOREVER);
        if (FuncRetCode != OK_UL) {
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: awb statistics request", VinId, FuncRetCode);
        } else {
            /* task stop or statistics null? */
            if ((AmbaImgMainAwb_Cmd[VinId][ChainId] == (UINT32) AWB_TASK_CMD_STOP) ||
                (AmbaImgMainAwb_Cmd[VinId][ChainId] == (UINT32) AWB_TASK_CMD_LOCK_DOWN) ||
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                ((pCtx != NULL) && (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_SLAVE)) ||
#endif
                (pPort->Data.pCfa == NULL) ||
                (pPort->Data.pRgb == NULL)) {
                continue;
            }

            /* awb lock/unlock for still */
            switch (AmbaImgMainAwb_Cmd[VinId][ChainId]) {
                case (UINT32) AWB_TASK_CMD_LOCK:
                    /* awb in lock */
                    /* awb cmd lock proc */
                    AmbaImgMainAwb_Cmd[VinId][ChainId] = (UINT32) AWB_TASK_CMD_LOCK_PROC;
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "awb lock", VinId, AlgoId);
                    break;
                case (UINT32) AWB_TASK_CMD_UNLOCK:
                    /* awb in unlock */
                    VideoStatus.Awb = AMBA_IDLE;
                    StillStatus.Awb = AMBA_IDLE;
                    FuncRetCode = AmbaImgProc_SetAWBStatus(AlgoId, VideoStatus.Awb, StillStatus.Awb);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* awb cmd start */
                    AmbaImgMainAwb_Cmd[VinId][ChainId] = (UINT32) AWB_TASK_CMD_START;
                    /* unlock ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAwb_UnlockAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "awb unlock", VinId, AlgoId);
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
            if (AaaOpInfo.AwbOp > 0U) {
                if (AaaOpInfo.AeOp == 0U) {
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
                        AmbaImgMainAwb_VrStatProc(pPort, AmbaImgMainAwb_Zone2AlgoTable);
                    }
                    /* debug only */
                    if (AmbaImgMainAwb_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                        if (VrEnable == 0U) {
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "[Awb Statistics]");
                            MemInfo.Ctx.pCvoid = pPort->Data.pCfa[StatisticsIndex];
                            AmbaImgPrint64Ex2(PRINT_FLAG_DBG, "cfa", (UINT64) VinId, 10U, (UINT64) MemInfo.Ctx.Data, 16U);
                            MemInfo.Ctx.pCvoid = pPort->Data.pRgb[StatisticsIndex];
                            AmbaImgPrint64Ex2(PRINT_FLAG_DBG, "rgb", (UINT64) VinId, 10U, (UINT64) MemInfo.Ctx.Data, 16U);
                        }
                    }
                }
                /* aaa status get */
                FuncRetCode = AmbaImgProc_GetAAAStatus(AlgoId, &VideoStatus, &StillStatus);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* awb control */
                FuncRetCode = AmbaImgProc_AWBControl(AlgoId, &VideoStatus, &StillStatus);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* awb status update */
                FuncRetCode = AmbaImgProc_SetAWBStatus(AlgoId, VideoStatus.Awb, StillStatus.Awb);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* awb lockdown */
            if (AmbaImgMainAwb_Cmd[VinId][ChainId] == (UINT32) AWB_TASK_CMD_LOCK_PROC) {
                /* mwb? */
                if (AaaOpInfo.AwbOp == 0U) {
                    /* mwb lock */
                    VideoStatus.Awb = AMBA_LOCK;
                    StillStatus.Awb = AMBA_LOCK;
                    /* mwb status update */
                    FuncRetCode = AmbaImgProc_SetAWBStatus(AlgoId, VideoStatus.Awb, StillStatus.Awb);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
                /* awb/mwb lock? */
                if (StillStatus.Awb == AMBA_LOCK) {
                    /* lockdown */
                    AmbaImgMainAwb_Cmd[VinId][ChainId] = (UINT32) AWB_TASK_CMD_LOCK_DOWN;
                    /* lock ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaImgMainAwb_LockAck[VinId][ChainId]));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "awb lockdown", VinId, AlgoId);
                } else {
                    /* lock loop */
                    AmbaImgPrint2(PRINT_FLAG_LOCK, "awb lock...", VinId, AlgoId);
                }
            }

            /* awb info get */
            FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, 0U, IP_MODE_VIDEO, WbGain);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* wb update */
            Wgc[0] = WbGain[0].GainR;
            Wgc[1] = WbGain[0].GainG;
            Wgc[2] = WbGain[0].GainB;

            if (HdrEnable == 0U) {
                if ((Wgc[0] != LastWgc[0]) ||
                    (Wgc[1] != LastWgc[1]) ||
                    (Wgc[2] != LastWgc[2])) {
#ifndef AMBA_IMG_MAIN_AWB_AIK_SYNC
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId, Mode | (((UINT32) WbId) << 16U), Wgc);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#else
                    if (AmbaImgSensorHAL_AuxInfo[VinId].Ctx.Bits.OpMode != (UINT8) VIDEO_OP_RAWENC) {
                        /* awb aik sync */
                        if ((pCtx != NULL) &&
                            (pCtx->pAikSync != NULL) &&
                            (pAikSlot != NULL)) {
#ifdef CONFIG_BUILD_IMGFRW_EFOV
                            /* awb carry */
                            if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                                AmbaImgMainAwb_EFovTxRxData[VinId][ChainId].User.WbInfo.Ctx.GainR = Wgc[0];
                                AmbaImgMainAwb_EFovTxRxData[VinId][ChainId].User.WbInfo.Ctx.GainG = Wgc[1];
                                AmbaImgMainAwb_EFovTxRxData[VinId][ChainId].User.WbInfo.Ctx.GainB = Wgc[2];
                            }
#endif
                            /* data put */
                            pAikSlot->Msg[0] = Mode | (((UINT32) WbId) << 16U);
                            pAikSlot->Msg[1] = Wgc[0];
                            pAikSlot->Msg[2] = Wgc[1];
                            pAikSlot->Msg[3] = Wgc[2];
                            pAikSlot->pName = "Awb_Param";
                            /* awb req put */
                            FuncRetCode = AmbaKAL_SemaphoreGive(&(pAikSlot->Sem.Req));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                            /* awb ack get */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&(pAikSlot->Sem.Ack), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    } else {
                        /* awb set */
                        FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId, Mode | (((UINT32) WbId) << 16U), Wgc);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
#endif
                }
            }

            /* last wb */
            LastWgc[0] = Wgc[0];
            LastWgc[1] = Wgc[1];
            LastWgc[2] = Wgc[2];

            /* debug only */
            if (AmbaImgMainAwb_DebugCount[VinId][ChainId] < AmbaImgMain_DebugCount) {
                /* awb parameter */
                AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                AmbaImgPrint2(PRINT_FLAG_DBG, "wgc[R]", VinId, Wgc[0]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "wgc[G]", VinId, Wgc[1]);
                AmbaImgPrint2(PRINT_FLAG_DBG, "wgc[B]", VinId, Wgc[2]);
                /* count accumulate */
                AmbaImgMainAwb_DebugCount[VinId][ChainId]++;
                if (AmbaImgMainAwb_DebugCount[VinId][ChainId] == AmbaImgMain_DebugCount) {
                    /* count reset */
                    AmbaImgMainAwb_DebugCount[VinId][ChainId] = 0xFFFFFFFFU;
                }
            }
        }
    }
}

/**
 *  Amba image main awb aaa algorithm id get
 *  @param[in] ImageChanId image channel id
 *  @return algorithm id
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainAwb_AaaAlgoIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
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
 *  Amba image main awb snap
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] SnapCount snap count
 *  @param[out] pIsHiso pointer to the high iso enable flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainAwb_SnapAebCount[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
UINT32 AmbaImgMainAwb_Snap(UINT32 VinId, UINT32 ChainId, UINT32 SnapCount, const UINT32 *pIsHiso)
{
    UINT32 RetCode = OK_UL;

    (void) SnapCount;
    (void) pIsHiso;

    AmbaImgMainAwb_SnapAebCount[VinId][ChainId] = 0U;

    return RetCode;
}

/**
 *  @private
 *  Amba image main awb snap dsp white balance gain
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id
 *  @param[in] pCfa pointer to the CFA statistics data
 *  @param[in] IsPiv piv enable flag
 *  @param[in] IsHiso high iso enable flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAwb_SnapDspWgc(UINT32 VinId, UINT32 ChainId, const AMBA_IK_CFA_3A_DATA_s *pCfa, UINT32 IsPiv, UINT32 IsHiso)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 HdrEnable;
    UINT32 ExposureNum;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    UINT32 AlgoId;

    AMBA_AAA_OP_INFO_s AAAOpInfo = {0};
    AMBA_IK_WB_GAIN_s StillWbInfo[MAX_AEB_NUM] = {0};
    AMBA_IK_WB_GAIN_s WbGain[AMBA_IMG_SENSOR_HAL_HDR_SIZE];

    UINT32 Wgc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];

    UINT32 WbPosition = 0U;
    UINT32 WbId = (UINT32) WB_ID_FE;
    UINT32 Mode = AMBA_IK_PIPE_STILL;

    UINT32 AebIndex;

   (void) IsHiso;

    for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
        WbGain[i].GainR = 4096U;
        WbGain[i].GainG = 4096U;
        WbGain[i].GainB = 4096U;
    }

    if ((pAmbaImgMainChannel[VinId] != NULL) &&
        (pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL) {
            /* hdr info get */
            HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
            ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "still awb iso", VinId, ChainId);
            AmbaImgPrint(PRINT_FLAG_DBG, "hdr enable", HdrEnable);
            AmbaImgPrint(PRINT_FLAG_DBG, "exposure num", ExposureNum);

            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;

            /* image channel get */
            ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
            /* zone id get */
            ImageChanId.Ctx.Bits.ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId);

            /* algo id get */
            AlgoId = AmbaImgMainAwb_AaaAlgoIdGet(ImageChanId);
            AmbaImgPrint(PRINT_FLAG_DBG, "algo id", AlgoId);

            if (IsPiv == 1U) {
                /* piv (r2y), evaluated only */
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
                if (HdrEnable == 0U) {
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
                    AmbaImgPrint(PRINT_FLAG_DBG, "wb position", WbPosition);
                    AmbaImgPrint(PRINT_FLAG_DBG, "wb id", WbId);
                    /* awb info get (TBD) */
                    for (i = 0U; i < ExposureNum; i++) {
                        FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_STILL/*IP_MODE_VIDEO*/, StillWbInfo);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        WbGain[i] = StillWbInfo[0];
                    }
                    /* wb put */
                    Wgc[0] = WbGain[0].GainR;
                    Wgc[1] = WbGain[0].GainG;
                    Wgc[2] = WbGain[0].GainB;
                    /* wb write */
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId, Mode | (((UINT32) WbId) << 16U), Wgc);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
#endif
            } else {
                /* linear (non-hdr) */
                if (HdrEnable == 0U) {
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
                    AmbaImgPrint(PRINT_FLAG_DBG, "wb position", WbPosition);
                    AmbaImgPrint(PRINT_FLAG_DBG, "wb id", WbId);
                    /* op get */
                    FuncRetCode = AmbaImgProc_GetAAAOPInfo(AlgoId, &AAAOpInfo);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* still awb */
                    if ((pCfa != NULL) &&
                        (AAAOpInfo.AwbOp == ENABLE)) {
                        /* vr? */
                        if (pCtx->VrMap.Id.Ctx.Bits.VrId > 0U) {
                            /* disable GAAA */
                            FuncRetCode = AmbaImgProc_GAAASetEnb(AlgoId, 0U);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                        /* cfa put */
                        FuncRetCode = AmbaImgProc_SetCFAAAAStat(AlgoId, pCfa);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* still post (TBD) */
                        FuncRetCode = AmbaImgProc_AWBPostStill(AlgoId, -1);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                    /* aeb index get */
                    AebIndex = AmbaImgMainAwb_SnapAebCount[VinId][ChainId];
                    /* awb info get */
                    for (i = 0U; i < ExposureNum; i++) {
                        FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_STILL, StillWbInfo);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        WbGain[i] = StillWbInfo[AebIndex];
                    }
                    /* aeb index next */
                    if ((pCtx->Snap.Ctx.Bits.AebId > 0U) && (IsPiv == 0U)) {
                        AmbaImgMainAwb_SnapAebCount[VinId][ChainId]++;
                    }
                    /* r2r? */
                    if (IsPiv == 3U) {
                        /* wb put (r2r) */
                        Wgc[0] = 4096U;
                        Wgc[1] = 4096U;
                        Wgc[2] = 4096U;
                    } else {
                        /* wb put */
                        Wgc[0] = WbGain[0].GainR;
                        Wgc[1] = WbGain[0].GainG;
                        Wgc[2] = WbGain[0].GainB;
                    }
                    AmbaImgPrint(PRINT_FLAG_DBG, "gain r", Wgc[0]);
                    AmbaImgPrint(PRINT_FLAG_DBG, "gain g", Wgc[1]);
                    AmbaImgPrint(PRINT_FLAG_DBG, "gain b", Wgc[2]);
                    /* wb write */
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId, Mode | (((UINT32) WbId) << 16U), Wgc);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                } else {
                    /* hdr, wgc in ae */
                }
            }
        }
    }

    return RetCode;
}
