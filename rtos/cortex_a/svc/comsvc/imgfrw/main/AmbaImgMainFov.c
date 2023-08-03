/**
 *  @file AmbaImgMainFov.c
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
 *  @details Amba Image Main Fov
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AeAwbAdj_Control.h"
#include "Amba_Image.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainFov.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_AVM_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR       Data;
    void                         *pVoid;
    const void                   *pCvoid;
    AMBA_IK_CFA_3A_DATA_s        *pCfa;
    AMBA_IK_PG_3A_DATA_s         *pRgb;
    AMBA_IP_GLOBAL_AAA_DATA_s    *pAaaData;
} FOV_MEM_INFO_u;

typedef struct /*_AVM_MEM_INFO_s_*/ {
    FOV_MEM_INFO_u Ctx;
} FOV_MEM_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_FOV_TASK_INFO_s_*/ {
    char Name[32];
} AMBA_IMG_MAIN_FOV_TASK_INFO_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaImgMainFovTaskId[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];
static void AmbaImgMainFov_Task(UINT32 Param);
static void *AmbaImgMainFov_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainFov_Ready[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainFov_Go[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaImgMainFov_Ack[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];

static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainFov_Cmd[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];

AMBA_IMG_CHANNEL_ID_s AmbaImgMainFov_ImageChanId[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];
UINT32 AmbaImgMainFov_LastIndex = 0U;

static UINT32 AmbaImgMainFov_ImgIq[AMBA_IMG_NUM_FOV_ADJ_CHANNEL] = {0};
static UINT32 AmbaImgMainFov_ImgHdr[AMBA_IMG_NUM_FOV_ADJ_CHANNEL] = {0};

static UINT32 AmbaImgMainFov_DummyFlag[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];

UINT32 AmbaImgMainFov_DebugCount[AMBA_IMG_NUM_FOV_ADJ_CHANNEL] = {0};
static UINT32 GNU_SECTION_NOZEROINIT AmbaImgMainFov_IndexStrNum;

static UINT32 AmbaImgMainFov_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion);
static UINT32 AmbaImgMainFov_Delete(UINT32 VinId);
static UINT32 AmbaImgMainFov_Active(UINT32 VinId);
static UINT32 AmbaImgMainFov_Inactive(UINT32 VinId);
static UINT32 AmbaImgMainFov_IdleWait(UINT32 VinId);

/**
 *  Amba image main fov image framework dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainFov_ImgFrwDispatch(UINT32 VinId, UINT64 Param)
{
    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) IMAGE_CMD_CHAN:
                switch (CmdMsg.Ctx.Bits.Msg) {
                    case (UINT8) CHAN_MSG_FOV:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chan-msg-fov", VinId, (UINT32) CmdMsg.Ctx.ImageChanFov.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chain id", VinId, CmdMsg.Ctx.ImageChanFov.ChainId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "adj id", VinId, CmdMsg.Ctx.ImageChanFov.AdjId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "algo id", VinId, CmdMsg.Ctx.ImageChanFov.AlgoId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "zone id", VinId, CmdMsg.Ctx.ImageChanFov.ZoneId);
                        AmbaImgMainFov_ImageChanId[CmdMsg.Ctx.ImageChanFov.AdjId].Ctx.Bits.VinId = (UINT8) VinId;
                        AmbaImgMainFov_ImageChanId[CmdMsg.Ctx.ImageChanFov.AdjId].Ctx.Bits.ChainId = CmdMsg.Ctx.ImageChanFov.ChainId;
                        AmbaImgMainFov_ImageChanId[CmdMsg.Ctx.ImageChanFov.AdjId].Ctx.Bits.AlgoId = CmdMsg.Ctx.ImageChanFov.AlgoId;
                        AmbaImgMainFov_ImageChanId[CmdMsg.Ctx.ImageChanFov.AdjId].Ctx.Bits.ZoneId = CmdMsg.Ctx.ImageChanFov.ZoneId;
                        AmbaImgMainFov_LastIndex = (UINT32) CmdMsg.Ctx.ImageChanFov.AdjId;
                        break;
                    case (UINT8) CHAN_MSG_FOV_IQ:
                        AmbaImgPrint2(PRINT_FLAG_DBG, "chan-msg-fov-iq", VinId, (UINT32) CmdMsg.Ctx.ImageChanFovIq.Msg);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "adj id", VinId, CmdMsg.Ctx.ImageChanFovIq.AdjId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "hdr mode", VinId, CmdMsg.Ctx.ImageChanFovIq.HdrId);
                        AmbaImgPrint2(PRINT_FLAG_DBG, "iq table", VinId, CmdMsg.Ctx.ImageChanFovIq.Id);
                        AmbaImgMainFov_ImgHdr[CmdMsg.Ctx.ImageChanFovIq.AdjId] = (UINT32) CmdMsg.Ctx.ImageChanFovIq.HdrId;
                        AmbaImgMainFov_ImgIq[CmdMsg.Ctx.ImageChanFovIq.AdjId] = (UINT32) CmdMsg.Ctx.ImageChanFovIq.Id;
                        break;
                    default:
                        /* */
                        break;
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main fov algorithm dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainFov_AlgoDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;
    UINT32 AlgoId;
    UINT32 PipeMode = (UINT32) IMAGE_PIPE_NORMAL;
    UINT32 i, j;

    UINT16 ImgIq;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) FOV_CMD_START:
                for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
                    /* fov iq table */
                    if ((AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
                        (AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
                        /* algo id get */
                        AlgoId = AmbaImgMainFov_ImageChanId[i].Ctx.Bits.AlgoId;
                        /* algo to image channel */
                        AmbaImgMain_Algo2ChanTable[AlgoId].Ctx.Data = AmbaImgMainFov_ImageChanId[i].Ctx.Data;
                        /* pipe mode */
                        for (j = 0U; j < AMBA_IMG_NUM_FOV_CHANNEL; j++) {
                            if ((AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId & (((UINT32) 1U) << j)) > 0U) {
                                /* pipe mode get */
                                FuncRetCode = AmbaImgChannel_PipeModeGet(VinId, AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ChainId, j, &PipeMode);
                                if (FuncRetCode != OK_UL) {
                                    /* pipe mode fail */
                                    PipeMode = (UINT32) IMAGE_PIPE_NORMAL;
                                }
                                break;
                            }
                        }
                        AmbaImgPrint2(PRINT_FLAG_MSG, "fov algo pipe mode", AlgoId, PipeMode);
                        /* pipe mode set */
                        if (PipeMode == (UINT32) IMAGE_PIPE_DRAM_EFCY) {
                            FuncRetCode = AmbaImgProc_SetPipelineMode(AlgoId, 4U/*dram efficiency*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        } else {
                            FuncRetCode = AmbaImgProc_SetPipelineMode(AlgoId, 1U/*normal*/);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                        /* hdr mode */
                        AmbaImgPrint2(PRINT_FLAG_MSG, "fov algo hdr mode", AlgoId, AmbaImgMainFov_ImgHdr[i]);
                        FuncRetCode = AmbaImgProc_HDRSetVideoMode(AlgoId, AmbaImgMainFov_ImgHdr[i]);
                        if (FuncRetCode != OK) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main fov hdr mode init");
                            AmbaImgPrint_Flush();
                        }
                        /* linear ce? */
                        if (AmbaImgSystem_Info[AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId][AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ChainId].Pipe.Hdr.Bits.Enable == 0U) {
                            /* non-hdr */
                            AmbaImgPrint2(PRINT_FLAG_MSG, "algo lce", AlgoId, AmbaImgSystem_Info[AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId][AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ChainId].Pipe.Hdr.Bits.LCeEnable);
                            if (AmbaImgSystem_Info[AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId][AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ChainId].Pipe.Hdr.Bits.LCeEnable > 0U) {
                                FuncRetCode = AmbaImgProc_HDRSetLinearCEEnable(AlgoId, 1U);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            } else {
                                FuncRetCode = AmbaImgProc_HDRSetLinearCEEnable(AlgoId, 0U);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            }
                        }
                        /* iq table */
                        AmbaImgPrint2(PRINT_FLAG_MSG, "load fov iq table", AlgoId, AmbaImgMainFov_ImgIq[i]);
                        ImgIq = (UINT16) (AmbaImgMainFov_ImgIq[i] & 0xFFFFU);
                        FuncRetCode = App_Image_Init_Iq_Params(AlgoId, (INT32) ImgIq);
                        if (FuncRetCode != OK) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: image main fov iq param init");
                            AmbaImgPrint_Flush();
                        }
                        AmbaImgPrint2(PRINT_FLAG_MSG, "load fov iq table end", AlgoId, AmbaImgMainFov_ImgIq[i]);
                    }
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  Amba image main fov task dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainFov_TaskDispatch(UINT32 VinId, UINT64 Param)
{
    UINT32 FuncRetCode;

    AMBA_IMG_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;

    if (VinId < AMBA_IMG_NUM_VIN_CHANNEL) {
        switch (CmdMsg.Ctx.Bits.Cmd) {
            case (UINT8) FOV_CMD_START:
                /* fov task create */
                FuncRetCode = AmbaImgMainFov_Create(VinId, 22U, 0x01U);
                if (FuncRetCode == OK_UL) {
                    /* fov task idle wait */
                    FuncRetCode = AmbaImgMainFov_IdleWait(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* fov task active */
                        FuncRetCode = AmbaImgMainFov_Active(VinId);
                        if (FuncRetCode != OK_UL) {
                            AmbaImgPrintStr(PRINT_FLAG_ERR, "error: fov task active");
                        }
                    } else {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: fov task ready");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: fov task create");
                }
                break;
            case (UINT8) FOV_CMD_STOP:
                /* fov task inactive */
                FuncRetCode = AmbaImgMainFov_Inactive(VinId);
                if (FuncRetCode == OK_UL) {
                    /* statistics push */
                    FuncRetCode = AmbaImgStatistics_Push(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: statistics push");
                    }
                    /* fov task idle wait */
                    FuncRetCode = AmbaImgMainFov_IdleWait(VinId);
                    if (FuncRetCode != OK_UL) {
                        AmbaImgPrintStr(PRINT_FLAG_ERR, "error: fov task idle");
                    }
                } else {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: fov task inactive");
                }
                /* fov task delete */
                FuncRetCode = AmbaImgMainFov_Delete(VinId);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrintStr(PRINT_FLAG_ERR, "error: fov task delete");
                }
                break;
            default:
                /* */
                break;
        }
    }
}

/**
 *  @private
 *  Amba image main fov dispatch
 *  @param[in] VinId vin id
 *  @param[in] Param command and message
 *  @note this function is intended for internal use only
 */
void AmbaImgMainFov_Dispatch(UINT32 VinId, UINT64 Param)
{
    /* image framework */
    AmbaImgMainFov_ImgFrwDispatch(VinId, Param);
    /* algorithm */
    AmbaImgMainFov_AlgoDispatch(VinId, Param);
    /* task */
    AmbaImgMainFov_TaskDispatch(VinId, Param);
}

/**
 *  Amba image main fov create
 *  @param[in] VinId vin id
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainFov_Create(UINT32 VinId, UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_IMG_MAIN_FOV_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaImgMainFov_TaskStack[AMBA_IMG_NUM_FOV_ADJ_CHANNEL][AMBA_IMG_MAIN_FOV_TASK_STACK_SIZE];
    static AMBA_IMG_MAIN_FOV_TASK_INFO_s GNU_SECTION_NOZEROINIT AmbaImgMainFov_TaskInfo[AMBA_IMG_NUM_FOV_ADJ_CHANNEL];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
        if ((AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            /* semaphore */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainFov_Ready[i], NULL, 0U);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainFov_Go[i], NULL, 0);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaImgMainFov_Ack[i], NULL, 0);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* task name */
            {
                char str[11];
                UINT32 IndexStrNum = (AmbaImgMainFov_IndexStrNum > 9U) ? (UINT32) 2U : (UINT32) 1U;
                AmbaImgMainFov_TaskInfo[i].Name[0] = '\0';
                (void) var_strcat(AmbaImgMainFov_TaskInfo[i].Name, "ImageFovAdj");
                var_utoa(i, str, 10U, IndexStrNum, (UINT32) VAR_LEADING_ZERO);
                (void) var_strcat(AmbaImgMainFov_TaskInfo[i].Name, str);
            }
            /* task */
            if (RetCode == OK) {
                AmbaImgPrint(PRINT_FLAG_MSG, "image create fov task", i);
                {
                    FOV_MEM_INFO_s MemInfo;
                    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) i;
                    FuncRetCode = AmbaKAL_TaskCreate(&AmbaImgMainFovTaskId[i],
                                                     AmbaImgMainFov_TaskInfo[i].Name,
                                                     Priority,
                                                     AmbaImgMainFov_TaskEx,
                                                     MemInfo.Ctx.pVoid,
                                                     AmbaImgMainFov_TaskStack[i],
                                                     AMBA_IMG_MAIN_FOV_TASK_STACK_SIZE,
                                                     AMBA_KAL_DONT_START);
                }
                if (FuncRetCode == OK) {
                    UINT32 FovCoreInclusion = CoreInclusion;
                    if (AmbaImgMain_ImgAaaCore[VinId][AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ChainId] > 0U) {
                        FovCoreInclusion = AmbaImgMain_ImgAaaCore[VinId][AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ChainId];
                    }
                    FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImgMainFovTaskId[i], FovCoreInclusion);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                    FuncRetCode = AmbaKAL_TaskResume(&AmbaImgMainFovTaskId[i]);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main fov delete
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainFov_Delete(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
        if ((AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            /* task */
            FuncRetCode = AmbaKAL_TaskTerminate(&AmbaImgMainFovTaskId[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskDelete(&AmbaImgMainFovTaskId[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* semaphore */
            FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainFov_Ready[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainFov_Go[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaImgMainFov_Ack[i]);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main fov init
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainFov_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    for (i = 0U; i < AMBA_IMG_NUM_FOV_ADJ_CHANNEL; i++) {
        /* image channel id init */
        AmbaImgMainFov_ImageChanId[i].Ctx.Data = 0ULL;
        /* dummy flag init */
        AmbaImgMainFov_DummyFlag[i] = 1U;
    }
    /* fov debug count reset */
    if (AmbaImgMain_DebugCount == 0U) {
        for (i = 0U; i < AMBA_IMG_NUM_FOV_ADJ_CHANNEL; i++) {
            AmbaImgMainFov_DebugCount[i] = 0xFFFFFFFFU;
        }
    }

    AmbaImgMainFov_IndexStrNum = AMBA_IMG_NUM_FOV_ADJ_CHANNEL;

    return RetCode;
}

/**
 *  Amba image main fov active
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainFov_Active(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
        if ((AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            if (AmbaImgMainFov_Cmd[i] == (UINT32) FOV_TASK_CMD_STOP) {
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainFov_Ready[i], AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* cmd start */
                    AmbaImgMainFov_Cmd[i] = (UINT32) FOV_TASK_CMD_START;
                    /* task go */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainFov_Go[i]);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* ack take */
                        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainFov_Ack[i], AMBA_KAL_WAIT_FOREVER);
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

    return RetCode;
}

/**
 *  Amba image main fov inactive
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainFov_Inactive(UINT32 VinId)
{
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
        if ((AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            if (AmbaImgMainFov_Cmd[i] == (UINT32) FOV_TASK_CMD_START) {
                AmbaImgMainFov_Cmd[i] = (UINT32) FOV_TASK_CMD_STOP;
            }
        }
    }

    return OK_UL;
}

/**
 *  Amba image main fov idle wait
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainFov_IdleWait(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
        if ((AmbaImgMainFov_ImageChanId[i].Ctx.Bits.VinId == VinId) &&
            (AmbaImgMainFov_ImageChanId[i].Ctx.Bits.ZoneId > 0U)) {
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainFov_Ready[i], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainFov_Ready[i]);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image main fov aaa algorithm id get
 *  @param[in] ImageChanId image channel id
 *  @return algorithm  id
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgMainFov_AaaAlgoIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
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
 *  Amba image main fov multi statistics processing
 *  @param[in] pPort pointer to the statisitcs port data
 *  @param[in] FovIdx fov index
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainFov_MultiStatProc(const AMBA_IMG_STATISTICS_PORT_s *pPort, UINT32 FovIdx)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 AlgoId = pPort->ImageChanId.Ctx.Bits.AlgoId;
    UINT32 StatZoneId = pPort->ZoneId.Req;

    UINT32 StatCnt = 0U;

    UINT32 TileRow;
    UINT32 TileCol;

    FOV_MEM_INFO_s MemInfo;
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
                pAaaData->StatisticMap[StatCnt] = AlgoId;

                /* count update */
                StatCnt++;
            }
        }

        /* stat count */
        pAaaData->StatCount = StatCnt;

        /* debug only */
        if (AmbaImgMainFov_DebugCount[FovIdx] < AmbaImgMain_DebugCount) {
            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "fov algo id", pPort->ImageChanId.Ctx.Bits.VinId, AlgoId);
            MemInfo.Ctx.pAaaData = pAaaData;
            AmbaImgPrint64Ex2(PRINT_FLAG_DBG, "algo aaa buffer", (UINT64) pPort->ImageChanId.Ctx.Bits.VinId, 10U, (UINT64) MemInfo.Ctx.Data, 16U);
            j = 0U;
            for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((StatZoneId >> i) > 0U); i++) {
                if ((StatZoneId & (((UINT32) 1U) << i)) > 0U) {
                    AmbaImgPrint2(PRINT_FLAG_DBG, "zone 2 algo", i, AlgoId);
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
 *  Amba image main fov task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaImgMainFov_TaskEx(void *pParam)
{
    FOV_MEM_INFO_s MemInfo;

    MemInfo.Ctx.pVoid = pParam;
    AmbaImgMainFov_Task((UINT32) MemInfo.Ctx.Data);

    return NULL;
}

/**
 *  Amba image main fov task
 *  @param[in] Param fov index
 *  @note this function is intended for internal use only
 */
static void AmbaImgMainFov_Task(UINT32 Param)
{
    UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;

    FOV_MEM_INFO_s MemInfo;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    UINT32 AlgoId = 0U;
    UINT32 ExposureNum = 1U;

    UINT32 AaaAlgoId = 0U;
    AMBA_AE_INFO_s AeInfo[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    AMBA_IK_WB_GAIN_s WbGain[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    static AMBA_IMG_STATISTICS_PORT_s AmbaImgMainFov_StatPort[AMBA_IMG_NUM_FOV_ADJ_CHANNEL] = {0};
    AMBA_IMG_STATISTICS_PORT_s *pPort;

    AMBA_AAA_OP_INFO_s AaaOpInfo;
    UINT32 FovIdx = Param;

    UINT32 ZoneId;
    UINT32 ZoneCnt = 0U;

    UINT32 StatisticsIndex = 0U;

    /* param init */
    ImageChanId.Ctx.Data = 0ULL;

    pPort = &(AmbaImgMainFov_StatPort[FovIdx]);

    AmbaImgMainFov_Cmd[FovIdx] = (UINT32) FOV_TASK_CMD_STOP;

    while (AmbaImgMainFov_DummyFlag[FovIdx] == 1U) {
        if (AmbaImgMainFov_Cmd[FovIdx] == (UINT32) FOV_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                /* task first time create */
                FirstActive = 0U;
            } else {
                /* statistics close */
                FuncRetCode = AmbaImgStatistics_Close(pPort);
                if (FuncRetCode != OK_UL) {
                    AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov statistics close", FovIdx, FuncRetCode);
                }
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainFov_Ready[FovIdx]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov task idle", FovIdx, FuncRetCode);
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaImgMainFov_Go[FovIdx], AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov task go", FovIdx, FuncRetCode);
            }

            /* image channel get */
            ImageChanId.Ctx.Data = AmbaImgMainFov_ImageChanId[FovIdx].Ctx.Data;
            /* algo id get */
            AlgoId = ImageChanId.Ctx.Bits.AlgoId;
            /* aaa algo id */
            AaaAlgoId = AmbaImgMainFov_AaaAlgoIdGet(ImageChanId);

            /* exposure number get */
            ExposureNum = (UINT32) AmbaImgSystem_Info[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].Pipe.Hdr.Bits.ExposureNum;

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint2(PRINT_FLAG_DBG, "exposure num", FovIdx, ExposureNum);

            /* fov ae init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "fov ae algo init...", FovIdx, AlgoId);
            FuncRetCode = AmbaImgProc_AEInit(AlgoId, 0U);
            if (FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov ae algo init", FovIdx, AlgoId);
            }
            /* fov awb init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "fov awb algo init...", FovIdx, AlgoId);
            FuncRetCode = AmbaImgProc_AWBInit(AlgoId, 0U);
            if (FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov awb algo init", FovIdx, AlgoId);
            }

            /* fov adj init */
            AmbaImgPrint2(PRINT_FLAG_MSG, "fov adj algo init...", FovIdx, AlgoId);
            FuncRetCode = Amba_AdjInit(AlgoId);
            if (FuncRetCode != OK) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov adj algo init", FovIdx, AlgoId);
            }

            /* zone cnt & statistics index (single zone) */
            ZoneCnt = 0U;
            ZoneId = ImageChanId.Ctx.Bits.ZoneId;
            for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((ZoneId >> i) > 0U); i++) {
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    if (ZoneCnt == 0U) {
                        StatisticsIndex = i;
                    }
                    ZoneCnt++;
                }
            }

            if (ZoneCnt > 1U) {
                /* global enable for statistics */
                FuncRetCode = AmbaImgProc_GAAASetEnb(AlgoId, 1U);
                if (FuncRetCode != OK) {
                    /* */
                }
            }

            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
            AmbaImgPrint(PRINT_FLAG_DBG, "fov idx", FovIdx);
            AmbaImgPrint(PRINT_FLAG_DBG, "vin id", ImageChanId.Ctx.Bits.VinId);
            AmbaImgPrint(PRINT_FLAG_DBG, "chain id", ImageChanId.Ctx.Bits.ChainId);
            AmbaImgPrint(PRINT_FLAG_DBG, "algo id", AlgoId);
            AmbaImgPrintEx(PRINT_FLAG_DBG, "zone id", ImageChanId.Ctx.Bits.ZoneId, 16U);
            AmbaImgPrint(PRINT_FLAG_DBG, "aaa algo id", AaaAlgoId);

            /* ae/awb preset */
            for (i = 0U; i < ExposureNum; i++) {
                /* ae */
                FuncRetCode = AmbaImgProc_AEGetExpInfo(AaaAlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaImgProc_AESetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* awb */
                FuncRetCode = AmbaImgProc_AWBGetWBGain(AaaAlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaImgProc_AWBSetWBGain(AlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }

            /* adj preset */
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                if (((UINT32) ImageChanId.Ctx.Bits.ZoneId >> i) == 0U) {
                    break;
                }
                if (((UINT32) ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                    FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                    if (FuncRetCode == OK_UL) {
                        FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                        if (FuncRetCode == OK_UL) {
                            ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            AmbaImgPrint2(PRINT_FLAG_DBG, "fov adj context", i, ImgMode.ContextId);
                            AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                            /* tile settings */
                            (void) App_Image_Init_Ae_Tile_Config(AlgoId, &ImgMode);
                            (void) App_Image_Init_Awb_Tile_Config(AlgoId, &ImgMode);
                            (void) App_Image_Init_Af_Tile_Config(AlgoId, &ImgMode);
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
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov statistics open", FovIdx, FuncRetCode);
            }

            /* ack */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaImgMainFov_Ack[FovIdx]);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov task ack", FovIdx, FuncRetCode);
            }
        }

        FuncRetCode = AmbaImgStatistics_Request(pPort, REQUEST_WAIT_FOREVER);
        if (FuncRetCode != OK_UL) {
            AmbaImgPrint2(PRINT_FLAG_ERR, "error: fov statistics request", FovIdx, FuncRetCode);
        } else {
            /* task stop & statistics null? */
            if ((AmbaImgMainFov_Cmd[FovIdx] == (UINT32) FOV_TASK_CMD_STOP) ||
                (pPort->Data.pCfa == NULL) ||
                (pPort->Data.pRgb == NULL)) {
                continue;
            }

            /* aaa op get */
            FuncRetCode = AmbaImgProc_GetAAAOPInfo(AlgoId, &AaaOpInfo);
            if (FuncRetCode != OK_UL) {
                /* */
            }

            /* op enable? */
            if ((AaaOpInfo.AdjOp > 0U) &&
                (ZoneCnt > 0U)) {
                /* stat proc */
                if (ZoneCnt == 1U) {
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
                    AmbaImgMainFov_MultiStatProc(pPort, FovIdx);
                }

                /* fov ae/awb set */
                for (i = 0U; i < ExposureNum; i++) {
                    /* awb */
                    FuncRetCode = AmbaImgProc_AEGetExpInfo(AaaAlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgProc_AESetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(AeInfo[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* awb */
                    FuncRetCode = AmbaImgProc_AWBGetWBGain(AaaAlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgProc_AWBSetWBGain(AlgoId, i, IP_MODE_VIDEO, &(WbGain[i]));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }

                /* hdr? */
                if (ExposureNum > 1U) {
                    /* hdr control */
                    Amba_Adj_VideoHdr_Control(AlgoId, AeInfo, WbGain);
                }

                /* fov adj control */
                Amba_AdjControl(AlgoId);

                /* fov adj set */
                ImageChanId2.Ctx.Data = pPort->ImageChanId.Ctx.Data;
                for (i = 0U; i < AMBA_IMG_NUM_FOV_CHANNEL; i++) {
                    if (((UINT32) ImageChanId.Ctx.Bits.ZoneId >> i) == 0U) {
                        break;
                    }
                    if (((UINT32) ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << i)) > 0U) {
                        ImageChanId2.Ctx.Bits.ZoneId = (UINT32) (((UINT32) 1U) << i);
                        FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
                        if (FuncRetCode == OK_UL) {
                            FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                            if (FuncRetCode == OK_UL) {
                                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                                /* debug only */
                                if (AmbaImgMainFov_DebugCount[FovIdx] < AmbaImgMain_DebugCount) {
                                    AmbaImgPrintStr(PRINT_FLAG_DBG, "\n\n");
                                    AmbaImgPrint2(PRINT_FLAG_DBG, "fov adj context", i, ImgMode.ContextId);
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

            /* debug only */
            if (AmbaImgMainFov_DebugCount[FovIdx] < AmbaImgMain_DebugCount) {
                /* count accumulate */
                AmbaImgMainFov_DebugCount[FovIdx]++;
                if (AmbaImgMainFov_DebugCount[FovIdx] == AmbaImgMain_DebugCount) {
                    /* count reset */
                    AmbaImgMainFov_DebugCount[FovIdx] = 0xFFFFFFFFU;
                }
            }
        }
    }
}
